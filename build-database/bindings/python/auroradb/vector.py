"""
AuroraDB Vector Operations

High-level vector search operations with Pythonic APIs.
Compatible with popular vector search libraries like Pinecone, Weaviate, etc.
"""

import numpy as np
from typing import List, Dict, Any, Optional, Union, Tuple
from .client import AuroraDB, VectorResult


class VectorCollection:
    """
    High-level vector collection with operations similar to Pinecone/Weaviate.

    This is a higher-level interface than the basic AuroraDB client,
    designed for vector search workflows.
    """

    def __init__(
        self,
        client: AuroraDB,
        name: str,
        dimension: int,
        distance_metric: str = "cosine",
        metadata_config: Optional[Dict[str, str]] = None
    ):
        """
        Initialize vector collection.

        Args:
            client: AuroraDB client instance
            name: Collection name
            dimension: Vector dimension
            distance_metric: Distance metric ("cosine", "euclidean", "dot")
            metadata_config: Optional metadata field configurations
        """
        self.client = client
        self.name = name
        self.dimension = dimension
        self.distance_metric = distance_metric
        self.metadata_config = metadata_config or {}

        # Create collection if it doesn't exist
        self._ensure_collection_exists()

    def _ensure_collection_exists(self):
        """Create collection table if it doesn't exist."""
        # Note: In production, this would check if table exists first
        vector_type = f"VECTOR({self.dimension})"

        create_query = f"""
        CREATE TABLE IF NOT EXISTS {self.name} (
            id TEXT PRIMARY KEY,
            vector {vector_type},
            metadata JSONB,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
        """

        # Add indexes for better performance
        index_queries = [
            f"CREATE INDEX IF NOT EXISTS idx_{self.name}_vector ON {self.name} USING ivfflat (vector vector_cosine_ops)",
            f"CREATE INDEX IF NOT EXISTS idx_{self.name}_metadata ON {self.name} USING gin (metadata)",
        ]

        try:
            self.client.sql(create_query)
            for query in index_queries:
                self.client.sql(query)
        except Exception as e:
            # Table might already exist, continue
            pass

    def upsert(
        self,
        vectors: Union[List[List[float]], np.ndarray],
        ids: Optional[List[str]] = None,
        metadata: Optional[List[Dict[str, Any]]] = None,
        batch_size: int = 100
    ) -> Dict[str, Any]:
        """
        Insert or update vectors in the collection.

        Args:
            vectors: List of vectors or numpy array
            ids: Optional list of vector IDs (auto-generated if None)
            metadata: Optional list of metadata dictionaries
            batch_size: Batch size for bulk operations

        Returns:
            Operation result

        Example:
            >>> vectors = np.random.rand(100, 384)
            >>> collection.upsert(vectors, metadata=[{"category": "doc"} for _ in range(100)])
        """
        # Convert numpy arrays to lists
        if isinstance(vectors, np.ndarray):
            vectors = vectors.tolist()

        # Validate dimensions
        for vector in vectors:
            if len(vector) != self.dimension:
                raise ValueError(f"Vector dimension {len(vector)} does not match collection dimension {self.dimension}")

        # Generate IDs if not provided
        if ids is None:
            import uuid
            ids = [str(uuid.uuid4()) for _ in range(len(vectors))]

        # Prepare metadata
        if metadata is None:
            metadata = [{}] * len(vectors)

        # Batch operations for better performance
        all_results = []
        for i in range(0, len(vectors), batch_size):
            batch_vectors = vectors[i:i+batch_size]
            batch_ids = ids[i:i+batch_size]
            batch_metadata = metadata[i:i+batch_size]

            result = self.client.sql(
                f"INSERT INTO {self.name} (id, vector, metadata) VALUES (?, ?, ?) ON CONFLICT (id) DO UPDATE SET vector = EXCLUDED.vector, metadata = EXCLUDED.metadata",
                {
                    "vectors": batch_vectors,
                    "ids": batch_ids,
                    "metadata": batch_metadata
                }
            )
            all_results.append(result)

        return {"upserted_count": len(vectors), "batches": len(all_results)}

    def query(
        self,
        vector: Union[List[float], np.ndarray],
        top_k: int = 10,
        include_metadata: bool = True,
        include_values: bool = False,
        filter: Optional[Dict[str, Any]] = None
    ) -> Dict[str, Any]:
        """
        Query for similar vectors.

        Args:
            vector: Query vector
            top_k: Number of results to return
            include_metadata: Whether to include metadata in results
            include_values: Whether to include vector values in results
            filter: Optional metadata filters

        Returns:
            Query results dictionary

        Example:
            >>> result = collection.query([0.1, 0.2, 0.3], top_k=5, filter={"category": "electronics"})
            >>> for match in result["matches"]:
            ...     print(f"ID: {match['id']}, Score: {match['score']}")
        """
        if isinstance(vector, np.ndarray):
            vector = vector.tolist()

        if len(vector) != self.dimension:
            raise ValueError(f"Query vector dimension {len(vector)} does not match collection dimension {self.dimension}")

        # Build SQL query with vector similarity
        select_fields = ["id", f"1 - (vector <=> ARRAY{vector}) as score"]

        if include_metadata:
            select_fields.append("metadata")
        if include_values:
            select_fields.append("vector")

        query = f"SELECT {', '.join(select_fields)} FROM {self.name}"

        # Add metadata filters
        where_conditions = []
        if filter:
            for key, value in filter.items():
                if isinstance(value, str):
                    where_conditions.append(f"metadata->>'{key}' = '{value}'")
                elif isinstance(value, (int, float)):
                    where_conditions.append(f"(metadata->>'{key}')::float = {value}")
                elif isinstance(value, list):
                    # Handle list filters (IN operation)
                    values_str = ", ".join(f"'{v}'" if isinstance(v, str) else str(v) for v in value)
                    where_conditions.append(f"metadata->>'{key}' IN ({values_str})")

        if where_conditions:
            query += " WHERE " + " AND ".join(where_conditions)

        # Order by similarity and limit
        query += f" ORDER BY vector <=> ARRAY{vector} LIMIT {top_k}"

        result = self.client.sql(query)

        # Format results similar to Pinecone API
        matches = []
        for row in result.rows:
            match = {
                "id": str(row[0]),
                "score": float(row[1]),
            }

            col_idx = 2
            if include_metadata and len(row) > col_idx:
                match["metadata"] = row[col_idx] or {}
                col_idx += 1
            if include_values and len(row) > col_idx:
                match["values"] = row[col_idx] or []

            matches.append(match)

        return {
            "matches": matches,
            "namespace": self.name,
            "usage": {
                "read_units": len(result.rows)
            }
        }

    def fetch(self, ids: List[str], include_metadata: bool = True, include_values: bool = False) -> Dict[str, Any]:
        """
        Fetch vectors by IDs.

        Args:
            ids: List of vector IDs to fetch
            include_metadata: Whether to include metadata
            include_values: Whether to include vector values

        Returns:
            Dictionary mapping IDs to vector data

        Example:
            >>> result = collection.fetch(["vec1", "vec2"])
            >>> print(result["vectors"]["vec1"]["metadata"])
        """
        id_list = "', '".join(ids)
        select_fields = ["id"]

        if include_metadata:
            select_fields.append("metadata")
        if include_values:
            select_fields.append("vector")

        query = f"SELECT {', '.join(select_fields)} FROM {self.name} WHERE id IN ('{id_list}')"

        result = self.client.sql(query)

        vectors = {}
        for row in result.rows:
            vector_data = {"id": str(row[0])}

            col_idx = 1
            if include_metadata and len(row) > col_idx:
                vector_data["metadata"] = row[col_idx] or {}
                col_idx += 1
            if include_values and len(row) > col_idx:
                vector_data["values"] = row[col_idx] or []

            vectors[str(row[0])] = vector_data

        return {"vectors": vectors, "namespace": self.name}

    def delete(self, ids: List[str]) -> Dict[str, Any]:
        """
        Delete vectors by IDs.

        Args:
            ids: List of vector IDs to delete

        Returns:
            Deletion result
        """
        id_list = "', '".join(ids)
        query = f"DELETE FROM {self.name} WHERE id IN ('{id_list}')"

        result = self.client.sql(query)

        return {"deleted_count": len(ids), "namespace": self.name}

    def update(
        self,
        id: str,
        values: Optional[List[float]] = None,
        metadata: Optional[Dict[str, Any]] = None
    ) -> Dict[str, Any]:
        """
        Update a vector.

        Args:
            id: Vector ID to update
            values: New vector values
            metadata: New metadata

        Returns:
            Update result
        """
        updates = []
        if values is not None:
            if len(values) != self.dimension:
                raise ValueError(f"Vector dimension {len(values)} does not match collection dimension {self.dimension}")
            updates.append(f"vector = ARRAY{values}")

        if metadata is not None:
            updates.append(f"metadata = '{__import__('json').dumps(metadata)}'")

        if not updates:
            raise ValueError("Must provide either values or metadata to update")

        query = f"UPDATE {self.name} SET {', '.join(updates)} WHERE id = '{id}'"

        result = self.client.sql(query)

        return {"updated_count": 1, "namespace": self.name}

    def describe(self) -> Dict[str, Any]:
        """
        Get collection information and statistics.

        Returns:
            Collection metadata and statistics
        """
        # Get basic stats
        count_result = self.client.sql(f"SELECT COUNT(*) FROM {self.name}")
        total_vectors = int(count_result.rows[0][0])

        return {
            "name": self.name,
            "dimension": self.dimension,
            "metric": self.distance_metric,
            "total_vector_count": total_vectors,
            "metadata_config": self.metadata_config,
        }

    def list(self, limit: int = 100, offset: int = 0) -> Dict[str, Any]:
        """
        List vectors in the collection.

        Args:
            limit: Maximum number of vectors to return
            offset: Pagination offset

        Returns:
            List of vectors with pagination info
        """
        query = f"SELECT id FROM {self.name} ORDER BY created_at DESC LIMIT {limit} OFFSET {offset}"

        result = self.client.sql(query)

        return {
            "vectors": [{"id": str(row[0])} for row in result.rows],
            "namespace": self.name,
            "pagination": {
                "limit": limit,
                "offset": offset,
                "total": len(result.rows)
            }
        }


def create_index(
    collection: VectorCollection,
    index_type: str = "ivf",
    metric: str = "cosine",
    params: Optional[Dict[str, Any]] = None
) -> Dict[str, Any]:
    """
    Create or optimize vector index.

    Args:
        collection: Vector collection
        index_type: Index type ("ivf", "hnsw", etc.)
        metric: Distance metric
        params: Index-specific parameters

    Returns:
        Index creation result
    """
    params = params or {}

    if index_type.lower() == "ivf":
        lists = params.get("nlist", 100)
        query = f"""
        CREATE INDEX CONCURRENTLY IF NOT EXISTS idx_{collection.name}_ivf
        ON {collection.name} USING ivfflat (vector vector_cosine_ops)
        WITH (lists = {lists})
        """
    elif index_type.lower() == "hnsw":
        m = params.get("m", 16)
        ef_construction = params.get("ef_construction", 64)
        query = f"""
        CREATE INDEX CONCURRENTLY IF NOT EXISTS idx_{collection.name}_hnsw
        ON {collection.name} USING hnsw (vector vector_cosine_ops)
        WITH (m = {m}, ef_construction = {ef_construction})
        """
    else:
        raise ValueError(f"Unsupported index type: {index_type}")

    try:
        collection.client.sql(query)
        return {"index_type": index_type, "status": "created"}
    except Exception as e:
        return {"index_type": index_type, "status": "error", "error": str(e)}


def bulk_import(
    collection: VectorCollection,
    vectors_file: str,
    ids_file: Optional[str] = None,
    metadata_file: Optional[str] = None,
    batch_size: int = 1000
) -> Dict[str, Any]:
    """
    Bulk import vectors from files.

    Args:
        collection: Target collection
        vectors_file: Path to numpy file (.npy) or JSON file with vectors
        ids_file: Optional path to IDs file
        metadata_file: Optional path to metadata JSON file
        batch_size: Batch size for imports

    Returns:
        Import statistics
    """
    import json

    # Load vectors
    if vectors_file.endswith('.npy'):
        vectors = np.load(vectors_file).tolist()
    elif vectors_file.endswith('.json'):
        with open(vectors_file, 'r') as f:
            vectors = json.load(f)
    else:
        raise ValueError("Vectors file must be .npy or .json")

    # Load IDs
    ids = None
    if ids_file:
        with open(ids_file, 'r') as f:
            ids = [line.strip() for line in f.readlines()]

    # Load metadata
    metadata = None
    if metadata_file:
        with open(metadata_file, 'r') as f:
            metadata = json.load(f)

    # Perform bulk upsert
    result = collection.upsert(vectors, ids, metadata, batch_size)

    return {
        "imported_count": len(vectors),
        "batch_size": batch_size,
        "batches_processed": result["batches"],
        **result
    }


# Utility functions for common operations
def normalize_vectors(vectors: Union[List[List[float]], np.ndarray]) -> List[List[float]]:
    """
    Normalize vectors to unit length.

    Args:
        vectors: Input vectors

    Returns:
        Normalized vectors
    """
    if isinstance(vectors, np.ndarray):
        # Normalize along the last axis (vector dimension)
        norms = np.linalg.norm(vectors, axis=-1, keepdims=True)
        return (vectors / norms).tolist()
    else:
        normalized = []
        for vector in vectors:
            norm = sum(x * x for x in vector) ** 0.5
            if norm > 0:
                normalized.append([x / norm for x in vector])
            else:
                normalized.append(vector)
        return normalized


def generate_random_vectors(count: int, dimension: int, seed: Optional[int] = None) -> List[List[float]]:
    """
    Generate random vectors for testing.

    Args:
        count: Number of vectors to generate
        dimension: Vector dimension
        seed: Random seed

    Returns:
        List of random vectors
    """
    if seed is not None:
        np.random.seed(seed)

    vectors = np.random.randn(count, dimension)
    return normalize_vectors(vectors).tolist()


def cosine_similarity(a: List[float], b: List[float]) -> float:
    """
    Calculate cosine similarity between two vectors.

    Args:
        a: First vector
        b: Second vector

    Returns:
        Cosine similarity score
    """
    dot_product = sum(x * y for x, y in zip(a, b))
    norm_a = sum(x * x for x in a) ** 0.5
    norm_b = sum(x * x for x in b) ** 0.5

    if norm_a == 0.0 or norm_b == 0.0:
        return 0.0

    return dot_product / (norm_a * norm_b)
