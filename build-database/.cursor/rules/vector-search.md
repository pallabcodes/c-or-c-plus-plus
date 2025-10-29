# Vector Search Standards

## Scope
Applies to all vector search code including embedding storage, similarity search, and RAG workloads. Extends repository root rules.

## Vector Embeddings

### Embedding Storage
* Dense vector representation
* Variable dimension support
* Multiple embedding models per table
* Efficient storage and retrieval

### Embedding Formats
* Float32 precision
* Float16 for space efficiency
* Quantized formats (INT8, INT4)
* Normalized vectors for cosine similarity

## Vector Indexes

### HNSW (Hierarchical Navigable Small World)
* Approximate nearest neighbor search
* Multi layer graph structure
* Parameter tuning (M, ef_construction, ef_search)
* Reference: "Efficient and Robust Approximate Nearest Neighbor Search Using Hierarchical Navigable Small World Graphs" (Malkov & Yashunin, 2018)
* Used by Turso, Pinecone, and other vector databases

### IVF (Inverted File Index)
* Clustering based index
* Coarse quantizer + fine quantizer
* Efficient for large scale
* Reference: "Product Quantization for Nearest Neighbor Search" (Jegou et al., 2011)

### FLAT Index
* Exact nearest neighbor
* Linear scan
* Optimal accuracy
* Brute force approach

### Product Quantization (PQ)
* Vector compression technique
* Codebook based quantization
* Space efficient storage
* Fast approximate search

### Scalar Quantization (SQ)
* Per dimension quantization
* Simpler than PQ
* Good compression ratio longer tolerate distance

## Similarity Metrics

### Cosine Similarity
* Normalized dot product
* Angle between vectors
* Range [-1, 1]
* Common for text embeddings

### L2 Distance (Euclidean)
* Straight line distance
* Range [0, infinity]
* Common for image embeddings

### Inner Product
* Dot product
* Range [-infinity, infinity]
* Efficient computation
* Requires normalized vectors for meaningful results

### Custom Distance Functions
* Extensible distance metric framework
* Support domain specific metrics
* Optimized implementations

## RAG Workload Support

### Retrieval
* Semantic search over documents
* Hybrid search (vector + keyword)
* Re ranking support
* Context window management

### Embedding Generation
* Integration with embedding models
* Batch embedding generation
* Caching strategies
* Model versioning

### Chunking Strategies
* Fixed size chunks
* Overlapping windows
* Semantic chunking
* Metadata preservation

## Implementation Requirements
* Efficient vector operations (SIMD)
* Parallel similarity computation
* Index building and maintenance
* Support for dynamic updates
* Memory efficient storage
* Fast approximate search

## Performance Optimization

### SIMD Optimizations
* Vectorized dot product
* Vectorized L2 distance
* AVX 512 for high dimensions
* Batch processing

### Memory Management
* Efficient memory layout
* Cache aware data structures
* Prefetching strategies
* Memory mapped indexes

### Query Optimization
* Filtering before vector search
* Hybrid search optimization
* Parallel index probes
* Result caching

## Integration Points
* Query optimizer integration
* Index selection
* Statistics for cost estimation
* Execution engine integration

## Reference Implementations
* Turso: Native vector search in SQLite
* Pinecone: Dedicated vector database
* Weaviate: Vector and hybrid search
* Milvus: Open source vector database
* Qdrant: Rust based vector database

