//! AuroraDB Distance Metrics: Optimized Similarity Computation
//!
//! High-performance distance calculations with SIMD acceleration and
//! hardware-specific optimizations for different vector dimensions.

use std::arch::x86_64::*;
use crate::core::errors::{AuroraResult, AuroraError};

/// Supported distance metrics for vector similarity search
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum DistanceMetric {
    /// Cosine similarity: measures angle between vectors (normalized)
    Cosine,
    /// Euclidean distance: straight-line distance in vector space
    Euclidean,
    /// Dot product: unnormalized similarity measure
    DotProduct,
    /// Manhattan distance: sum of absolute differences
    Manhattan,
    /// Hamming distance: count of different bits (binary vectors)
    Hamming,
    /// Jaccard similarity: intersection over union (sparse vectors)
    Jaccard,
}

/// SIMD-accelerated distance computation
pub struct DistanceComputer {
    metric: DistanceMetric,
    dimension: usize,
    use_simd: bool,
}

impl DistanceComputer {
    /// Create a new distance computer
    pub fn new(metric: DistanceMetric, dimension: usize) -> Self {
        let use_simd = is_x86_feature_detected!("avx2") && dimension % 8 == 0;
        Self {
            metric,
            dimension,
            use_simd,
        }
    }

    /// Compute distance between two vectors
    pub fn compute(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        if a.len() != b.len() || a.len() != self.dimension {
            return Err(AuroraError::Vector(format!(
                "Vector dimension mismatch: expected {}, got {} and {}",
                self.dimension, a.len(), b.len()
            )));
        }

        match self.metric {
            DistanceMetric::Cosine => self.cosine_similarity(a, b),
            DistanceMetric::Euclidean => self.euclidean_distance(a, b),
            DistanceMetric::DotProduct => self.dot_product(a, b),
            DistanceMetric::Manhattan => self.manhattan_distance(a, b),
            DistanceMetric::Hamming => self.hamming_distance(a, b),
            DistanceMetric::Jaccard => self.jaccard_similarity(a, b),
        }
    }

    /// Compute distance between query vector and multiple candidates (batch processing)
    pub fn compute_batch(&self, query: &[f32], candidates: &[&[f32]]) -> AuroraResult<Vec<f32>> {
        let mut distances = Vec::with_capacity(candidates.len());

        for candidate in candidates {
            distances.push(self.compute(query, candidate)?);
        }

        Ok(distances)
    }

    /// Cosine similarity: angle between vectors (higher = more similar)
    fn cosine_similarity(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        if self.use_simd && self.dimension >= 8 {
            self.cosine_similarity_simd(a, b)
        } else {
            self.cosine_similarity_scalar(a, b)
        }
    }

    fn cosine_similarity_scalar(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut dot_product = 0.0f32;
        let mut norm_a = 0.0f32;
        let mut norm_b = 0.0f32;

        for i in 0..self.dimension {
            dot_product += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }

        let norm_a = norm_a.sqrt();
        let norm_b = norm_b.sqrt();

        if norm_a == 0.0 || norm_b == 0.0 {
            return Ok(0.0);
        }

        Ok(dot_product / (norm_a * norm_b))
    }

    #[target_feature(enable = "avx2")]
    unsafe fn cosine_similarity_simd(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut dot_product = 0.0f32;
        let mut norm_a = 0.0f32;
        let mut norm_b = 0.0f32;

        let chunks = self.dimension / 8;
        for i in 0..chunks {
            let offset = i * 8;
            let a_vec = _mm256_loadu_ps(a.as_ptr().add(offset));
            let b_vec = _mm256_loadu_ps(b.as_ptr().add(offset));

            // Dot product
            let mul = _mm256_mul_ps(a_vec, b_vec);
            let sum = _mm256_hadd_ps(mul, mul);
            dot_product += _mm256_cvtss_f32(_mm256_castps256_ps128(sum)) +
                          _mm256_cvtss_f32(_mm256_extractf128_ps(sum, 1));

            // Norms
            let a_sq = _mm256_mul_ps(a_vec, a_vec);
            let b_sq = _mm256_mul_ps(b_vec, b_vec);
            let a_sum = _mm256_hadd_ps(a_sq, a_sq);
            let b_sum = _mm256_hadd_ps(b_sq, b_sq);
            norm_a += _mm256_cvtss_f32(_mm256_castps256_ps128(a_sum)) +
                     _mm256_cvtss_f32(_mm256_extractf128_ps(a_sum, 1));
            norm_b += _mm256_cvtss_f32(_mm256_castps256_ps128(b_sum)) +
                     _mm256_cvtss_f32(_mm256_extractf128_ps(b_sum, 1));
        }

        // Handle remaining elements
        for i in (chunks * 8)..self.dimension {
            dot_product += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }

        let norm_a = norm_a.sqrt();
        let norm_b = norm_b.sqrt();

        if norm_a == 0.0 || norm_b == 0.0 {
            return Ok(0.0);
        }

        Ok(dot_product / (norm_a * norm_b))
    }

    /// Euclidean distance: straight-line distance
    fn euclidean_distance(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        if self.use_simd && self.dimension >= 8 {
            self.euclidean_distance_simd(a, b)
        } else {
            self.euclidean_distance_scalar(a, b)
        }
    }

    fn euclidean_distance_scalar(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut sum = 0.0f32;
        for i in 0..self.dimension {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }
        Ok(sum.sqrt())
    }

    #[target_feature(enable = "avx2")]
    unsafe fn euclidean_distance_simd(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut sum = 0.0f32;

        let chunks = self.dimension / 8;
        for i in 0..chunks {
            let offset = i * 8;
            let a_vec = _mm256_loadu_ps(a.as_ptr().add(offset));
            let b_vec = _mm256_loadu_ps(b.as_ptr().add(offset));

            let diff = _mm256_sub_ps(a_vec, b_vec);
            let sq = _mm256_mul_ps(diff, diff);
            let sq_sum = _mm256_hadd_ps(sq, sq);
            sum += _mm256_cvtss_f32(_mm256_castps256_ps128(sq_sum)) +
                  _mm256_cvtss_f32(_mm256_extractf128_ps(sq_sum, 1));
        }

        // Handle remaining elements
        for i in (chunks * 8)..self.dimension {
            let diff = a[i] - b[i];
            sum += diff * diff;
        }

        Ok(sum.sqrt())
    }

    /// Dot product similarity
    fn dot_product(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        if self.use_simd && self.dimension >= 8 {
            self.dot_product_simd(a, b)
        } else {
            self.dot_product_scalar(a, b)
        }
    }

    fn dot_product_scalar(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut sum = 0.0f32;
        for i in 0..self.dimension {
            sum += a[i] * b[i];
        }
        Ok(sum)
    }

    #[target_feature(enable = "avx2")]
    unsafe fn dot_product_simd(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut sum = 0.0f32;

        let chunks = self.dimension / 8;
        for i in 0..chunks {
            let offset = i * 8;
            let a_vec = _mm256_loadu_ps(a.as_ptr().add(offset));
            let b_vec = _mm256_loadu_ps(b.as_ptr().add(offset));

            let mul = _mm256_mul_ps(a_vec, b_vec);
            let mul_sum = _mm256_hadd_ps(mul, mul);
            sum += _mm256_cvtss_f32(_mm256_castps256_ps128(mul_sum)) +
                  _mm256_cvtss_f32(_mm256_extractf128_ps(mul_sum, 1));
        }

        // Handle remaining elements
        for i in (chunks * 8)..self.dimension {
            sum += a[i] * b[i];
        }

        Ok(sum)
    }

    /// Manhattan distance: sum of absolute differences
    fn manhattan_distance(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut sum = 0.0f32;
        for i in 0..self.dimension {
            sum += (a[i] - b[i]).abs();
        }
        Ok(sum)
    }

    /// Hamming distance: count of different bits (for binary vectors)
    fn hamming_distance(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut distance = 0u32;
        for i in 0..self.dimension {
            let a_bits = a[i].to_bits();
            let b_bits = b[i].to_bits();
            distance += (a_bits ^ b_bits).count_ones();
        }
        Ok(distance as f32)
    }

    /// Jaccard similarity: intersection over union (for sparse vectors)
    fn jaccard_similarity(&self, a: &[f32], b: &[f32]) -> AuroraResult<f32> {
        let mut intersection = 0usize;
        let mut union = 0usize;

        for i in 0..self.dimension {
            let a_nonzero = a[i] != 0.0;
            let b_nonzero = b[i] != 0.0;

            if a_nonzero && b_nonzero {
                intersection += 1;
            }
            if a_nonzero || b_nonzero {
                union += 1;
            }
        }

        if union == 0 {
            return Ok(1.0); // Both vectors are empty
        }

        Ok(intersection as f32 / union as f32)
    }
}

/// Precomputed distance cache for frequently accessed vectors
pub struct DistanceCache {
    cache: std::collections::HashMap<(u64, u64), f32>,
    max_size: usize,
    metric: DistanceMetric,
}

impl DistanceCache {
    pub fn new(metric: DistanceMetric, max_size: usize) -> Self {
        Self {
            cache: std::collections::HashMap::new(),
            max_size,
            metric,
        }
    }

    pub fn get_or_compute(&mut self, id_a: u64, id_b: u64, computer: &DistanceComputer, vec_a: &[f32], vec_b: &[f32]) -> AuroraResult<f32> {
        let key = if id_a < id_b { (id_a, id_b) } else { (id_b, id_a) };

        if let Some(&distance) = self.cache.get(&key) {
            return Ok(distance);
        }

        let distance = computer.compute(vec_a, vec_b)?;

        // Cache the result if we haven't exceeded max size
        if self.cache.len() < self.max_size {
            self.cache.insert(key, distance);
        }

        Ok(distance)
    }

    pub fn clear(&mut self) {
        self.cache.clear();
    }

    pub fn size(&self) -> usize {
        self.cache.len()
    }
}

/// Batch distance computation for multiple queries
pub struct BatchDistanceComputer {
    computer: DistanceComputer,
    max_batch_size: usize,
}

impl BatchDistanceComputer {
    pub fn new(metric: DistanceMetric, dimension: usize, max_batch_size: usize) -> Self {
        Self {
            computer: DistanceComputer::new(metric, dimension),
            max_batch_size,
        }
    }

    /// Compute distances between one query and multiple candidates efficiently
    pub fn compute_query_candidates(&self, query: &[f32], candidates: &[&[f32]]) -> AuroraResult<Vec<f32>> {
        let mut distances = Vec::with_capacity(candidates.len());

        // Process in batches for better cache locality
        for chunk in candidates.chunks(self.max_batch_size) {
            for candidate in chunk {
                distances.push(self.computer.compute(query, candidate)?);
            }
        }

        Ok(distances)
    }

    /// Compute all pairwise distances between two sets of vectors
    pub fn compute_pairwise(&self, set_a: &[&[f32]], set_b: &[&[f32]]) -> AuroraResult<Vec<Vec<f32>>> {
        let mut distances = Vec::with_capacity(set_a.len());

        for vec_a in set_a {
            let row_distances = self.compute_query_candidates(vec_a, set_b)?;
            distances.push(row_distances);
        }

        Ok(distances)
    }
}

/// Distance metric selector based on use case
pub struct DistanceMetricSelector;

impl DistanceMetricSelector {
    /// Select optimal distance metric based on use case
    pub fn select_for_usecase(usecase: VectorUseCase) -> DistanceMetric {
        match usecase {
            VectorUseCase::SemanticSearch => DistanceMetric::Cosine,
            VectorUseCase::ImageSimilarity => DistanceMetric::Cosine,
            VectorUseCase::Recommendation => DistanceMetric::DotProduct,
            VectorUseCase::Clustering => DistanceMetric::Euclidean,
            VectorUseCase::AnomalyDetection => DistanceMetric::Manhattan,
            VectorUseCase::BinaryClassification => DistanceMetric::Hamming,
            VectorUseCase::TextSimilarity => DistanceMetric::Cosine,
            VectorUseCase::SparseData => DistanceMetric::Jaccard,
        }
    }

    /// Get metric properties
    pub fn get_properties(metric: &DistanceMetric) -> DistanceProperties {
        match metric {
            DistanceMetric::Cosine => DistanceProperties {
                range: (-1.0, 1.0),
                higher_is_similar: true,
                normalized: true,
                sparse_friendly: false,
            },
            DistanceMetric::Euclidean => DistanceProperties {
                range: (0.0, f32::INFINITY),
                higher_is_similar: false,
                normalized: false,
                sparse_friendly: false,
            },
            DistanceMetric::DotProduct => DistanceProperties {
                range: (f32::NEG_INFINITY, f32::INFINITY),
                higher_is_similar: true,
                normalized: false,
                sparse_friendly: false,
            },
            DistanceMetric::Manhattan => DistanceProperties {
                range: (0.0, f32::INFINITY),
                higher_is_similar: false,
                normalized: false,
                sparse_friendly: true,
            },
            DistanceMetric::Hamming => DistanceProperties {
                range: (0.0, f32::INFINITY),
                higher_is_similar: false,
                normalized: false,
                sparse_friendly: false,
            },
            DistanceMetric::Jaccard => DistanceProperties {
                range: (0.0, 1.0),
                higher_is_similar: true,
                normalized: true,
                sparse_friendly: true,
            },
        }
    }
}

/// Properties of a distance metric
#[derive(Debug, Clone)]
pub struct DistanceProperties {
    pub range: (f32, f32),
    pub higher_is_similar: bool, // true if higher values mean more similar
    pub normalized: bool,        // true if values are in [0,1] or [-1,1]
    pub sparse_friendly: bool,   // true if good for sparse vectors
}

/// Vector use cases for metric selection
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum VectorUseCase {
    SemanticSearch,
    ImageSimilarity,
    Recommendation,
    Clustering,
    AnomalyDetection,
    BinaryClassification,
    TextSimilarity,
    SparseData,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_cosine_similarity() {
        let computer = DistanceComputer::new(DistanceMetric::Cosine, 3);
        let a = vec![1.0, 0.0, 0.0];
        let b = vec![0.0, 1.0, 0.0];
        let distance = computer.compute(&a, &b).unwrap();
        assert!((distance - 0.0).abs() < 1e-6); // Orthogonal vectors

        let a = vec![1.0, 2.0, 3.0];
        let b = vec![1.0, 2.0, 3.0];
        let distance = computer.compute(&a, &b).unwrap();
        assert!((distance - 1.0).abs() < 1e-6); // Identical vectors
    }

    #[test]
    fn test_euclidean_distance() {
        let computer = DistanceComputer::new(DistanceMetric::Euclidean, 3);
        let a = vec![0.0, 0.0, 0.0];
        let b = vec![3.0, 4.0, 0.0];
        let distance = computer.compute(&a, &b).unwrap();
        assert!((distance - 5.0).abs() < 1e-6); // 3-4-5 triangle
    }

    #[test]
    fn test_dot_product() {
        let computer = DistanceComputer::new(DistanceMetric::DotProduct, 3);
        let a = vec![1.0, 2.0, 3.0];
        let b = vec![4.0, 5.0, 6.0];
        let distance = computer.compute(&a, &b).unwrap();
        assert!((distance - 32.0).abs() < 1e-6); // 1*4 + 2*5 + 3*6 = 32
    }

    #[test]
    fn test_manhattan_distance() {
        let computer = DistanceComputer::new(DistanceMetric::Manhattan, 3);
        let a = vec![1.0, 2.0, 3.0];
        let b = vec![4.0, 6.0, 8.0];
        let distance = computer.compute(&a, &b).unwrap();
        assert!((distance - 12.0).abs() < 1e-6); // |1-4| + |2-6| + |3-8| = 3+4+5 = 12
    }

    #[test]
    fn test_batch_computation() {
        let computer = DistanceComputer::new(DistanceMetric::Euclidean, 3);
        let query = vec![0.0, 0.0, 0.0];
        let candidates = vec![
            vec![1.0, 0.0, 0.0],
            vec![0.0, 1.0, 0.0],
            vec![0.0, 0.0, 1.0],
        ];
        let candidate_refs: Vec<&[f32]> = candidates.iter().map(|v| v.as_slice()).collect();

        let distances = computer.compute_batch(&query, &candidate_refs).unwrap();
        assert_eq!(distances.len(), 3);
        assert!((distances[0] - 1.0).abs() < 1e-6);
        assert!((distances[1] - 1.0).abs() < 1e-6);
        assert!((distances[2] - 1.0).abs() < 1e-6);
    }

    #[test]
    fn test_distance_cache() {
        let mut cache = DistanceCache::new(DistanceMetric::Euclidean, 100);
        let computer = DistanceComputer::new(DistanceMetric::Euclidean, 3);

        let vec_a = vec![1.0, 2.0, 3.0];
        let vec_b = vec![4.0, 5.0, 6.0];

        // First computation should calculate
        let dist1 = cache.get_or_compute(1, 2, &computer, &vec_a, &vec_b).unwrap();

        // Second computation should use cache
        let dist2 = cache.get_or_compute(1, 2, &computer, &vec_a, &vec_b).unwrap();

        assert_eq!(dist1, dist2);
        assert_eq!(cache.size(), 1);
    }

    #[test]
    fn test_metric_properties() {
        let cosine_props = DistanceMetricSelector::get_properties(&DistanceMetric::Cosine);
        assert!(cosine_props.higher_is_similar);
        assert!(cosine_props.normalized);
        assert_eq!(cosine_props.range, (-1.0, 1.0));

        let euclidean_props = DistanceMetricSelector::get_properties(&DistanceMetric::Euclidean);
        assert!(!euclidean_props.higher_is_similar);
        assert!(!euclidean_props.normalized);
        assert_eq!(euclidean_props.range, (0.0, f32::INFINITY));
    }

    #[test]
    fn test_usecase_selection() {
        assert_eq!(DistanceMetricSelector::select_for_usecase(VectorUseCase::SemanticSearch), DistanceMetric::Cosine);
        assert_eq!(DistanceMetricSelector::select_for_usecase(VectorUseCase::Recommendation), DistanceMetric::DotProduct);
        assert_eq!(DistanceMetricSelector::select_for_usecase(VectorUseCase::Clustering), DistanceMetric::Euclidean);
    }

    #[test]
    fn test_dimension_mismatch_error() {
        let computer = DistanceComputer::new(DistanceMetric::Cosine, 3);
        let a = vec![1.0, 2.0];
        let b = vec![1.0, 2.0, 3.0];

        assert!(computer.compute(&a, &b).is_err());
    }
}
