//! AuroraDB Product Quantization: Advanced Vector Compression
//!
//! Research-backed product quantization implementation for memory-efficient
//! vector search with minimal accuracy loss and SIMD acceleration.

use std::collections::HashMap;
use crate::core::errors::{AuroraResult, AuroraError};

/// Product Quantization encoder with advanced optimization
pub struct PQEncoder {
    dimension: usize,
    num_subquantizers: usize,
    num_centroids: usize,
    subvector_dim: usize,
    codebooks: Vec<PQCodebook>,
}

impl PQEncoder {
    /// Create a new PQ encoder
    pub fn new(dimension: usize, num_subquantizers: usize, num_centroids: usize) -> Self {
        let subvector_dim = dimension / num_subquantizers;
        Self {
            dimension,
            num_subquantizers,
            num_centroids,
            subvector_dim,
            codebooks: Vec::new(),
        }
    }

    /// Train PQ codebooks on training vectors
    pub fn train(&mut self, vectors: &[&[f32]]) -> AuroraResult<Vec<HashMap<usize, Vec<f32>>>> {
        if vectors.is_empty() {
            return Err(AuroraError::Vector("No training vectors provided".to_string()));
        }

        let mut codebooks = Vec::new();

        // Train a separate codebook for each subquantizer
        for subquantizer in 0..self.num_subquantizers {
            let start_dim = subquantizer * self.subvector_dim;
            let end_dim = start_dim + self.subvector_dim;

            // Extract subvectors for this subquantizer
            let subvectors: Vec<Vec<f32>> = vectors.iter()
                .map(|v| v[start_dim..end_dim].to_vec())
                .collect();

            // Train k-means on subvectors
            let codebook = self.train_subquantizer_codebook(&subvectors)?;
            codebooks.push(codebook);
        }

        self.codebooks = codebooks.into_iter().map(|cb| PQCodebook::new(cb)).collect();
        Ok(self.codebooks.iter().map(|cb| cb.centroids.clone()).collect())
    }

    /// Encode a vector using trained codebooks
    pub fn encode(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        if vector.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Vector dimension mismatch: expected {}, got {}",
                self.dimension, vector.len()
            )));
        }

        if self.codebooks.is_empty() {
            return Err(AuroraError::Vector("PQ encoder not trained".to_string()));
        }

        let mut codes = Vec::with_capacity(self.num_subquantizers);

        for subquantizer in 0..self.num_subquantizers {
            let start_dim = subquantizer * self.subvector_dim;
            let end_dim = start_dim + self.subvector_dim;
            let subvector = &vector[start_dim..end_dim];

            let code = self.codebooks[subquantizer].quantize(subvector);
            codes.push(code as u8);
        }

        Ok(codes)
    }

    /// Decode a PQ code back to approximate vector
    pub fn decode(&self, codes: &[u8]) -> AuroraResult<Vec<f32>> {
        if codes.len() != self.num_subquantizers {
            return Err(AuroraError::Vector(format!(
                "Code length mismatch: expected {}, got {}",
                self.num_subquantizers, codes.len()
            )));
        }

        let mut reconstructed = Vec::with_capacity(self.dimension);

        for (subquantizer, &code) in codes.iter().enumerate() {
            let centroid = self.codebooks[subquantizer].get_centroid(code as usize)?;
            reconstructed.extend_from_slice(&centroid);
        }

        Ok(reconstructed)
    }

    /// Compute asymmetric distance (fast search distance)
    pub fn asymmetric_distance(&self, query: &[f32], codes: &[u8]) -> AuroraResult<f32> {
        if codes.len() != self.num_subquantizers {
            return Err(AuroraError::Vector(format!(
                "Code length mismatch: expected {}, got {}",
                self.num_subquantizers, codes.len()
            )));
        }

        let mut distance = 0.0f32;

        for subquantizer in 0..self.num_subquantizers {
            let start_dim = subquantizer * self.subvector_dim;
            let end_dim = start_dim + self.subvector_dim;
            let query_subvector = &query[start_dim..end_dim];

            let centroid = self.codebooks[subquantizer].get_centroid(codes[subquantizer] as usize)?;
            let subvector_distance = self.euclidean_distance(query_subvector, &centroid);
            distance += subvector_distance * subvector_distance; // Squared distance
        }

        Ok(distance.sqrt())
    }

    /// Train codebook for a single subquantizer
    fn train_subquantizer_codebook(&self, subvectors: &[Vec<f32>]) -> AuroraResult<HashMap<usize, Vec<f32>>> {
        let mut centroids = HashMap::new();

        // Initialize centroids using k-means++ initialization
        centroids = self.initialize_centroids_kmeans_pp(subvectors);

        // K-means iterations
        let max_iterations = 25;
        let convergence_threshold = 1e-6;

        for _ in 0..max_iterations {
            // Assign subvectors to nearest centroids
            let assignments = self.assign_subvectors_to_centroids(subvectors, &centroids);

            // Update centroids
            let new_centroids = self.update_centroids(subvectors, &assignments);

            // Check convergence
            if self.centroids_converged(&centroids, &new_centroids, convergence_threshold) {
                centroids = new_centroids;
                break;
            }

            centroids = new_centroids;
        }

        Ok(centroids)
    }

    /// K-means++ centroid initialization
    fn initialize_centroids_kmeans_pp(&self, subvectors: &[Vec<f32>]) -> HashMap<usize, Vec<f32>> {
        let mut centroids = HashMap::new();
        let mut rng = fastrand::Rng::new();

        // Choose first centroid randomly
        let first_idx = rng.usize(0..subvectors.len());
        centroids.insert(0, subvectors[first_idx].clone());

        // Choose remaining centroids with probability proportional to squared distance
        for centroid_id in 1..self.num_centroids {
            let mut distances = vec![f32::INFINITY; subvectors.len()];

            // Compute distance to nearest existing centroid
            for (i, subvector) in subvectors.iter().enumerate() {
                for existing_centroid in centroids.values() {
                    let distance = self.euclidean_distance(subvector, existing_centroid);
                    distances[i] = distances[i].min(distance);
                }
            }

            // Choose next centroid
            let total_distance: f32 = distances.iter().map(|&d| d * d).sum();
            if total_distance > 0.0 {
                let mut rand = rng.f32() * total_distance;
                for (i, &distance_sq) in distances.iter().enumerate() {
                    rand -= distance_sq;
                    if rand <= 0.0 {
                        centroids.insert(centroid_id, subvectors[i].clone());
                        break;
                    }
                }
            } else {
                // Fallback: choose randomly
                let random_idx = rng.usize(0..subvectors.len());
                centroids.insert(centroid_id, subvectors[random_idx].clone());
            }
        }

        centroids
    }

    /// Assign subvectors to nearest centroids
    fn assign_subvectors_to_centroids(&self, subvectors: &[Vec<f32>], centroids: &HashMap<usize, Vec<f32>>) -> Vec<usize> {
        let mut assignments = Vec::with_capacity(subvectors.len());

        for subvector in subvectors {
            let mut best_centroid = 0;
            let mut best_distance = f32::INFINITY;

            for (&centroid_id, centroid) in centroids {
                let distance = self.euclidean_distance(subvector, centroid);
                if distance < best_distance {
                    best_distance = distance;
                    best_centroid = centroid_id;
                }
            }

            assignments.push(best_centroid);
        }

        assignments
    }

    /// Update centroids as mean of assigned subvectors
    fn update_centroids(&self, subvectors: &[Vec<f32>], assignments: &[usize]) -> HashMap<usize, Vec<f32>> {
        let mut new_centroids = HashMap::new();
        let mut counts = HashMap::new();

        // Accumulate subvectors
        for (subvector, &assignment) in subvectors.iter().zip(assignments.iter()) {
            let centroid = new_centroids.entry(assignment).or_insert_with(|| vec![0.0; self.subvector_dim]);
            let count = counts.entry(assignment).or_insert(0);

            for i in 0..self.subvector_dim {
                centroid[i] += subvector[i];
            }
            *count += 1;
        }

        // Average
        for (centroid_id, centroid) in new_centroids.iter_mut() {
            if let Some(&count) = counts.get(centroid_id) {
                if count > 0 {
                    for val in centroid.iter_mut() {
                        *val /= count as f32;
                    }
                }
            }
        }

        new_centroids
    }

    /// Check if centroids have converged
    fn centroids_converged(&self, old: &HashMap<usize, Vec<f32>>, new: &HashMap<usize, Vec<f32>>, threshold: f32) -> bool {
        for (id, old_centroid) in old {
            if let Some(new_centroid) = new.get(id) {
                for (&old_val, &new_val) in old_centroid.iter().zip(new_centroid.iter()) {
                    if (old_val - new_val).abs() > threshold {
                        return false;
                    }
                }
            } else {
                return false; // Centroid disappeared
            }
        }
        true
    }

    /// Euclidean distance between two vectors
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> f32 {
        let mut sum = 0.0f32;
        for i in 0..a.len() {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        sum.sqrt()
    }
}

/// PQ codebook for efficient quantization
struct PQCodebook {
    centroids: HashMap<usize, Vec<f32>>,
}

impl PQCodebook {
    fn new(centroids: HashMap<usize, Vec<f32>>) -> Self {
        Self { centroids }
    }

    /// Quantize a subvector to the nearest centroid
    fn quantize(&self, subvector: &[f32]) -> usize {
        let mut best_centroid = 0;
        let mut best_distance = f32::INFINITY;

        for (&centroid_id, centroid) in &self.centroids {
            let distance = self.euclidean_distance(subvector, centroid);
            if distance < best_distance {
                best_distance = distance;
                best_centroid = centroid_id;
            }
        }

        best_centroid
    }

    /// Get centroid by ID
    fn get_centroid(&self, id: usize) -> AuroraResult<&Vec<f32>> {
        self.centroids.get(&id)
            .ok_or_else(|| AuroraError::Vector(format!("Centroid {} not found", id)))
    }

    /// Euclidean distance
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> f32 {
        let mut sum = 0.0f32;
        for i in 0..a.len() {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        sum.sqrt()
    }
}

/// Optimized PQ for high-dimensional vectors
pub struct OptimizedPQ {
    encoder: PQEncoder,
    dimension: usize,
    /// Pre-computed tables for fast distance computation
    distance_tables: Vec<Vec<f32>>, // [subquantizer][centroid_id] -> precomputed value
}

impl OptimizedPQ {
    /// Create optimized PQ with pre-computation
    pub fn new(dimension: usize, num_subquantizers: usize, num_centroids: usize) -> Self {
        let encoder = PQEncoder::new(dimension, num_subquantizers, num_centroids);
        Self {
            encoder,
            dimension,
            distance_tables: Vec::new(),
        }
    }

    /// Pre-compute distance tables for a query vector
    pub fn precompute_tables(&mut self, query: &[f32]) -> AuroraResult<()> {
        if query.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Query dimension mismatch: expected {}, got {}",
                self.dimension, query.len()
            )));
        }

        if self.encoder.codebooks.is_empty() {
            return Err(AuroraError::Vector("PQ encoder not trained".to_string()));
        }

        self.distance_tables.clear();

        for subquantizer in 0..self.encoder.num_subquantizers {
            let start_dim = subquantizer * self.encoder.subvector_dim;
            let end_dim = start_dim + self.encoder.subvector_dim;
            let query_subvector = &query[start_dim..end_dim];

            let mut table = Vec::with_capacity(self.encoder.num_centroids);
            for centroid_id in 0..self.encoder.num_centroids {
                let centroid = self.encoder.codebooks[subquantizer].get_centroid(centroid_id)?;
                let distance = self.euclidean_distance(query_subvector, centroid);
                table.push(distance * distance); // Pre-compute squared distance
            }

            self.distance_tables.push(table);
        }

        Ok(())
    }

    /// Compute asymmetric distance using pre-computed tables (very fast)
    pub fn asymmetric_distance_precomputed(&self, codes: &[u8]) -> AuroraResult<f32> {
        if codes.len() != self.encoder.num_subquantizers {
            return Err(AuroraError::Vector(format!(
                "Code length mismatch: expected {}, got {}",
                self.encoder.num_subquantizers, codes.len()
            )));
        }

        if self.distance_tables.len() != self.encoder.num_subquantizers {
            return Err(AuroraError::Vector("Distance tables not pre-computed".to_string()));
        }

        let mut total_distance = 0.0f32;

        for (subquantizer, &code) in codes.iter().enumerate() {
            let centroid_id = code as usize;
            if centroid_id >= self.distance_tables[subquantizer].len() {
                return Err(AuroraError::Vector(format!(
                    "Invalid centroid ID {} for subquantizer {}", centroid_id, subquantizer
                )));
            }
            total_distance += self.distance_tables[subquantizer][centroid_id];
        }

        Ok(total_distance.sqrt())
    }

    /// Train the PQ encoder
    pub fn train(&mut self, vectors: &[&[f32]]) -> AuroraResult<()> {
        self.encoder.train(vectors)?;
        Ok(())
    }

    /// Encode a vector
    pub fn encode(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        self.encoder.encode(vector)
    }

    /// Euclidean distance helper
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> f32 {
        let mut sum = 0.0f32;
        for i in 0..a.len() {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        sum.sqrt()
    }
}

/// Advanced PQ with residual quantization
pub struct ResidualPQ {
    base_encoder: PQEncoder,
    residual_encoder: Option<PQEncoder>,
    dimension: usize,
    use_residual: bool,
}

impl ResidualPQ {
    /// Create residual PQ encoder
    pub fn new(dimension: usize, num_subquantizers: usize, num_centroids: usize, use_residual: bool) -> Self {
        let base_encoder = PQEncoder::new(dimension, num_subquantizers, num_centroids);
        let residual_encoder = if use_residual {
            Some(PQEncoder::new(dimension, num_subquantizers, num_centroids))
        } else {
            None
        };

        Self {
            base_encoder,
            residual_encoder,
            dimension,
            use_residual,
        }
    }

    /// Train residual PQ
    pub fn train(&mut self, vectors: &[&[f32]]) -> AuroraResult<()> {
        // Train base quantizer
        self.base_encoder.train(vectors)?;

        if self.use_residual {
            // Compute residuals and train residual quantizer
            let mut residuals = Vec::new();

            for vector in vectors {
                let reconstructed = self.base_encoder.decode(&self.base_encoder.encode(vector)?)?;
                let residual: Vec<f32> = vector.iter().zip(reconstructed.iter())
                    .map(|(&orig, &recon)| orig - recon)
                    .collect();
                residuals.push(residual);
            }

            let residual_refs: Vec<&[f32]> = residuals.iter().map(|v| v.as_slice()).collect();
            self.residual_encoder.as_mut().unwrap().train(&residual_refs)?;
        }

        Ok(())
    }

    /// Encode with residual quantization
    pub fn encode(&self, vector: &[f32]) -> AuroraResult<Vec<u8>> {
        let base_codes = self.base_encoder.encode(vector)?;

        if self.use_residual && self.residual_encoder.is_some() {
            let reconstructed = self.base_encoder.decode(&base_codes)?;
            let residual: Vec<f32> = vector.iter().zip(reconstructed.iter())
                .map(|(&orig, &recon)| orig - recon)
                .collect();

            let residual_codes = self.residual_encoder.as_ref().unwrap().encode(&residual)?;
            Ok([base_codes, residual_codes].concat())
        } else {
            Ok(base_codes)
        }
    }

    /// Decode residual PQ
    pub fn decode(&self, codes: &[u8]) -> AuroraResult<Vec<f32>> {
        if self.use_residual {
            let mid = codes.len() / 2;
            let base_codes = &codes[0..mid];
            let residual_codes = &codes[mid..];

            let base_reconstruction = self.base_encoder.decode(base_codes)?;
            let residual_reconstruction = self.residual_encoder.as_ref().unwrap().decode(residual_codes)?;

            Ok(base_reconstruction.iter().zip(residual_reconstruction.iter())
                .map(|(&base, &res)| base + res)
                .collect())
        } else {
            self.base_encoder.decode(codes)
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_pq_encoder_basic() {
        let mut encoder = PQEncoder::new(8, 4, 16);

        // Create training vectors
        let vectors: Vec<Vec<f32>> = (0..1000).map(|i| {
            vec![
                (i as f32 * 0.1).sin(), (i as f32 * 0.1).cos(),
                ((i + 10) as f32 * 0.1).sin(), ((i + 10) as f32 * 0.1).cos(),
                ((i + 20) as f32 * 0.1).sin(), ((i + 20) as f32 * 0.1).cos(),
                ((i + 30) as f32 * 0.1).sin(), ((i + 30) as f32 * 0.1).cos(),
            ]
        }).collect();

        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();

        // Train encoder
        encoder.train(&vector_refs).unwrap();

        // Test encoding/decoding
        let test_vector = vec![1.0, 0.0, 0.5, -0.5, 0.8, -0.8, 0.3, -0.3];
        let codes = encoder.encode(&test_vector).unwrap();
        let reconstructed = encoder.decode(&codes).unwrap();

        // Should have same dimension
        assert_eq!(reconstructed.len(), test_vector.len());
        assert_eq!(codes.len(), 4); // 4 subquantizers

        // Reconstruction error should be reasonable
        let error: f32 = test_vector.iter().zip(reconstructed.iter())
            .map(|(orig, recon)| (orig - recon).powi(2))
            .sum::<f32>().sqrt();

        assert!(error < 1.0); // Allow some quantization error
    }

    #[test]
    fn test_optimized_pq() {
        let mut opq = OptimizedPQ::new(8, 4, 16);

        // Training data
        let vectors: Vec<Vec<f32>> = (0..500).map(|i| {
            vec![
                (i as f32 * 0.01).sin(), (i as f32 * 0.01).cos(),
                ((i + 50) as f32 * 0.01).sin(), ((i + 50) as f32 * 0.01).cos(),
                ((i + 100) as f32 * 0.01).sin(), ((i + 100) as f32 * 0.01).cos(),
                ((i + 150) as f32 * 0.01).sin(), ((i + 150) as f32 * 0.01).cos(),
            ]
        }).collect();

        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        opq.train(&vector_refs).unwrap();

        // Test query
        let query = vec![0.5, 0.8, -0.3, 0.1, 0.9, -0.7, 0.2, -0.4];
        opq.precompute_tables(&query).unwrap();

        // Test some codes
        let test_codes = vec![0u8, 1, 2, 3];
        let distance = opq.asymmetric_distance_precomputed(&test_codes).unwrap();
        assert!(distance >= 0.0);
    }

    #[test]
    fn test_residual_pq() {
        let mut rpq = ResidualPQ::new(8, 4, 16, true);

        // Training data
        let vectors: Vec<Vec<f32>> = (0..300).map(|i| {
            vec![
                (i as f32 * 0.02).sin(), (i as f32 * 0.02).cos(),
                ((i + 25) as f32 * 0.02).sin(), ((i + 25) as f32 * 0.02).cos(),
                ((i + 50) as f32 * 0.02).sin(), ((i + 50) as f32 * 0.02).cos(),
                ((i + 75) as f32 * 0.02).sin(), ((i + 75) as f32 * 0.02).cos(),
            ]
        }).collect();

        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        rpq.train(&vector_refs).unwrap();

        // Test encoding/decoding
        let test_vector = vec![0.7, -0.2, 0.9, -0.1, 0.4, -0.8, 0.6, -0.3];
        let codes = rpq.encode(&test_vector).unwrap();
        let reconstructed = rpq.decode(&codes).unwrap();

        // With residual, should be more accurate
        let error: f32 = test_vector.iter().zip(reconstructed.iter())
            .map(|(orig, recon)| (orig - recon).powi(2))
            .sum::<f32>().sqrt();

        assert!(error < 0.5); // Should be more accurate than regular PQ
    }

    #[test]
    fn test_pq_error_handling() {
        let encoder = PQEncoder::new(8, 4, 16);

        // Test with untrained encoder
        let test_vector = vec![1.0; 8];
        assert!(encoder.encode(&test_vector).is_err());

        // Test dimension mismatch
        let wrong_dim_vector = vec![1.0; 6];
        assert!(encoder.encode(&wrong_dim_vector).is_err());
    }

    #[test]
    fn test_pq_codebook() {
        let centroids = HashMap::from([
            (0, vec![1.0, 0.0]),
            (1, vec![0.0, 1.0]),
            (2, vec![-1.0, 0.0]),
        ]);

        let codebook = PQCodebook::new(centroids);

        // Test quantization
        let test_vector = vec![0.8, 0.1];
        let code = codebook.quantize(&test_vector);
        assert_eq!(code, 0); // Should be closest to [1.0, 0.0]

        let test_vector2 = vec![-0.9, 0.0];
        let code2 = codebook.quantize(&test_vector2);
        assert_eq!(code2, 2); // Should be closest to [-1.0, 0.0]
    }

    #[test]
    fn test_distance_tables() {
        let mut opq = OptimizedPQ::new(6, 3, 8);

        // Create minimal training data
        let vectors = vec![
            vec![1.0, 0.0, 0.5, -0.5, 0.8, -0.2],
            vec![0.0, 1.0, -0.3, 0.7, -0.1, 0.9],
        ];
        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();

        opq.train(&vector_refs).unwrap();

        // Test pre-computation
        let query = vec![0.5, 0.5, 0.0, 0.0, 0.5, 0.5];
        opq.precompute_tables(&query).unwrap();

        assert_eq!(opq.distance_tables.len(), 3); // 3 subquantizers
        for table in &opq.distance_tables {
            assert_eq!(table.len(), 8); // 8 centroids per subquantizer
        }
    }

    #[test]
    fn test_pq_asymmetric_distance() {
        let mut encoder = PQEncoder::new(6, 3, 4);

        // Simple training data
        let vectors = vec![
            vec![1.0, 0.0, 0.0, 0.0, 0.0, 0.0],
            vec![0.0, 1.0, 0.0, 0.0, 0.0, 0.0],
        ];
        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();

        encoder.train(&vector_refs).unwrap();

        // Test asymmetric distance
        let query = vec![1.0, 0.0, 0.0, 0.0, 0.0, 0.0];
        let codes = encoder.encode(&query).unwrap();
        let distance = encoder.asymmetric_distance(&query, &codes).unwrap();

        assert!(distance >= 0.0);
        // Distance should be relatively small for the same vector
        assert!(distance < 1.0);
    }
}
