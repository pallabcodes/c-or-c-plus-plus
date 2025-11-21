//! AuroraDB Vector Operations: Essential Vector Mathematics and Utilities
//!
//! Comprehensive vector operations supporting AuroraDB's UNIQUENESS:
//! - SIMD-accelerated mathematical operations
//! - Vector normalization and preprocessing
//! - Batch processing for efficiency
//! - Memory-efficient implementations

use crate::core::errors::{AuroraResult, AuroraError};
use std::arch::x86_64::*;

/// Vector normalization modes
#[derive(Debug, Clone, PartialEq)]
pub enum NormalizationMode {
    /// L2 normalization (unit vectors) - good for cosine similarity
    L2,
    /// L1 normalization - sum of absolute values = 1
    L1,
    /// Min-max normalization to [0,1] range
    MinMax,
    /// Z-score normalization (mean=0, std=1)
    ZScore,
    /// No normalization
    None,
}

/// Vector preprocessing utilities
pub struct VectorPreprocessor {
    normalization_mode: NormalizationMode,
    dimension: usize,
}

impl VectorPreprocessor {
    /// Create a new vector preprocessor
    pub fn new(normalization_mode: NormalizationMode, dimension: usize) -> Self {
        Self {
            normalization_mode,
            dimension,
        }
    }

    /// Preprocess a vector (apply normalization)
    pub fn preprocess(&self, vector: &[f32]) -> AuroraResult<Vec<f32>> {
        if vector.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Vector dimension mismatch: expected {}, got {}",
                self.dimension, vector.len()
            )));
        }

        match self.normalization_mode {
            NormalizationMode::L2 => self.l2_normalize(vector),
            NormalizationMode::L1 => self.l1_normalize(vector),
            NormalizationMode::MinMax => self.minmax_normalize(vector),
            NormalizationMode::ZScore => self.zscore_normalize(vector),
            NormalizationMode::None => Ok(vector.to_vec()),
        }
    }

    /// Batch preprocess multiple vectors
    pub fn preprocess_batch(&self, vectors: &[&[f32]]) -> AuroraResult<Vec<Vec<f32>>> {
        let mut result = Vec::with_capacity(vectors.len());
        for vector in vectors {
            result.push(self.preprocess(vector)?);
        }
        Ok(result)
    }

    /// L2 normalization (Euclidean norm)
    fn l2_normalize(&self, vector: &[f32]) -> AuroraResult<Vec<f32>> {
        let norm = self.euclidean_norm(vector);
        if norm == 0.0 {
            return Ok(vec![0.0; self.dimension]);
        }

        if is_x86_feature_detected!("avx2") && self.dimension >= 8 {
            self.l2_normalize_simd(vector, norm)
        } else {
            Ok(vector.iter().map(|&x| x / norm).collect())
        }
    }

    #[target_feature(enable = "avx2")]
    unsafe fn l2_normalize_simd(&self, vector: &[f32], norm: f32) -> AuroraResult<Vec<f32>> {
        let mut result = vec![0.0; self.dimension];
        let norm_vec = _mm256_set1_ps(norm);

        let chunks = self.dimension / 8;
        for i in 0..chunks {
            let offset = i * 8;
            let vec_data = _mm256_loadu_ps(vector.as_ptr().add(offset));
            let normalized = _mm256_div_ps(vec_data, norm_vec);
            _mm256_storeu_ps(result.as_mut_ptr().add(offset), normalized);
        }

        // Handle remaining elements
        for i in (chunks * 8)..self.dimension {
            result[i] = vector[i] / norm;
        }

        Ok(result)
    }

    /// L1 normalization
    fn l1_normalize(&self, vector: &[f32]) -> AuroraResult<Vec<f32>> {
        let sum: f32 = vector.iter().map(|&x| x.abs()).sum();
        if sum == 0.0 {
            return Ok(vec![0.0; self.dimension]);
        }
        Ok(vector.iter().map(|&x| x / sum).collect())
    }

    /// Min-max normalization
    fn minmax_normalize(&self, vector: &[f32]) -> AuroraResult<Vec<f32>> {
        let min_val = vector.iter().fold(f32::INFINITY, |a, &b| a.min(b));
        let max_val = vector.iter().fold(f32::NEG_INFINITY, |a, &b| a.max(b));

        if (max_val - min_val).abs() < 1e-10 {
            return Ok(vec![0.5; self.dimension]); // All values same, map to 0.5
        }

        Ok(vector.iter().map(|&x| (x - min_val) / (max_val - min_val)).collect())
    }

    /// Z-score normalization
    fn zscore_normalize(&self, vector: &[f32]) -> AuroraResult<Vec<f32>> {
        let mean = vector.iter().sum::<f32>() / vector.len() as f32;
        let variance = vector.iter().map(|&x| (x - mean).powi(2)).sum::<f32>() / vector.len() as f32;
        let std_dev = variance.sqrt();

        if std_dev == 0.0 {
            return Ok(vec![0.0; self.dimension]);
        }

        Ok(vector.iter().map(|&x| (x - mean) / std_dev).collect())
    }

    /// Compute Euclidean norm
    fn euclidean_norm(&self, vector: &[f32]) -> f32 {
        if is_x86_feature_detected!("avx2") && self.dimension >= 8 {
            unsafe { self.euclidean_norm_simd(vector) }
        } else {
            vector.iter().map(|&x| x * x).sum::<f32>().sqrt()
        }
    }

    #[target_feature(enable = "avx2")]
    unsafe fn euclidean_norm_simd(&self, vector: &[f32]) -> f32 {
        let mut sum = 0.0f32;

        let chunks = self.dimension / 8;
        for i in 0..chunks {
            let offset = i * 8;
            let vec_data = _mm256_loadu_ps(vector.as_ptr().add(offset));
            let sq = _mm256_mul_ps(vec_data, vec_data);
            let sq_sum = _mm256_hadd_ps(sq, sq);
            sum += _mm256_cvtss_f32(_mm256_castps256_ps128(sq_sum)) +
                  _mm256_cvtss_f32(_mm256_extractf128_ps(sq_sum, 1));
        }

        // Handle remaining elements
        for i in (chunks * 8)..self.dimension {
            sum += vector[i] * vector[i];
        }

        sum.sqrt()
    }
}

/// Vector batch processor for efficient operations on multiple vectors
pub struct VectorBatchProcessor {
    dimension: usize,
    batch_size: usize,
}

impl VectorBatchProcessor {
    pub fn new(dimension: usize, batch_size: usize) -> Self {
        Self {
            dimension,
            batch_size,
        }
    }

    /// Compute centroids of vector clusters
    pub fn compute_centroids(&self, vectors: &[&[f32]], assignments: &[usize], num_clusters: usize) -> AuroraResult<Vec<Vec<f32>>> {
        let mut centroids = vec![vec![0.0; self.dimension]; num_clusters];
        let mut counts = vec![0; num_clusters];

        // Accumulate vectors
        for (i, &cluster_id) in assignments.iter().enumerate() {
            if cluster_id >= num_clusters {
                continue;
            }

            counts[cluster_id] += 1;
            for j in 0..self.dimension {
                centroids[cluster_id][j] += vectors[i][j];
            }
        }

        // Average
        for cluster_id in 0..num_clusters {
            if counts[cluster_id] > 0 {
                let count = counts[cluster_id] as f32;
                for j in 0..self.dimension {
                    centroids[cluster_id][j] /= count;
                }
            }
        }

        Ok(centroids)
    }

    /// Compute pairwise distances between two sets of vectors
    pub fn pairwise_distances(&self, set_a: &[&[f32]], set_b: &[&[f32]]) -> AuroraResult<Vec<Vec<f32>>> {
        let mut distances = Vec::with_capacity(set_a.len());

        for &vec_a in set_a {
            let mut row_distances = Vec::with_capacity(set_b.len());
            for &vec_b in set_b {
                let distance = self.euclidean_distance(vec_a, vec_b);
                row_distances.push(distance);
            }
            distances.push(row_distances);
        }

        Ok(distances)
    }

    /// Find k nearest neighbors for multiple queries
    pub fn batch_knn(&self, queries: &[&[f32]], vectors: &[&[f32]], k: usize) -> AuroraResult<Vec<Vec<(usize, f32)>>> {
        let mut results = Vec::with_capacity(queries.len());

        for &query in queries {
            let mut candidates: Vec<(usize, f32)> = vectors.iter().enumerate()
                .map(|(i, vec)| (i, self.euclidean_distance(query, vec)))
                .collect();

            candidates.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap());
            candidates.truncate(k);
            results.push(candidates);
        }

        Ok(results)
    }

    /// Euclidean distance between two vectors
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> f32 {
        let mut sum = 0.0f32;
        for i in 0..self.dimension {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        sum.sqrt()
    }
}

/// Vector quantization utilities
pub struct VectorQuantizer {
    dimension: usize,
    codebook_size: usize,
    codebook: Vec<Vec<f32>>,
}

impl VectorQuantizer {
    pub fn new(dimension: usize, codebook_size: usize) -> Self {
        Self {
            dimension,
            codebook_size,
            codebook: Vec::new(),
        }
    }

    /// Train quantizer on training vectors
    pub fn train(&mut self, vectors: &[&[f32]], max_iterations: usize) -> AuroraResult<()> {
        // Initialize codebook randomly
        self.initialize_codebook(vectors);

        // Lloyd's algorithm (k-means for quantization)
        for _ in 0..max_iterations {
            let assignments = self.assign_vectors_to_codewords(vectors);
            let new_codebook = self.update_codewords(vectors, &assignments);

            // Check convergence
            if self.codebook_converged(&new_codebook) {
                break;
            }
            self.codebook = new_codebook;
        }

        Ok(())
    }

    /// Quantize a vector
    pub fn quantize(&self, vector: &[f32]) -> AuroraResult<(usize, f32)> {
        let mut best_codeword = 0;
        let mut best_distance = f32::INFINITY;

        for (i, codeword) in self.codebook.iter().enumerate() {
            let distance = self.euclidean_distance(vector, codeword);
            if distance < best_distance {
                best_distance = distance;
                best_codeword = i;
            }
        }

        Ok((best_codeword, best_distance))
    }

    /// Batch quantize multiple vectors
    pub fn quantize_batch(&self, vectors: &[&[f32]]) -> AuroraResult<Vec<(usize, f32)>> {
        let mut results = Vec::with_capacity(vectors.len());
        for vector in vectors {
            results.push(self.quantize(vector)?);
        }
        Ok(results)
    }

    /// Initialize codebook using k-means++ initialization
    fn initialize_codebook(&mut self, vectors: &[&[f32]]) {
        self.codebook.clear();

        // Choose first centroid randomly
        let first_idx = fastrand::usize(0..vectors.len());
        self.codebook.push(vectors[first_idx].to_vec());

        // Choose remaining centroids using k-means++ probability
        for _ in 1..self.codebook_size {
            let mut distances = vec![f32::INFINITY; vectors.len()];

            // Compute distances to nearest existing centroid
            for (i, vector) in vectors.iter().enumerate() {
                for centroid in &self.codebook {
                    let distance = self.euclidean_distance(vector, centroid);
                    distances[i] = distances[i].min(distance);
                }
            }

            // Choose next centroid with probability proportional to squared distance
            let total_distance: f32 = distances.iter().map(|&d| d * d).sum();
            let mut rand = fastrand::f32() * total_distance;

            for (i, &distance_sq) in distances.iter().enumerate() {
                rand -= distance_sq;
                if rand <= 0.0 {
                    self.codebook.push(vectors[i].to_vec());
                    break;
                }
            }
        }
    }

    /// Assign vectors to nearest codewords
    fn assign_vectors_to_codewords(&self, vectors: &[&[f32]]) -> Vec<usize> {
        let mut assignments = Vec::with_capacity(vectors.len());

        for vector in vectors {
            let (codeword_idx, _) = self.quantize(vector).unwrap();
            assignments.push(codeword_idx);
        }

        assignments
    }

    /// Update codewords as centroids of assigned vectors
    fn update_codewords(&self, vectors: &[&[f32]], assignments: &[usize]) -> Vec<Vec<f32>> {
        let mut new_codebook = vec![vec![0.0; self.dimension]; self.codebook_size];
        let mut counts = vec![0; self.codebook_size];

        // Accumulate vectors
        for (vector, &assignment) in vectors.iter().zip(assignments.iter()) {
            counts[assignment] += 1;
            for i in 0..self.dimension {
                new_codebook[assignment][i] += vector[i];
            }
        }

        // Average
        for i in 0..self.codebook_size {
            if counts[i] > 0 {
                let count = counts[i] as f32;
                for j in 0..self.dimension {
                    new_codebook[i][j] /= count;
                }
            } else {
                // Keep old codeword if no vectors assigned
                new_codebook[i] = self.codebook[i].clone();
            }
        }

        new_codebook
    }

    /// Check if codebook has converged
    fn codebook_converged(&self, new_codebook: &[Vec<f32>]) -> bool {
        let tolerance = 1e-6;

        for (old, new) in self.codebook.iter().zip(new_codebook.iter()) {
            for (&old_val, &new_val) in old.iter().zip(new.iter()) {
                if (old_val - new_val).abs() > tolerance {
                    return false;
                }
            }
        }

        true
    }

    /// Euclidean distance between vectors
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> f32 {
        let mut sum = 0.0f32;
        for i in 0..self.dimension {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        sum.sqrt()
    }
}

/// Vector validation utilities
pub struct VectorValidator;

impl VectorValidator {
    /// Validate vector dimensions
    pub fn validate_dimensions(vectors: &[&[f32]], expected_dim: usize) -> AuroraResult<()> {
        for (i, vector) in vectors.iter().enumerate() {
            if vector.len() != expected_dim {
                return Err(AuroraError::Vector(format!(
                    "Vector {} has dimension {}, expected {}",
                    i, vector.len(), expected_dim
                )));
            }
        }
        Ok(())
    }

    /// Check for NaN or infinite values
    pub fn check_finite_values(vector: &[f32]) -> AuroraResult<()> {
        for (i, &val) in vector.iter().enumerate() {
            if !val.is_finite() {
                return Err(AuroraError::Vector(format!(
                    "Vector contains non-finite value at index {}: {}",
                    i, val
                )));
            }
        }
        Ok(())
    }

    /// Validate vector range
    pub fn validate_range(vector: &[f32], min_val: f32, max_val: f32) -> AuroraResult<()> {
        for (i, &val) in vector.iter().enumerate() {
            if val < min_val || val > max_val {
                return Err(AuroraError::Vector(format!(
                    "Vector value at index {} ({}) is outside range [{}, {}]",
                    i, val, min_val, max_val
                )));
            }
        }
        Ok(())
    }

    /// Batch validate multiple vectors
    pub fn validate_batch(vectors: &[&[f32]], expected_dim: usize, min_val: f32, max_val: f32) -> AuroraResult<()> {
        Self::validate_dimensions(vectors, expected_dim)?;

        for vector in vectors {
            Self::check_finite_values(vector)?;
            Self::validate_range(vector, min_val, max_val)?;
        }

        Ok(())
    }
}

/// Vector I/O utilities
pub struct VectorIO;

impl VectorIO {
    /// Load vectors from CSV file
    pub fn load_csv(path: &str, has_header: bool, dimension: usize) -> AuroraResult<Vec<Vec<f32>>> {
        let content = std::fs::read_to_string(path)?;
        let mut vectors = Vec::new();

        for (line_num, line) in content.lines().enumerate() {
            if has_header && line_num == 0 {
                continue;
            }

            let parts: Vec<&str> = line.split(',').collect();
            if parts.len() != dimension {
                return Err(AuroraError::Vector(format!(
                    "Line {} has {} values, expected {}",
                    line_num + 1, parts.len(), dimension
                )));
            }

            let mut vector = Vec::new();
            for part in parts {
                let val: f32 = part.trim().parse().map_err(|_| {
                    AuroraError::Vector(format!("Invalid float value: {}", part))
                })?;
                vector.push(val);
            }

            vectors.push(vector);
        }

        Ok(vectors)
    }

    /// Save vectors to CSV file
    pub fn save_csv(vectors: &[&[f32]], path: &str) -> AuroraResult<()> {
        use std::io::Write;

        let mut file = std::fs::File::create(path)?;
        for vector in vectors {
            let line = vector.iter()
                .map(|&x| x.to_string())
                .collect::<Vec<_>>()
                .join(",");
            writeln!(file, "{}", line)?;
        }

        Ok(())
    }

    /// Load vectors from binary format
    pub fn load_binary(path: &str, dimension: usize) -> AuroraResult<Vec<Vec<f32>>> {
        let data = std::fs::read(path)?;
        let bytes_per_vector = dimension * 4; // f32 = 4 bytes

        if data.len() % bytes_per_vector != 0 {
            return Err(AuroraError::Vector("Invalid binary file size".to_string()));
        }

        let num_vectors = data.len() / bytes_per_vector;
        let mut vectors = Vec::with_capacity(num_vectors);

        for i in 0..num_vectors {
            let start = i * bytes_per_vector;
            let end = start + bytes_per_vector;
            let vector_bytes = &data[start..end];

            let mut vector = Vec::new();
            for chunk in vector_bytes.chunks(4) {
                let bytes: [u8; 4] = chunk.try_into().unwrap();
                let val = f32::from_le_bytes(bytes);
                vector.push(val);
            }

            vectors.push(vector);
        }

        Ok(vectors)
    }

    /// Save vectors to binary format
    pub fn save_binary(vectors: &[&[f32]], path: &str) -> AuroraResult<()> {
        let mut data = Vec::new();

        for vector in vectors {
            for &val in vector {
                data.extend_from_slice(&val.to_le_bytes());
            }
        }

        std::fs::write(path, data)?;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_l2_normalization() {
        let preprocessor = VectorPreprocessor::new(NormalizationMode::L2, 3);
        let vector = vec![3.0, 4.0, 0.0];
        let normalized = preprocessor.preprocess(&vector).unwrap();

        let norm: f32 = normalized.iter().map(|&x| x * x).sum::<f32>().sqrt();
        assert!((norm - 1.0).abs() < 1e-6);
    }

    #[test]
    fn test_l1_normalization() {
        let preprocessor = VectorPreprocessor::new(NormalizationMode::L1, 3);
        let vector = vec![1.0, 2.0, 3.0];
        let normalized = preprocessor.preprocess(&vector).unwrap();

        let sum: f32 = normalized.iter().map(|&x| x.abs()).sum();
        assert!((sum - 1.0).abs() < 1e-6);
    }

    #[test]
    fn test_minmax_normalization() {
        let preprocessor = VectorPreprocessor::new(NormalizationMode::MinMax, 3);
        let vector = vec![1.0, 2.0, 3.0];
        let normalized = preprocessor.preprocess(&vector).unwrap();

        assert!(normalized.iter().all(|&x| x >= 0.0 && x <= 1.0));
        assert_eq!(normalized[0], 0.0); // min value
        assert_eq!(normalized[2], 1.0); // max value
    }

    #[test]
    fn test_batch_preprocessing() {
        let preprocessor = VectorPreprocessor::new(NormalizationMode::L2, 3);
        let vectors = vec![
            vec![1.0, 0.0, 0.0],
            vec![0.0, 1.0, 0.0],
            vec![0.0, 0.0, 1.0],
        ];
        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();

        let processed = preprocessor.preprocess_batch(&vector_refs).unwrap();
        assert_eq!(processed.len(), 3);

        for processed_vec in processed {
            let norm: f32 = processed_vec.iter().map(|&x| x * x).sum::<f32>().sqrt();
            assert!((norm - 1.0).abs() < 1e-6);
        }
    }

    #[test]
    fn test_batch_processor_centroids() {
        let processor = VectorBatchProcessor::new(3, 32);

        let vectors = vec![
            vec![1.0, 2.0, 3.0],
            vec![1.1, 2.1, 3.1],
            vec![4.0, 5.0, 6.0],
            vec![4.1, 5.1, 6.1],
        ];
        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();
        let assignments = vec![0, 0, 1, 1]; // Two clusters

        let centroids = processor.compute_centroids(&vector_refs, &assignments, 2).unwrap();
        assert_eq!(centroids.len(), 2);

        // First cluster centroid should be average of first two vectors
        assert!((centroids[0][0] - 1.05).abs() < 1e-6);
        assert!((centroids[0][1] - 2.05).abs() < 1e-6);

        // Second cluster centroid should be average of last two vectors
        assert!((centroids[1][0] - 4.05).abs() < 1e-6);
        assert!((centroids[1][1] - 5.05).abs() < 1e-6);
    }

    #[test]
    fn test_vector_quantizer() {
        let mut quantizer = VectorQuantizer::new(3, 4);

        let vectors = vec![
            vec![1.0, 0.0, 0.0],
            vec![0.0, 1.0, 0.0],
            vec![0.0, 0.0, 1.0],
            vec![1.0, 1.0, 1.0],
        ];
        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();

        quantizer.train(&vector_refs, 10).unwrap();

        // Quantize a test vector
        let test_vector = vec![0.9, 0.1, 0.1];
        let (codeword, distance) = quantizer.quantize(&test_vector).unwrap();

        assert!(codeword < 4); // Valid codeword index
        assert!(distance >= 0.0); // Valid distance
    }

    #[test]
    fn test_vector_validation() {
        // Valid vectors
        let vectors = vec![
            vec![1.0, 2.0, 3.0],
            vec![4.0, 5.0, 6.0],
        ];
        let vector_refs: Vec<&[f32]> = vectors.iter().map(|v| v.as_slice()).collect();

        assert!(VectorValidator::validate_batch(&vector_refs, 3, f32::NEG_INFINITY, f32::INFINITY).is_ok());

        // Invalid dimension
        let invalid_vectors = vec![vec![1.0, 2.0], vec![3.0, 4.0, 5.0, 6.0]];
        let invalid_refs: Vec<&[f32]> = invalid_vectors.iter().map(|v| v.as_slice()).collect();
        assert!(VectorValidator::validate_batch(&invalid_refs, 3, f32::NEG_INFINITY, f32::INFINITY).is_err());

        // Non-finite values
        let nan_vector = vec![1.0, f32::NAN, 3.0];
        assert!(VectorValidator::check_finite_values(&nan_vector).is_err());

        // Out of range
        let oor_vector = vec![1.0, 2.0, 3.0];
        assert!(VectorValidator::validate_range(&oor_vector, 0.0, 2.0).is_err());
    }

    #[test]
    fn test_preprocessing_modes() {
        let vector = vec![1.0, 2.0, 3.0, 4.0];

        // Test different normalization modes
        let modes = vec![
            NormalizationMode::L2,
            NormalizationMode::L1,
            NormalizationMode::MinMax,
            NormalizationMode::ZScore,
            NormalizationMode::None,
        ];

        for mode in modes {
            let preprocessor = VectorPreprocessor::new(mode, 4);
            let result = preprocessor.preprocess(&vector).unwrap();

            // All results should be finite
            assert!(result.iter().all(|&x| x.is_finite()));
        }
    }

    #[test]
    fn test_batch_operations() {
        let processor = VectorBatchProcessor::new(3, 16);

        let set_a = vec![
            vec![0.0, 0.0, 0.0],
            vec![1.0, 0.0, 0.0],
        ];
        let set_b = vec![
            vec![0.0, 0.0, 0.0],
            vec![0.0, 1.0, 0.0],
        ];

        let a_refs: Vec<&[f32]> = set_a.iter().map(|v| v.as_slice()).collect();
        let b_refs: Vec<&[f32]> = set_b.iter().map(|v| v.as_slice()).collect();

        // Test pairwise distances
        let distances = processor.pairwise_distances(&a_refs, &b_refs).unwrap();
        assert_eq!(distances.len(), 2);
        assert_eq!(distances[0].len(), 2);
        assert_eq!(distances[0][0], 0.0); // Distance from (0,0,0) to (0,0,0)
        assert_eq!(distances[0][1], 1.0); // Distance from (0,0,0) to (0,1,0)

        // Test batch KNN
        let queries = vec![vec![0.0, 0.0, 0.0]];
        let query_refs: Vec<&[f32]> = queries.iter().map(|v| v.as_slice()).collect();

        let knn_results = processor.batch_knn(&query_refs, &b_refs, 2).unwrap();
        assert_eq!(knn_results.len(), 1);
        assert_eq!(knn_results[0].len(), 2);
        assert_eq!(knn_results[0][0].0, 0); // Closest point
        assert_eq!(knn_results[0][1].0, 1); // Second closest
    }
}
