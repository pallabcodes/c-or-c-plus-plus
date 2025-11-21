//! Vector Index: AI/ML Similarity Search and Embedding Optimization

use std::collections::HashMap;
use crate::core::errors::AuroraResult;

#[derive(Debug, Clone)]
pub enum VectorIndexType {
    HNSW,     // Hierarchical Navigable Small World
    IVF,      // Inverted File Index
    PQ,       // Product Quantization
}

#[derive(Debug, Clone)]
pub enum DistanceMetric {
    Cosine,
    Euclidean,
    DotProduct,
    Manhattan,
}

#[derive(Debug, Clone)]
pub struct VectorIndexConfig {
    pub name: String,
    pub column: String,
    pub dimensions: usize,
    pub index_type: VectorIndexType,
    pub distance_metric: DistanceMetric,
}

#[derive(Debug)]
pub struct VectorIndex {
    config: VectorIndexConfig,
    vectors: HashMap<u64, Vec<f32>>, // object_id -> vector
}

impl VectorIndex {
    pub fn new(config: VectorIndexConfig) -> AuroraResult<Self> {
        Ok(Self {
            config,
            vectors: HashMap::new(),
        })
    }

    pub fn insert(&mut self, object_id: u64, vector: Vec<f32>) -> AuroraResult<()> {
        if vector.len() != self.config.dimensions {
            return Err(crate::core::errors::AuroraError::InvalidArgument(
                format!("Vector dimension mismatch: expected {}, got {}",
                       self.config.dimensions, vector.len())
            ));
        }
        self.vectors.insert(object_id, vector);
        Ok(())
    }

    pub fn search(&self, query_vector: &[f32], k: usize) -> AuroraResult<Vec<(u64, f64)>> {
        if query_vector.len() != self.config.dimensions {
            return Err(crate::core::errors::AuroraError::InvalidArgument(
                "Query vector dimension mismatch".to_string()
            ));
        }

        let mut results = Vec::new();

        for (id, vector) in &self.vectors {
            let distance = self.calculate_distance(query_vector, vector);
            results.push((*id, distance));
        }

        // Sort by distance (ascending for similarity)
        results.sort_by(|a, b| a.1.partial_cmp(&b.1).unwrap());
        results.truncate(k);

        Ok(results)
    }

    fn calculate_distance(&self, v1: &[f32], v2: &[f32]) -> f64 {
        match self.config.distance_metric {
            DistanceMetric::Cosine => self.cosine_distance(v1, v2),
            DistanceMetric::Euclidean => self.euclidean_distance(v1, v2),
            DistanceMetric::DotProduct => -self.dot_product(v1, v2), // Negative for ascending sort
            DistanceMetric::Manhattan => self.manhattan_distance(v1, v2),
        }
    }

    fn cosine_distance(&self, v1: &[f32], v2: &[f32]) -> f64 {
        let dot = self.dot_product(v1, v2);
        let norm1 = (v1.iter().map(|x| x * x).sum::<f32>()).sqrt();
        let norm2 = (v2.iter().map(|x| x * x).sum::<f32>()).sqrt();
        1.0 - (dot / (norm1 * norm2)).max(-1.0).min(1.0)
    }

    fn euclidean_distance(&self, v1: &[f32], v2: &[f32]) -> f64 {
        v1.iter().zip(v2.iter())
            .map(|(a, b)| (a - b).powi(2))
            .sum::<f32>()
            .sqrt() as f64
    }

    fn dot_product(&self, v1: &[f32], v2: &[f32]) -> f64 {
        v1.iter().zip(v2.iter())
            .map(|(a, b)| a * b)
            .sum::<f32>() as f64
    }

    fn manhattan_distance(&self, v1: &[f32], v2: &[f32]) -> f64 {
        v1.iter().zip(v2.iter())
            .map(|(a, b)| (a - b).abs())
            .sum::<f32>() as f64
    }
}
