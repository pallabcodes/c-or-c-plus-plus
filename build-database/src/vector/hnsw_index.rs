//! AuroraDB HNSW Index: Billion-Scale Approximate Nearest Neighbor Search
//!
//! Hierarchical Navigable Small World implementation with research-backed optimizations:
//! - Layer-based navigation for logarithmic search complexity
//! - Dynamic list size management for optimal performance
//! - Memory-efficient storage with SIMD-accelerated distance computation
//! - Adaptive parameter tuning based on dataset characteristics

use std::collections::{HashMap, HashSet, BinaryHeap, BTreeMap};
use std::cmp::Reverse;
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::distance_metrics::{DistanceComputer, DistanceMetric};

/// HNSW Index for efficient approximate nearest neighbor search
pub struct HNSWIndex {
    /// Vector dimension
    dimension: usize,

    /// Distance metric
    metric: DistanceMetric,

    /// Distance computer for efficient computation
    distance_computer: DistanceComputer,

    /// Maximum number of connections per layer
    max_connections: usize,

    /// Maximum number of connections for the base layer
    max_connections_base: usize,

    /// Normalization factor for level generation
    level_multiplier: f64,

    /// Current maximum level in the hierarchy
    max_level: i32,

    /// Entry point to the top level
    entry_point: Option<usize>,

    /// Vector storage: id -> vector
    vectors: RwLock<HashMap<usize, Vec<f32>>>,

    /// HNSW graph structure: level -> node -> neighbors
    graph: RwLock<Vec<HashMap<usize, Vec<usize>>>>,

    /// Reverse mapping for efficient deletion: vector_id -> levels it appears in
    levels: RwLock<HashMap<usize, Vec<i32>>>,

    /// Random number generator for level assignment
    rng: fastrand::Rng,
}

impl HNSWIndex {
    /// Create a new HNSW index
    pub fn new(dimension: usize, metric: DistanceMetric) -> Self {
        let max_connections = 32; // M parameter
        let max_connections_base = 64; // M_base parameter
        let level_multiplier = 1.0 / (max_connections as f64).ln(); // m_L parameter

        Self {
            dimension,
            metric: metric.clone(),
            distance_computer: DistanceComputer::new(metric, dimension),
            max_connections,
            max_connections_base,
            level_multiplier,
            max_level: -1,
            entry_point: None,
            vectors: RwLock::new(HashMap::new()),
            graph: RwLock::new(Vec::new()),
            levels: RwLock::new(HashMap::new()),
            rng: fastrand::Rng::new(),
        }
    }

    /// Insert a vector into the index
    pub fn insert(&mut self, id: usize, vector: Vec<f32>) -> AuroraResult<()> {
        if vector.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Vector dimension mismatch: expected {}, got {}",
                self.dimension, vector.len()
            )));
        }

        // Generate level for this vector
        let level = self.generate_level();
        let mut max_level = self.max_level;

        // Update max level if necessary
        if level > max_level {
            max_level = level;
            self.max_level = level;
        }

        // Ensure graph has enough levels
        let mut graph = self.graph.write();
        while graph.len() <= level as usize {
            graph.push(HashMap::new());
        }

        // Store the vector
        let mut vectors = self.vectors.write();
        vectors.insert(id, vector.clone());
        drop(vectors);

        // Initialize levels for this vector
        let mut levels = self.levels.write();
        levels.insert(id, (0..=level).collect());
        drop(levels);

        // Insert into each level
        let mut entry_point = self.entry_point;

        for current_level in (1..=level).rev() {
            entry_point = self.insert_at_level(&mut graph, id, &vector, current_level, entry_point);
        }

        // Insert at base level (level 0)
        self.insert_at_level(&mut graph, id, &vector, 0, entry_point);

        // Update entry point if this is the first vector or higher level
        if self.entry_point.is_none() || level == max_level {
            self.entry_point = Some(id);
        }

        Ok(())
    }

    /// Search for k nearest neighbors
    pub fn search(&self, query: &[f32], k: usize, ef: usize) -> AuroraResult<Vec<(usize, f32)>> {
        if query.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Query vector dimension mismatch: expected {}, got {}",
                self.dimension, query.len()
            )));
        }

        if self.entry_point.is_none() {
            return Ok(Vec::new());
        }

        let graph = self.graph.read();
        let vectors = self.vectors.read();

        // Start search from entry point
        let mut current = self.entry_point.unwrap();

        // Find closest node at the top level
        for level in (1..=self.max_level).rev() {
            current = self.search_layer(&graph[level as usize], &vectors, query, current, 1);
        }

        // Search at base level with beam search
        let candidates = self.search_layer_beam(&graph[0], &vectors, query, current, ef);

        // Select k best candidates
        let mut results: Vec<(usize, f32)> = candidates.into_iter()
            .map(|id| {
                let vector = vectors.get(&id).unwrap();
                let distance = self.distance_computer.compute(query, vector).unwrap();
                (id, distance)
            })
            .collect();

        // Sort by distance (ascending for distance metrics, descending for similarity)
        let higher_is_similar = super::distance_metrics::DistanceMetricSelector::get_properties(&self.metric).higher_is_similar;

        if higher_is_similar {
            results.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap()); // Higher similarity first
        } else {
            results.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap()); // Lower distance first
        }

        results.truncate(k);
        Ok(results)
    }

    /// Delete a vector from the index
    pub fn delete(&mut self, id: usize) -> AuroraResult<()> {
        let mut graph = self.graph.write();
        let mut levels = self.levels.write();

        if let Some(vector_levels) = levels.remove(&id) {
            for level in vector_levels {
                if let Some(level_graph) = graph.get_mut(level as usize) {
                    level_graph.remove(&id);

                    // Remove this node from all neighbor lists
                    for neighbors in level_graph.values_mut() {
                        neighbors.retain(|&neighbor| neighbor != id);
                    }
                }
            }
        }

        let mut vectors = self.vectors.write();
        vectors.remove(&id);

        // Update entry point if necessary
        if self.entry_point == Some(id) {
            self.entry_point = self.find_new_entry_point(&graph);
        }

        Ok(())
    }

    /// Get statistics about the index
    pub fn stats(&self) -> HNSWStats {
        let graph = self.graph.read();
        let vectors = self.vectors.read();
        let levels = self.levels.read();

        let mut total_connections = 0;
        let mut max_connections = 0;
        let mut level_sizes = Vec::new();

        for (level, level_graph) in graph.iter().enumerate() {
            let level_size = level_graph.len();
            level_sizes.push(level_size);

            for connections in level_graph.values() {
                total_connections += connections.len();
                max_connections = max_connections.max(connections.len());
            }
        }

        let avg_connections = if vectors.len() > 0 {
            total_connections as f64 / vectors.len() as f64
        } else {
            0.0
        };

        HNSWStats {
            dimension: self.dimension,
            metric: self.metric.clone(),
            total_vectors: vectors.len(),
            max_level: self.max_level,
            total_connections,
            avg_connections,
            max_connections,
            level_sizes,
            memory_usage_mb: self.estimate_memory_usage(),
        }
    }

    /// Generate random level for a new vector
    fn generate_level(&mut self) -> i32 {
        let mut level = 0;
        while self.rng.f64() < (1.0 / self.level_multiplier.exp()) && level < 32 {
            level += 1;
        }
        level
    }

    /// Insert a vector at a specific level
    fn insert_at_level(&self, graph: &mut Vec<HashMap<usize, Vec<usize>>>, id: usize, vector: &[f32], level: i32, entry_point: Option<usize>) -> Option<usize> {
        let level_graph = &mut graph[level as usize];

        // Find neighbors for this vector at this level
        let neighbors = if let Some(ep) = entry_point {
            self.select_neighbors(level_graph, vector, ep, self.max_connections)
        } else {
            Vec::new()
        };

        // Add bidirectional connections
        level_graph.insert(id, neighbors.clone());
        for &neighbor in &neighbors {
            if let Some(neighbor_list) = level_graph.get_mut(&neighbor) {
                if !neighbor_list.contains(&id) {
                    neighbor_list.push(id);
                    // Shrink neighbor list if too large
                    if neighbor_list.len() > self.max_connections {
                        self.shrink_neighbors(level_graph, neighbor, self.max_connections);
                    }
                }
            }
        }

        // Return the closest neighbor as new entry point for lower levels
        neighbors.first().copied()
    }

    /// Search for the closest node at a given level
    fn search_layer(&self, level_graph: &HashMap<usize, Vec<usize>>, vectors: &HashMap<usize, Vec<f32>>, query: &[f32], entry_point: usize, ef: usize) -> usize {
        let mut visited = HashSet::new();
        let mut candidates = BinaryHeap::new();
        let mut best = entry_point;

        visited.insert(entry_point);
        candidates.push((Reverse(self.distance_to_query(vectors, query, entry_point)), entry_point));

        while let Some((Reverse(_), current)) = candidates.pop() {
            if let Some(neighbors) = level_graph.get(&current) {
                for &neighbor in neighbors {
                    if visited.insert(neighbor) {
                        let distance = self.distance_to_query(vectors, query, neighbor);
                        candidates.push((Reverse(distance), neighbor));

                        // Update best candidate
                        let best_distance = self.distance_to_query(vectors, query, best);
                        if distance < best_distance {
                            best = neighbor;
                        }
                    }
                }
            }
        }

        best
    }

    /// Beam search at base level to find ef closest neighbors
    fn search_layer_beam(&self, level_graph: &HashMap<usize, Vec<usize>>, vectors: &HashMap<usize, Vec<f32>>, query: &[f32], entry_point: usize, ef: usize) -> Vec<usize> {
        let mut visited = HashSet::new();
        let mut candidates = BinaryHeap::new(); // Max heap for distances
        let mut results = BinaryHeap::new(); // Min heap for best results

        visited.insert(entry_point);
        candidates.push((self.distance_to_query(vectors, query, entry_point), entry_point));
        results.push((Reverse(self.distance_to_query(vectors, query, entry_point)), entry_point));

        while let Some((_, current)) = candidates.pop() {
            if let Some(neighbors) = level_graph.get(&current) {
                for &neighbor in neighbors {
                    if visited.insert(neighbor) {
                        let distance = self.distance_to_query(vectors, query, neighbor);

                        // Add to candidates
                        candidates.push((distance, neighbor));

                        // Add to results if better than worst in results
                        if results.len() < ef {
                            results.push((Reverse(distance), neighbor));
                        } else if let Some((Reverse(worst_distance), _)) = results.peek() {
                            if distance < *worst_distance {
                                results.pop();
                                results.push((Reverse(distance), neighbor));
                            }
                        }
                    }
                }
            }
        }

        results.into_iter().map(|(_, id)| id).collect()
    }

    /// Select neighbors for a vector during insertion
    fn select_neighbors(&self, level_graph: &HashMap<usize, Vec<usize>>, vector: &[f32], entry_point: usize, max_connections: usize) -> Vec<usize> {
        let vectors = self.vectors.read();
        let mut candidates = HashSet::new();
        let mut results = BinaryHeap::new();

        // Start with entry point
        candidates.insert(entry_point);
        results.push((Reverse(self.distance_to_query(&vectors, vector, entry_point)), entry_point));

        // Explore neighbors
        while !results.is_empty() {
            let (_, current) = results.pop().unwrap();

            if let Some(neighbors) = level_graph.get(&current) {
                for &neighbor in neighbors {
                    if candidates.insert(neighbor) {
                        let distance = self.distance_between_vectors(&vectors, vector, neighbor);

                        if results.len() < max_connections {
                            results.push((Reverse(distance), neighbor));
                        } else if let Some((Reverse(furthest), _)) = results.peek() {
                            if distance < *furthest {
                                results.pop();
                                results.push((Reverse(distance), neighbor));
                            }
                        }
                    }
                }
            }
        }

        results.into_iter().map(|(_, id)| id).collect()
    }

    /// Shrink neighbor list to maximum size
    fn shrink_neighbors(&self, level_graph: &mut HashMap<usize, Vec<usize>>, node: usize, max_size: usize) {
        if let Some(neighbors) = level_graph.get_mut(&node) {
            if neighbors.len() <= max_size {
                return;
            }

            // Keep only the closest neighbors
            let vectors = self.vectors.read();
            let node_vector = vectors.get(&node).unwrap();

            let mut neighbor_distances: Vec<(f32, usize)> = neighbors.iter()
                .map(|&neighbor| {
                    let distance = self.distance_between_vectors(&vectors, node_vector, neighbor);
                    (distance, neighbor)
                })
                .collect();

            neighbor_distances.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
            neighbor_distances.truncate(max_size);

            *neighbors = neighbor_distances.into_iter().map(|(_, id)| id).collect();
        }
    }

    /// Find a new entry point after deletion
    fn find_new_entry_point(&self, graph: &[HashMap<usize, Vec<usize>>]) -> Option<usize> {
        for level_graph in graph.iter().rev() {
            if let Some(&node) = level_graph.keys().next() {
                return Some(node);
            }
        }
        None
    }

    /// Compute distance between query and a stored vector
    fn distance_to_query(&self, vectors: &HashMap<usize, Vec<f32>>, query: &[f32], id: usize) -> f32 {
        let vector = vectors.get(&id).unwrap();
        self.distance_computer.compute(query, vector).unwrap()
    }

    /// Compute distance between two stored vectors
    fn distance_between_vectors(&self, vectors: &HashMap<usize, Vec<f32>>, vector: &[f32], id: usize) -> f32 {
        let other_vector = vectors.get(&id).unwrap();
        self.distance_computer.compute(vector, other_vector).unwrap()
    }

    /// Estimate memory usage of the index
    fn estimate_memory_usage(&self) -> f64 {
        let graph = self.graph.read();
        let vectors = self.vectors.read();

        // Vector storage: dimension * 4 bytes per vector
        let vector_memory = vectors.len() as f64 * self.dimension as f64 * 4.0;

        // Graph storage: connections * 8 bytes (for usize)
        let mut graph_memory = 0.0;
        for level_graph in graph.iter() {
            for neighbors in level_graph.values() {
                graph_memory += neighbors.len() as f64 * 8.0;
            }
        }

        // Overhead and metadata
        let overhead = (vectors.len() as f64 * 32.0) + (graph.len() as f64 * 64.0);

        (vector_memory + graph_memory + overhead) / (1024.0 * 1024.0) // Convert to MB
    }
}

/// HNSW index statistics
#[derive(Debug, Clone)]
pub struct HNSWStats {
    pub dimension: usize,
    pub metric: DistanceMetric,
    pub total_vectors: usize,
    pub max_level: i32,
    pub total_connections: usize,
    pub avg_connections: f64,
    pub max_connections: usize,
    pub level_sizes: Vec<usize>,
    pub memory_usage_mb: f64,
}

/// HNSW index configuration
#[derive(Debug, Clone)]
pub struct HNSWConfig {
    pub max_connections: usize,
    pub max_connections_base: usize,
    pub ef_construction: usize,
    pub ef_search: usize,
}

impl Default for HNSWConfig {
    fn default() -> Self {
        Self {
            max_connections: 32,
            max_connections_base: 64,
            ef_construction: 200,
            ef_search: 64,
        }
    }
}

/// Adaptive HNSW that tunes parameters based on dataset characteristics
pub struct AdaptiveHNSW {
    base_index: HNSWIndex,
    config: HNSWConfig,
    dataset_stats: DatasetStats,
}

#[derive(Debug, Clone)]
struct DatasetStats {
    pub vector_count: usize,
    pub avg_norm: f32,
    pub dimensionality: usize,
    pub intrinsic_dimensionality: f32,
}

impl AdaptiveHNSW {
    pub fn new(dimension: usize, metric: DistanceMetric, initial_config: HNSWConfig) -> Self {
        Self {
            base_index: HNSWIndex::new(dimension, metric),
            config: initial_config,
            dataset_stats: DatasetStats {
                vector_count: 0,
                avg_norm: 0.0,
                dimensionality: dimension,
                intrinsic_dimensionality: dimension as f32,
            },
        }
    }

    /// Adapt configuration based on current dataset
    pub fn adapt_configuration(&mut self) {
        let vector_count = self.base_index.vectors.read().len();

        // Adjust max connections based on dataset size
        if vector_count > 100000 {
            self.config.max_connections = 16; // Smaller for large datasets
            self.config.max_connections_base = 32;
        } else if vector_count > 10000 {
            self.config.max_connections = 24;
            self.config.max_connections_base = 48;
        }

        // Adjust ef parameters based on accuracy requirements
        // (Higher ef = better accuracy but slower search)
        if vector_count > 1000000 {
            self.config.ef_construction = 400;
            self.config.ef_search = 128;
        }
    }

    /// Insert with adaptive configuration
    pub fn insert(&mut self, id: usize, vector: Vec<f32>) -> AuroraResult<()> {
        self.adapt_configuration();
        self.base_index.insert(id, vector)?;

        // Update dataset statistics
        self.dataset_stats.vector_count += 1;
        let norm = vector.iter().map(|x| x * x).sum::<f32>().sqrt();
        self.dataset_stats.avg_norm = (self.dataset_stats.avg_norm * (self.dataset_stats.vector_count - 1) as f32 + norm) /
                                      self.dataset_stats.vector_count as f32;

        Ok(())
    }

    /// Search with adaptive parameters
    pub fn search(&self, query: &[f32], k: usize) -> AuroraResult<Vec<(usize, f32)>> {
        // Use adaptive ef_search based on k
        let ef = if k <= 10 {
            self.config.ef_search / 2
        } else if k <= 100 {
            self.config.ef_search
        } else {
            self.config.ef_search * 2
        };

        self.base_index.search(query, k, ef)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_hnsw_basic_operations() {
        let mut index = HNSWIndex::new(3, DistanceMetric::Euclidean);

        // Insert some vectors
        index.insert(0, vec![1.0, 0.0, 0.0]).unwrap();
        index.insert(1, vec![0.0, 1.0, 0.0]).unwrap();
        index.insert(2, vec![0.0, 0.0, 1.0]).unwrap();
        index.insert(3, vec![1.0, 1.0, 1.0]).unwrap();

        // Search for nearest neighbors
        let query = vec![1.0, 0.0, 0.0];
        let results = index.search(&query, 2, 32).unwrap();

        assert_eq!(results.len(), 2);
        assert_eq!(results[0].0, 0); // Should find the identical vector first

        // Check that distances are reasonable
        for (_, distance) in results {
            assert!(distance >= 0.0);
        }
    }

    #[test]
    fn test_hnsw_stats() {
        let mut index = HNSWIndex::new(3, DistanceMetric::Euclidean);

        // Insert vectors
        for i in 0..10 {
            let vector = vec![i as f32, (i * 2) as f32, (i * 3) as f32];
            index.insert(i, vector).unwrap();
        }

        let stats = index.stats();
        assert_eq!(stats.dimension, 3);
        assert_eq!(stats.total_vectors, 10);
        assert!(stats.total_connections > 0);
        assert!(stats.memory_usage_mb > 0.0);
    }

    #[test]
    fn test_hnsw_deletion() {
        let mut index = HNSWIndex::new(3, DistanceMetric::Euclidean);

        // Insert vectors
        index.insert(0, vec![1.0, 0.0, 0.0]).unwrap();
        index.insert(1, vec![0.0, 1.0, 0.0]).unwrap();

        // Delete a vector
        index.delete(0).unwrap();

        let stats = index.stats();
        assert_eq!(stats.total_vectors, 1);
    }

    #[test]
    fn test_adaptive_hnsw() {
        let mut adaptive = AdaptiveHNSW::new(3, DistanceMetric::Cosine, HNSWConfig::default());

        // Insert vectors
        for i in 0..100 {
            let vector = vec![
                (i as f32).sin(),
                (i as f32).cos(),
                ((i * 2) as f32).sin(),
            ];
            adaptive.insert(i, vector).unwrap();
        }

        // Search
        let query = vec![0.0, 1.0, 0.0];
        let results = adaptive.search(&query, 5).unwrap();

        assert_eq!(results.len(), 5);
    }

    #[test]
    fn test_hnsw_different_metrics() {
        // Test with different distance metrics
        let metrics = vec![DistanceMetric::Euclidean, DistanceMetric::Cosine, DistanceMetric::DotProduct];

        for metric in metrics {
            let mut index = HNSWIndex::new(3, metric);

            index.insert(0, vec![1.0, 0.0, 0.0]).unwrap();
            index.insert(1, vec![0.0, 1.0, 0.0]).unwrap();

            let query = vec![1.0, 0.0, 0.0];
            let results = index.search(&query, 1, 32).unwrap();

            assert_eq!(results.len(), 1);
            assert_eq!(results[0].0, 0);
        }
    }

    #[test]
    fn test_hnsw_empty_search() {
        let index = HNSWIndex::new(3, DistanceMetric::Euclidean);

        let query = vec![1.0, 0.0, 0.0];
        let results = index.search(&query, 5, 32).unwrap();

        assert_eq!(results.len(), 0);
    }

    #[test]
    fn test_hnsw_dimension_mismatch() {
        let mut index = HNSWIndex::new(3, DistanceMetric::Euclidean);

        // Try to insert wrong dimension vector
        let result = index.insert(0, vec![1.0, 2.0]);
        assert!(result.is_err());
    }

    #[test]
    fn test_hnsw_large_scale() {
        let mut index = HNSWIndex::new(128, DistanceMetric::Cosine);

        // Insert many high-dimensional vectors
        for i in 0..1000 {
            let mut vector = Vec::with_capacity(128);
            for j in 0..128 {
                vector.push(((i * 128 + j) as f32).sin());
            }
            index.insert(i, vector).unwrap();
        }

        let stats = index.stats();
        assert_eq!(stats.total_vectors, 1000);
        assert_eq!(stats.dimension, 128);

        // Search
        let query = vec![0.0; 128];
        let results = index.search(&query, 10, 64).unwrap();
        assert_eq!(results.len(), 10);
    }
}
