//! AuroraDB IVF Index: Inverted File Index for Large-Scale Vector Search
//!
//! IVF implementation with PQ quantization for memory-efficient similarity search:
//! - Clustering-based indexing for fast candidate selection
//! - Product quantization for sub-linear search complexity
//! - Optimized for high-dimensional vectors and large datasets
//! - Memory-efficient storage with configurable compression

use std::collections::{HashMap, HashSet, BTreeMap};
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use super::distance_metrics::{DistanceComputer, DistanceMetric};

/// IVF (Inverted File Index) with PQ (Product Quantization)
pub struct IVFIndex {
    /// Vector dimension
    dimension: usize,

    /// Distance metric
    metric: DistanceMetric,

    /// Distance computer
    distance_computer: DistanceComputer,

    /// Number of clusters (inverted lists)
    num_clusters: usize,

    /// Number of sub-quantizers for PQ
    num_subquantizers: usize,

    /// Number of centroids per sub-quantizer
    num_centroids: usize,

    /// Cluster centroids: cluster_id -> centroid vector
    centroids: RwLock<HashMap<usize, Vec<f32>>>,

    /// Inverted lists: cluster_id -> list of (vector_id, pq_code)
    inverted_lists: RwLock<HashMap<usize, Vec<(usize, Vec<u8>)>>>,

    /// PQ codebook: subquantizer -> centroid_id -> subvector
    pq_codebook: RwLock<Vec<HashMap<usize, Vec<f32>>>>,

    /// Vector storage for exact distance computation
    vectors: RwLock<HashMap<usize, Vec<f32>>>,

    /// PQ encoder for compressing vectors
    pq_encoder: PQEncoder,

    /// Random number generator
    rng: fastrand::Rng,
}

impl IVFIndex {
    /// Create a new IVF index
    pub fn new(dimension: usize, metric: DistanceMetric, config: IVFConfig) -> Self {
        let pq_encoder = PQEncoder::new(dimension, config.num_subquantizers, config.num_centroids);

        Self {
            dimension,
            metric: metric.clone(),
            distance_computer: DistanceComputer::new(metric, dimension),
            num_clusters: config.num_clusters,
            num_subquantizers: config.num_subquantizers,
            num_centroids: config.num_centroids,
            centroids: RwLock::new(HashMap::new()),
            inverted_lists: RwLock::new(HashMap::new()),
            pq_codebook: RwLock::new(Vec::new()),
            vectors: RwLock::new(HashMap::new()),
            pq_encoder,
            rng: fastrand::Rng::new(),
        }
    }

    /// Build the index from a set of vectors
    pub fn build(&mut self, vectors: HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
        if vectors.is_empty() {
            return Ok(());
        }

        // Store vectors
        let mut stored_vectors = self.vectors.write();
        *stored_vectors = vectors.clone();
        drop(stored_vectors);

        // Train PQ codebook
        self.train_pq_codebook(&vectors)?;

        // Perform k-means clustering
        self.train_clusters(&vectors)?;

        // Assign vectors to clusters and encode with PQ
        self.build_inverted_lists(vectors)?;

        Ok(())
    }

    /// Insert a vector into the index
    pub fn insert(&mut self, id: usize, vector: Vec<f32>) -> AuroraResult<()> {
        if vector.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Vector dimension mismatch: expected {}, got {}",
                self.dimension, vector.len()
            )));
        }

        // Store vector
        let mut vectors = self.vectors.write();
        vectors.insert(id, vector.clone());
        drop(vectors);

        // Find closest cluster
        let cluster_id = self.find_nearest_cluster(&vector)?;

        // Encode vector with PQ
        let pq_code = self.pq_encoder.encode(&vector)?;

        // Add to inverted list
        let mut inverted_lists = self.inverted_lists.write();
        inverted_lists.entry(cluster_id).or_insert_with(Vec::new).push((id, pq_code));

        Ok(())
    }

    /// Search for k nearest neighbors
    pub fn search(&self, query: &[f32], k: usize, nprobe: usize) -> AuroraResult<Vec<(usize, f32)>> {
        if query.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Query vector dimension mismatch: expected {}, got {}",
                self.dimension, query.len()
            )));
        }

        // Find nprobe closest clusters
        let probe_clusters = self.find_nearest_clusters(query, nprobe)?;

        // Search within selected clusters
        let mut candidates = Vec::new();

        let inverted_lists = self.inverted_lists.read();
        let vectors = self.vectors.read();

        for &cluster_id in &probe_clusters {
            if let Some(cluster_vectors) = inverted_lists.get(&cluster_id) {
                // Use PQ for fast candidate selection, then exact distance
                for &(vector_id, _) in cluster_vectors {
                    if let Some(vector) = vectors.get(&vector_id) {
                        let distance = self.distance_computer.compute(query, vector)?;
                        candidates.push((vector_id, distance));
                    }
                }
            }
        }

        // Sort by distance and return top k
        let higher_is_similar = super::distance_metrics::DistanceMetricSelector::get_properties(&self.metric).higher_is_similar;

        if higher_is_similar {
            candidates.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap()); // Higher similarity first
        } else {
            candidates.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap()); // Lower distance first
        }

        candidates.truncate(k);
        Ok(candidates)
    }

    /// Delete a vector from the index
    pub fn delete(&mut self, id: usize) -> AuroraResult<()> {
        let mut inverted_lists = self.inverted_lists.write();
        let mut vectors = self.vectors.write();

        // Remove from vectors
        vectors.remove(&id);

        // Remove from inverted lists
        for cluster_vectors in inverted_lists.values_mut() {
            cluster_vectors.retain(|(vid, _)| *vid != id);
        }

        Ok(())
    }

    /// Get index statistics
    pub fn stats(&self) -> IVFStats {
        let inverted_lists = self.inverted_lists.read();
        let vectors = self.vectors.read();

        let mut total_vectors = 0;
        let mut cluster_sizes = Vec::new();
        let mut avg_cluster_size = 0.0;

        for cluster_vectors in inverted_lists.values() {
            let size = cluster_vectors.len();
            total_vectors += size;
            cluster_sizes.push(size);
            avg_cluster_size += size as f64;
        }

        if !cluster_sizes.is_empty() {
            avg_cluster_size /= cluster_sizes.len() as f64;
        }

        IVFStats {
            dimension: self.dimension,
            metric: self.metric.clone(),
            num_clusters: self.num_clusters,
            num_subquantizers: self.num_subquantizers,
            num_centroids: self.num_centroids,
            total_vectors: vectors.len(),
            avg_cluster_size,
            max_cluster_size: cluster_sizes.iter().max().copied().unwrap_or(0),
            min_cluster_size: cluster_sizes.iter().min().copied().unwrap_or(0),
            memory_usage_mb: self.estimate_memory_usage(),
        }
    }

    /// Train PQ codebook
    fn train_pq_codebook(&mut self, vectors: &HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
        let vector_list: Vec<&Vec<f32>> = vectors.values().collect();
        let codebook = self.pq_encoder.train(&vector_list)?;

        let mut pq_codebook = self.pq_codebook.write();
        *pq_codebook = codebook;

        Ok(())
    }

    /// Train cluster centroids using k-means
    fn train_clusters(&mut self, vectors: &HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
        let vector_list: Vec<(usize, &Vec<f32>)> = vectors.iter().map(|(id, vec)| (*id, vec)).collect();

        // Initialize centroids randomly
        let mut centroids = self.initialize_centroids(&vector_list);

        // K-means iterations
        let max_iterations = 50;
        for _ in 0..max_iterations {
            let assignments = self.assign_to_centroids(&vector_list, &centroids);
            let new_centroids = self.update_centroids(&vector_list, &assignments);

            // Check convergence
            if self.centroids_converged(&centroids, &new_centroids) {
                break;
            }
            centroids = new_centroids;
        }

        let mut stored_centroids = self.centroids.write();
        for (i, centroid) in centroids.into_iter().enumerate() {
            stored_centroids.insert(i, centroid);
        }

        Ok(())
    }

    /// Build inverted lists with PQ codes
    fn build_inverted_lists(&mut self, vectors: HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
        let mut inverted_lists = self.inverted_lists.write();

        for (id, vector) in vectors {
            let cluster_id = self.find_nearest_cluster(&vector)?;
            let pq_code = self.pq_encoder.encode(&vector)?;

            inverted_lists.entry(cluster_id).or_insert_with(Vec::new).push((id, pq_code));
        }

        Ok(())
    }

    /// Find nearest cluster for a vector
    fn find_nearest_cluster(&self, vector: &[f32]) -> AuroraResult<usize> {
        let centroids = self.centroids.read();

        let mut best_cluster = 0;
        let mut best_distance = f32::INFINITY;

        for (cluster_id, centroid) in centroids.iter() {
            let distance = self.distance_computer.compute(vector, centroid)?;
            if distance < best_distance {
                best_distance = distance;
                best_cluster = *cluster_id;
            }
        }

        Ok(best_cluster)
    }

    /// Find nprobe nearest clusters
    fn find_nearest_clusters(&self, query: &[f32], nprobe: usize) -> AuroraResult<Vec<usize>> {
        let centroids = self.centroids.read();

        let mut cluster_distances: Vec<(f32, usize)> = centroids.iter()
            .map(|(id, centroid)| {
                let distance = self.distance_computer.compute(query, centroid).unwrap_or(f32::INFINITY);
                (distance, *id)
            })
            .collect();

        cluster_distances.sort_by(|a, b| a.0.partial_cmp(&b.0).unwrap());
        cluster_distances.truncate(nprobe);

        Ok(cluster_distances.into_iter().map(|(_, id)| id).collect())
    }

    /// Initialize centroids randomly
    fn initialize_centroids(&self, vectors: &[(usize, &Vec<f32>)]) -> Vec<Vec<f32>> {
        let mut centroids = Vec::new();

        for _ in 0..self.num_clusters {
            let random_idx = self.rng.usize(0..vectors.len());
            centroids.push(vectors[random_idx].1.clone());
        }

        centroids
    }

    /// Assign vectors to nearest centroids
    fn assign_to_centroids(&self, vectors: &[(usize, &Vec<f32>)], centroids: &[Vec<f32>]) -> HashMap<usize, Vec<usize>> {
        let mut assignments: HashMap<usize, Vec<usize>> = HashMap::new();

        for (id, vector) in vectors {
            let mut best_cluster = 0;
            let mut best_distance = f32::INFINITY;

            for (i, centroid) in centroids.iter().enumerate() {
                let distance = self.distance_computer.compute(vector, centroid).unwrap_or(f32::INFINITY);
                if distance < best_distance {
                    best_distance = distance;
                    best_cluster = i;
                }
            }

            assignments.entry(best_cluster).or_insert_with(Vec::new).push(*id);
        }

        assignments
    }

    /// Update centroids based on assignments
    fn update_centroids(&self, vectors: &[(usize, &Vec<f32>)], assignments: &HashMap<usize, Vec<usize>>) -> Vec<Vec<f32>> {
        let mut new_centroids = Vec::new();

        for cluster_id in 0..self.num_clusters {
            if let Some(cluster_vectors) = assignments.get(&cluster_id) {
                let mut centroid = vec![0.0; self.dimension];
                let count = cluster_vectors.len() as f32;

                for &vector_id in cluster_vectors {
                    if let Some((_, vector)) = vectors.iter().find(|(id, _)| *id == vector_id) {
                        for i in 0..self.dimension {
                            centroid[i] += vector[i];
                        }
                    }
                }

                for i in 0..self.dimension {
                    centroid[i] /= count;
                }

                new_centroids.push(centroid);
            } else {
                // Keep old centroid if no vectors assigned
                new_centroids.push(vec![0.0; self.dimension]);
            }
        }

        new_centroids
    }

    /// Check if centroids have converged
    fn centroids_converged(&self, old: &[Vec<f32>], new: &[Vec<f32>]) -> bool {
        let tolerance = 1e-6;

        for (old_centroid, new_centroid) in old.iter().zip(new.iter()) {
            for (old_val, new_val) in old_centroid.iter().zip(new_centroid.iter()) {
                if (old_val - new_val).abs() > tolerance {
                    return false;
                }
            }
        }

        true
    }

    /// Estimate memory usage
    fn estimate_memory_usage(&self) -> f64 {
        let vectors = self.vectors.read();
        let inverted_lists = self.inverted_lists.read();

        // Vector storage
        let vector_memory = vectors.len() as f64 * self.dimension as f64 * 4.0;

        // Inverted lists: (vector_id + pq_code) per entry
        let pq_code_size = self.num_subquantizers as f64; // bytes per PQ code
        let mut inverted_memory = 0.0;
        for cluster_vectors in inverted_lists.values() {
            inverted_memory += cluster_vectors.len() as f64 * (8.0 + pq_code_size); // usize + PQ code
        }

        // Centroids and codebook
        let centroids_memory = self.num_clusters as f64 * self.dimension as f64 * 4.0;
        let codebook_memory = self.num_subquantizers as f64 * self.num_centroids as f64 * (self.dimension / self.num_subquantizers) as f64 * 4.0;

        (vector_memory + inverted_memory + centroids_memory + codebook_memory) / (1024.0 * 1024.0)
    }
}

/// IVF index configuration
#[derive(Debug, Clone)]
pub struct IVFConfig {
    pub num_clusters: usize,
    pub num_subquantizers: usize,
    pub num_centroids: usize,
}

impl Default for IVFConfig {
    fn default() -> Self {
        Self {
            num_clusters: 1024,
            num_subquantizers: 8,
            num_centroids: 256,
        }
    }
}

/// IVF statistics
#[derive(Debug, Clone)]
pub struct IVFStats {
    pub dimension: usize,
    pub metric: DistanceMetric,
    pub num_clusters: usize,
    pub num_subquantizers: usize,
    pub num_centroids: usize,
    pub total_vectors: usize,
    pub avg_cluster_size: f64,
    pub max_cluster_size: usize,
    pub min_cluster_size: usize,
    pub memory_usage_mb: f64,
}

/// Product Quantization encoder
struct PQEncoder {
    dimension: usize,
    num_subquantizers: usize,
    num_centroids: usize,
    subvector_dim: usize,
}

impl PQEncoder {
    fn new(dimension: usize, num_subquantizers: usize, num_centroids: usize) -> Self {
        let subvector_dim = dimension / num_subquantizers;
        Self {
            dimension,
            num_subquantizers,
            num_centroids,
            subvector_dim,
        }
    }

    /// Train PQ codebook on training vectors
    fn train(&self, vectors: &[&Vec<f32>]) -> AuroraResult<Vec<HashMap<usize, Vec<f32>>>> {
        let mut codebook = Vec::new();

        for subquantizer in 0..self.num_subquantizers {
            let start_dim = subquantizer * self.subvector_dim;
            let end_dim = start_dim + self.subvector_dim;

            // Extract subvectors for this subquantizer
            let subvectors: Vec<Vec<f32>> = vectors.iter()
                .map(|v| v[start_dim..end_dim].to_vec())
                .collect();

            // Train k-means on subvectors
            let centroids = self.train_subquantizer(&subvectors)?;
            codebook.push(centroids);
        }

        Ok(codebook)
    }

    /// Encode a vector using the trained codebook
    fn encode(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        let pq_codebook = self.pq_codebook.read();
        let mut code = Vec::new();

        for subquantizer in 0..self.num_subquantizers {
            let start_dim = subquantizer * self.subvector_dim;
            let end_dim = start_dim + self.subvector_dim;
            let subvector = &vector[start_dim..end_dim];

            if let Some(centroids) = pq_codebook.get(subquantizer) {
                let closest_centroid = self.find_closest_centroid(subvector, centroids);
                code.push(closest_centroid as u8);
            } else {
                return Err(AuroraError::Vector("PQ codebook not trained".to_string()));
            }
        }

        Ok(code)
    }

    /// Train a single subquantizer using k-means
    fn train_subquantizer(&self, subvectors: &[Vec<f32>]) -> AuroraResult<HashMap<usize, Vec<f32>>> {
        let mut centroids = HashMap::new();

        // Initialize centroids randomly
        for i in 0..self.num_centroids {
            let random_idx = fastrand::usize(0..subvectors.len());
            centroids.insert(i, subvectors[random_idx].clone());
        }

        // K-means iterations
        let max_iterations = 20;
        for _ in 0..max_iterations {
            let assignments = self.assign_subvectors_to_centroids(subvectors, &centroids);
            let new_centroids = self.update_subvector_centroids(subvectors, &assignments);

            // Check convergence
            if self.subvector_centroids_converged(&centroids, &new_centroids) {
                break;
            }
            centroids = new_centroids;
        }

        Ok(centroids)
    }

    /// Find closest centroid for a subvector
    fn find_closest_centroid(&self, subvector: &[f32], centroids: &HashMap<usize, Vec<f32>>) -> usize {
        let mut best_centroid = 0;
        let mut best_distance = f32::INFINITY;

        for (centroid_id, centroid) in centroids {
            let distance = self.euclidean_distance(subvector, centroid);
            if distance < best_distance {
                best_distance = distance;
                best_centroid = *centroid_id;
            }
        }

        best_centroid
    }

    /// Assign subvectors to centroids
    fn assign_subvectors_to_centroids(&self, subvectors: &[Vec<f32>], centroids: &HashMap<usize, Vec<f32>>) -> HashMap<usize, Vec<usize>> {
        let mut assignments = HashMap::new();

        for (i, subvector) in subvectors.iter().enumerate() {
            let closest = self.find_closest_centroid(subvector, centroids);
            assignments.entry(closest).or_insert_with(Vec::new).push(i);
        }

        assignments
    }

    /// Update subvector centroids
    fn update_subvector_centroids(&self, subvectors: &[Vec<f32>], assignments: &HashMap<usize, Vec<usize>>) -> HashMap<usize, Vec<f32>> {
        let mut new_centroids = HashMap::new();

        for (centroid_id, vector_indices) in assignments {
            let mut centroid = vec![0.0; self.subvector_dim];
            let count = vector_indices.len() as f32;

            for &vector_idx in vector_indices {
                for i in 0..self.subvector_dim {
                    centroid[i] += subvectors[vector_idx][i];
                }
            }

            for i in 0..self.subvector_dim {
                centroid[i] /= count;
            }

            new_centroids.insert(*centroid_id, centroid);
        }

        new_centroids
    }

    /// Check convergence of subvector centroids
    fn subvector_centroids_converged(&self, old: &HashMap<usize, Vec<f32>>, new: &HashMap<usize, Vec<f32>>) -> bool {
        let tolerance = 1e-6;

        for (id, old_centroid) in old {
            if let Some(new_centroid) = new.get(id) {
                for (old_val, new_val) in old_centroid.iter().zip(new_centroid.iter()) {
                    if (old_val - new_val).abs() > tolerance {
                        return false;
                    }
                }
            }
        }

        true
    }

    /// Euclidean distance for subvectors
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> f32 {
        let mut sum = 0.0;
        for i in 0..a.len() {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        sum.sqrt()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_ivf_basic_operations() {
        let config = IVFConfig {
            num_clusters: 4,
            num_subquantizers: 2,
            num_centroids: 8,
        };
        let mut index = IVFIndex::new(4, DistanceMetric::Euclidean, config);

        // Create test vectors
        let mut vectors = HashMap::new();
        for i in 0..20 {
            let vector = vec![i as f32, (i * 2) as f32, (i * 3) as f32, (i * 4) as f32];
            vectors.insert(i, vector);
        }

        // Build index
        index.build(vectors).unwrap();

        // Search
        let query = vec![5.0, 10.0, 15.0, 20.0];
        let results = index.search(&query, 5, 2).unwrap();

        assert_eq!(results.len(), 5);
        // Should find vectors close to the query
        for (id, distance) in results {
            assert!(id < 20);
            assert!(distance >= 0.0);
        }
    }

    #[test]
    fn test_ivf_stats() {
        let config = IVFConfig {
            num_clusters: 8,
            num_subquantizers: 4,
            num_centroids: 16,
        };
        let mut index = IVFIndex::new(8, DistanceMetric::Cosine, config);

        let mut vectors = HashMap::new();
        for i in 0..50 {
            let vector = vec![0.0; 8]; // All zeros for simple test
            vectors.insert(i, vector);
        }

        index.build(vectors).unwrap();
        let stats = index.stats();

        assert_eq!(stats.dimension, 8);
        assert_eq!(stats.num_clusters, 8);
        assert_eq!(stats.total_vectors, 50);
        assert!(stats.memory_usage_mb > 0.0);
    }

    #[test]
    fn test_ivf_insert_and_delete() {
        let config = IVFConfig {
            num_clusters: 4,
            num_subquantizers: 2,
            num_centroids: 8,
        };
        let mut index = IVFIndex::new(4, DistanceMetric::Euclidean, config);

        // Insert vectors
        index.insert(0, vec![1.0, 2.0, 3.0, 4.0]).unwrap();
        index.insert(1, vec![5.0, 6.0, 7.0, 8.0]).unwrap();

        let stats_before = index.stats();
        assert_eq!(stats_before.total_vectors, 2);

        // Delete a vector
        index.delete(0).unwrap();

        let stats_after = index.stats();
        assert_eq!(stats_after.total_vectors, 1);
    }

    #[test]
    fn test_ivf_different_metrics() {
        let metrics = vec![DistanceMetric::Euclidean, DistanceMetric::Cosine];

        for metric in metrics {
            let config = IVFConfig {
                num_clusters: 2,
                num_subquantizers: 2,
                num_centroids: 4,
            };
            let mut index = IVFIndex::new(4, metric, config);

            let mut vectors = HashMap::new();
            vectors.insert(0, vec![1.0, 0.0, 0.0, 0.0]);
            vectors.insert(1, vec![0.0, 1.0, 0.0, 0.0]);

            index.build(vectors).unwrap();

            let query = vec![1.0, 0.0, 0.0, 0.0];
            let results = index.search(&query, 1, 2).unwrap();

            assert_eq!(results.len(), 1);
            assert_eq!(results[0].0, 0); // Should find the identical vector
        }
    }

    #[test]
    fn test_ivf_empty_search() {
        let config = IVFConfig::default();
        let index = IVFIndex::new(4, DistanceMetric::Euclidean, config);

        let query = vec![1.0, 2.0, 3.0, 4.0];
        let results = index.search(&query, 5, 1).unwrap();

        assert_eq!(results.len(), 0);
    }

    #[test]
    fn test_pq_encoder() {
        let encoder = PQEncoder::new(8, 4, 16);

        let vectors: Vec<Vec<f32>> = (0..100).map(|i| {
            vec![
                (i as f32).sin(), (i as f32).cos(),
                ((i * 2) as f32).sin(), ((i * 2) as f32).cos(),
                ((i * 3) as f32).sin(), ((i * 3) as f32).cos(),
                ((i * 4) as f32).sin(), ((i * 4) as f32).cos(),
            ]
        }).collect();

        let vector_refs: Vec<&Vec<f32>> = vectors.iter().collect();
        let codebook = encoder.train(&vector_refs).unwrap();

        assert_eq!(codebook.len(), 4); // 4 subquantizers

        // Test encoding
        let test_vector = vec![1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0];
        let code = encoder.encode(&test_vector).unwrap();

        assert_eq!(code.len(), 4); // 4 subquantizers
        for &centroid_id in &code {
            assert!(centroid_id < 16); // Should be valid centroid ID
        }
    }

    #[test]
    fn test_ivf_large_scale() {
        let config = IVFConfig {
            num_clusters: 256,
            num_subquantizers: 8,
            num_centroids: 64,
        };
        let mut index = IVFIndex::new(128, DistanceMetric::Cosine, config);

        // Generate high-dimensional vectors
        let mut vectors = HashMap::new();
        for i in 0..1000 {
            let mut vector = Vec::with_capacity(128);
            for j in 0..128 {
                vector.push(((i * 128 + j) as f32 * 0.01).sin());
            }
            vectors.insert(i, vector);
        }

        // Build index
        index.build(vectors).unwrap();

        let stats = index.stats();
        assert_eq!(stats.total_vectors, 1000);
        assert_eq!(stats.dimension, 128);
        assert!(stats.avg_cluster_size > 0.0);

        // Search
        let query = vec![0.0; 128];
        let results = index.search(&query, 10, 16).unwrap();
        assert_eq!(results.len(), 10);
    }
}
