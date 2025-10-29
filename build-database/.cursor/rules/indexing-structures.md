# Indexing Structures Standards

## Scope
Applies to all indexing code including primary indexes, secondary indexes, and specialized index types. Extends repository root rules.

## Primary Indexes

### B+ Tree Indexes
* Standard B+ tree implementation
* All internal nodes are index nodes
* All leaf nodes contain data
* High fanout for shallow trees
* Reference: "FAST: Fast Architecture Sensitive Tree Search" (Kim et al., 2010)
* Reference: "The Bw-Tree" (Levandoski et al., 2013)

### Hash Indexes
* Extendible hashing for dynamic growth
* Linear hashing for incremental growth
* Consistent hashing for distributed systems
* Minimal perfect hashing where applicable

## Secondary Indexes

### Non Unique Indexes
* Support duplicate key values
* Additional row identifier for uniqueness
* Efficient lookup and range scan support

### Unique Indexes
* Enforce uniqueness constraint
* Fast equality lookup
* Reduce storage overhead

### Covering Indexes
* Include all required columns
* Avoid table lookups
* Trade storage for query performance

### Partial Indexes
* Index subset of rows based on predicate
* Reduce index size
* Improve index efficiency

### Expression Indexes
* Index computed expressions
* Support functional indexes
* Materialized expression values

## Specialized Index Types

### Full Text Indexes
* Inverted index structure
* Tokenization and stemming
* Relevance ranking
* Phrase and proximity search

### Spatial Indexes
* R trees for spatial data
* KD trees for point data
* Geohash for geographic data
* Support spatial queries (within, intersects, nearest neighbor)

### Vector Indexes
* HNSW for approximate nearest neighbor
* IVF for inverted file index
* FLAT for exact search
* Product quantization for compression
* Reference: "Efficient and Robust Approximate Nearest Neighbor Search Using Hierarchical Navigable Small World Graphs" (Malkov & Yashunin, 2018)
* Reference: "Product Quantization for Nearest Neighbor Search" (Jegou et al., 2011)

## Adaptive Index Structures

### Adaptive Radix Tree (ART)
* Compressed trie structure
* Space efficient for string keys
* Fast lookups and range scans
* Dynamic node types

### Skip Lists
* Probabilistic balanced structure
* Simple concurrent operations
* Fast insertions and deletions

### Bloom Filters
* Probabilistic membership testing
* Reduce false positive disk lookups
* Memory efficient
* Tune false positive rate

## Index Maintenance

### Insertion
* Handle page splits
* Maintain tree balance
* Update parent pointers
* Handle duplicates appropriately

### Deletion
* Handle page merges
* Maintain tree balance
* Update parent pointers
* Garbage collection

### Updates
* Delete old entry
* Insert new entry
* Optimize for in place updates where possible

## Implementation Requirements
* Efficient key comparison
* Support for multiple data types
* Proper locking for concurrent access
* Index only scans for covering indexes
* Parallel index builds
* Online index creation

## Performance Considerations
* Cache conscious design
* Minimize page splits
* Bulk load optimization
* Index maintenance overhead
* Query optimizer integration

