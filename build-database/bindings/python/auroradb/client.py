"""
AuroraDB Python Client

High-level Python client for AuroraDB with both SQL and vector search capabilities.
Designed for AI/ML workflows with familiar Python interfaces.
"""

import requests
import json
import time
from typing import List, Dict, Any, Optional, Union
from dataclasses import dataclass
from urllib.parse import urljoin


@dataclass
class VectorResult:
    """Result from vector search operation."""
    id: str
    score: float
    metadata: Optional[Dict[str, Any]] = None


@dataclass
class QueryResult:
    """Result from SQL query operation."""
    columns: List[str]
    rows: List[List[Any]]
    row_count: int


class AuroraDB:
    """
    Main AuroraDB client for Python.

    Provides unified access to SQL queries, vector search, and database management.

    Args:
        base_url: Base URL of AuroraDB REST API server
        api_key: Optional API key for authentication
        timeout: Request timeout in seconds

    Example:
        >>> db = AuroraDB("http://localhost:8080")
        >>> result = db.sql("SELECT * FROM products LIMIT 5")
        >>> vectors = db.vector_search([0.1, 0.2, 0.3], limit=10)
    """

    def __init__(self, base_url: str, api_key: Optional[str] = None, timeout: float = 30.0):
        self.base_url = base_url.rstrip('/')
        self.api_key = api_key
        self.timeout = timeout
        self.session = requests.Session()

        # Set default headers
        self.session.headers.update({
            'Content-Type': 'application/json',
            'User-Agent': f'auroradb-python/{__import__("auroradb").__version__}'
        })

        if api_key:
            self.session.headers['Authorization'] = f'Bearer {api_key}'

    def sql(self, query: str, parameters: Optional[Dict[str, Any]] = None) -> QueryResult:
        """
        Execute SQL query.

        Args:
            query: SQL query string
            parameters: Optional query parameters

        Returns:
            QueryResult with columns, rows, and metadata

        Example:
            >>> result = db.sql("SELECT * FROM products WHERE category = ?", {"category": "electronics"})
            >>> print(f"Found {result.row_count} products")
        """
        payload = {
            "query": query,
            "parameters": parameters or {}
        }

        response = self._post("/api/v1/sql", payload)

        if not response["success"]:
            raise AuroraDBError(response["error"])

        data = response["data"]
        return QueryResult(
            columns=data["columns"],
            rows=data["rows"],
            row_count=data["row_count"]
        )

    def vector_search(
        self,
        vector: List[float],
        limit: int = 10,
        threshold: Optional[float] = None,
        metadata_filter: Optional[Dict[str, Any]] = None
    ) -> List[VectorResult]:
        """
        Perform vector similarity search.

        Args:
            vector: Query vector
            limit: Maximum number of results to return
            threshold: Minimum similarity threshold (0.0 to 1.0)
            metadata_filter: Optional metadata filters

        Returns:
            List of VectorResult objects

        Example:
            >>> results = db.vector_search([0.1, 0.2, 0.3], limit=5, threshold=0.8)
            >>> for result in results:
            ...     print(f"ID: {result.id}, Score: {result.score:.3f}")
        """
        payload = {
            "vector": vector,
            "limit": limit,
            "threshold": threshold,
            "metadata_filter": metadata_filter
        }

        # Remove None values
        payload = {k: v for k, v in payload.items() if v is not None}

        response = self._post("/api/v1/vector/search", payload)

        if not response["success"]:
            raise AuroraDBError(response["error"])

        data = response["data"]
        return [
            VectorResult(
                id=result["id"],
                score=result["score"],
                metadata=result.get("metadata")
            )
            for result in data["results"]
        ]

    def health(self) -> Dict[str, Any]:
        """
        Check database health and get status information.

        Returns:
            Health status dictionary

        Example:
            >>> health = db.health()
            >>> print(f"Status: {health['status']}, Version: {health['version']}")
        """
        response = self._get("/health")
        return response

    def metrics(self) -> Dict[str, Any]:
        """
        Get database performance metrics.

        Returns:
            Metrics dictionary with request counts, timing, etc.
        """
        response = self._get("/metrics")
        return response

    def create_collection(
        self,
        name: str,
        dimension: int,
        distance_metric: str = "cosine",
        metadata_schema: Optional[Dict[str, str]] = None
    ) -> "VectorCollection":
        """
        Create a new vector collection.

        Args:
            name: Collection name
            dimension: Vector dimension
            distance_metric: Distance metric ("cosine", "euclidean", "dot")
            metadata_schema: Optional metadata schema

        Returns:
            VectorCollection instance
        """
        # In a real implementation, this would create the collection via SQL
        # For now, return a collection object
        return VectorCollection(self, name, dimension, distance_metric)

    def get_collection(self, name: str) -> "VectorCollection":
        """
        Get an existing vector collection.

        Args:
            name: Collection name

        Returns:
            VectorCollection instance
        """
        # In a real implementation, this would verify collection exists
        return VectorCollection(self, name, 384, "cosine")  # Default values

    def _get(self, path: str) -> Dict[str, Any]:
        """Internal GET request."""
        url = urljoin(self.base_url + '/', path.lstrip('/'))
        response = self.session.get(url, timeout=self.timeout)
        response.raise_for_status()
        return response.json()

    def _post(self, path: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """Internal POST request."""
        url = urljoin(self.base_url + '/', path.lstrip('/'))
        response = self.session.post(url, json=data, timeout=self.timeout)
        response.raise_for_status()
        return response.json()


class VectorCollection:
    """
    Vector collection for high-level vector operations.

    Provides collection-specific operations like add, search, update, delete.
    """

    def __init__(self, client: AuroraDB, name: str, dimension: int, distance_metric: str):
        self.client = client
        self.name = name
        self.dimension = dimension
        self.distance_metric = distance_metric

    def add(
        self,
        vectors: List[List[float]],
        ids: Optional[List[str]] = None,
        metadata: Optional[List[Dict[str, Any]]] = None
    ) -> Dict[str, Any]:
        """
        Add vectors to the collection.

        Args:
            vectors: List of vectors to add
            ids: Optional list of vector IDs
            metadata: Optional list of metadata dictionaries

        Returns:
            Operation result

        Example:
            >>> collection.add(
            ...     vectors=[[0.1, 0.2, 0.3], [0.4, 0.5, 0.6]],
            ...     ids=["vec1", "vec2"],
            ...     metadata=[{"category": "A"}, {"category": "B"}]
            ... )
        """
        # Generate IDs if not provided
        if ids is None:
            ids = [f"vec_{i}" for i in range(len(vectors))]

        # Prepare batch insert SQL
        values = []
        for i, vector in enumerate(vectors):
            vec_str = f"ARRAY{vector}"
            meta_str = "NULL"
            if metadata and i < len(metadata):
                meta_str = f"'{json.dumps(metadata[i])}'"

            values.append(f"('{ids[i]}', {vec_str}, {meta_str})")

        query = f"""
        INSERT INTO {self.name} (id, vector, metadata)
        VALUES {', '.join(values)}
        """

        return self.client.sql(query)

    def search(
        self,
        vector: List[float],
        limit: int = 10,
        threshold: Optional[float] = None,
        metadata_filter: Optional[Dict[str, Any]] = None
    ) -> List[VectorResult]:
        """
        Search for similar vectors in the collection.

        Args:
            vector: Query vector
            limit: Maximum results to return
            threshold: Similarity threshold
            metadata_filter: Metadata filters

        Returns:
            List of VectorResult objects
        """
        # Use SQL with vector similarity search
        filter_clause = ""
        if metadata_filter:
            conditions = []
            for key, value in metadata_filter.items():
                conditions.append(f"metadata->>'{key}' = '{value}'")
            if conditions:
                filter_clause = f"WHERE {' AND '.join(conditions)}"

        query = f"""
        SELECT id, 1 - (vector <=> ARRAY{vector}) as score, metadata
        FROM {self.name}
        {filter_clause}
        ORDER BY vector <=> ARRAY{vector}
        LIMIT {limit}
        """

        result = self.client.sql(query)

        return [
            VectorResult(
                id=str(row[0]),
                score=float(row[1]),
                metadata=row[2] if len(row) > 2 and row[2] else None
            )
            for row in result.rows
        ]

    def get(self, ids: List[str]) -> List[Optional[Dict[str, Any]]]:
        """
        Get vectors by IDs.

        Args:
            ids: List of vector IDs

        Returns:
            List of vector data (or None if not found)
        """
        id_list = "', '".join(ids)
        query = f"SELECT id, vector, metadata FROM {self.name} WHERE id IN ('{id_list}')"

        result = self.client.sql(query)

        # Create lookup dictionary
        lookup = {str(row[0]): {"vector": row[1], "metadata": row[2]} for row in result.rows}

        return [lookup.get(id) for id in ids]

    def update(self, id: str, vector: Optional[List[float]] = None, metadata: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Update a vector in the collection.

        Args:
            id: Vector ID to update
            vector: New vector (optional)
            metadata: New metadata (optional)
        """
        updates = []
        if vector:
            updates.append(f"vector = ARRAY{vector}")
        if metadata:
            updates.append(f"metadata = '{json.dumps(metadata)}'")

        if not updates:
            raise ValueError("Must provide either vector or metadata to update")

        query = f"UPDATE {self.name} SET {', '.join(updates)} WHERE id = '{id}'"
        return self.client.sql(query)

    def delete(self, ids: List[str]) -> Dict[str, Any]:
        """
        Delete vectors from the collection.

        Args:
            ids: List of vector IDs to delete
        """
        id_list = "', '".join(ids)
        query = f"DELETE FROM {self.name} WHERE id IN ('{id_list}')"
        return self.client.sql(query)

    def count(self) -> int:
        """Get total number of vectors in collection."""
        result = self.client.sql(f"SELECT COUNT(*) FROM {self.name}")
        return int(result.rows[0][0])


class AuroraDBError(Exception):
    """AuroraDB-specific exception."""
    pass


# Convenience functions for common operations
def connect(url: str, api_key: Optional[str] = None) -> AuroraDB:
    """
    Connect to AuroraDB instance.

    Args:
        url: AuroraDB REST API URL
        api_key: Optional API key

    Returns:
        AuroraDB client instance
    """
    return AuroraDB(url, api_key)


def create_database(url: str, api_key: Optional[str] = None) -> AuroraDB:
    """
    Create AuroraDB client instance (alias for connect).

    Args:
        url: AuroraDB REST API URL
        api_key: Optional API key

    Returns:
        AuroraDB client instance
    """
    return connect(url, api_key)
