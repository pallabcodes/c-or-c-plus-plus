//! AuroraDB Advanced Vector Filtering: Metadata-Aware Similarity Search
//!
//! Revolutionary pre-filtering for vector search with AuroraDB UNIQUENESS:
//! - Metadata filtering before vector similarity computation
//! - Multi-attribute filtering with complex query support
//! - Filter optimization using selectivity estimation
//! - Hybrid filtering combining vector and metadata constraints

use std::collections::{HashMap, HashSet, BTreeMap};
use crate::core::errors::{AuroraResult, AuroraError};

/// Advanced vector filter that supports metadata-based pre-filtering
pub struct AdvancedVectorFilter {
    /// Metadata index for fast filtering
    metadata_index: MetadataIndex,
    /// Filter optimizer for query planning
    optimizer: FilterOptimizer,
    /// Cached filter results
    filter_cache: FilterCache,
    /// Statistics for optimization
    stats: FilterStatistics,
}

impl AdvancedVectorFilter {
    /// Create a new advanced vector filter
    pub fn new() -> Self {
        Self {
            metadata_index: MetadataIndex::new(),
            optimizer: FilterOptimizer::new(),
            filter_cache: FilterCache::new(),
            stats: FilterStatistics::default(),
        }
    }

    /// Add metadata for a vector
    pub fn add_metadata(&mut self, vector_id: usize, metadata: HashMap<String, MetadataValue>) -> AuroraResult<()> {
        self.metadata_index.add_metadata(vector_id, metadata)
    }

    /// Apply complex filters to get candidate vector IDs
    pub fn apply_filters(&self, filters: &[MetadataFilter]) -> AuroraResult<HashSet<usize>> {
        // Check cache first
        let cache_key = self.compute_cache_key(filters);
        if let Some(cached) = self.filter_cache.get(&cache_key) {
            self.stats.cache_hits += 1;
            return Ok(cached.clone());
        }

        self.stats.cache_misses += 1;
        self.stats.total_queries += 1;

        // Optimize filter order
        let optimized_filters = self.optimizer.optimize_filters(filters)?;

        // Apply filters in optimized order
        let mut candidates = None;

        for filter in optimized_filters {
            let filter_start = std::time::Instant::now();

            let filter_result = self.metadata_index.apply_filter(&filter)?;

            candidates = match candidates {
                None => Some(filter_result),
                Some(current) => Some(current.intersection(&filter_result).cloned().collect()),
            };

            let filter_time = filter_start.elapsed().as_nanos() as f64 / 1_000_000.0; // Convert to milliseconds
            self.stats.total_filter_time_ms += filter_time;

            // Early termination if no candidates left
            if candidates.as_ref().unwrap().is_empty() {
                break;
            }
        }

        let result = candidates.unwrap_or_default();
        self.stats.total_results += result.len();

        // Cache result
        self.filter_cache.put(cache_key, result.clone());

        Ok(result)
    }

    /// Get filter selectivity estimates for query planning
    pub fn estimate_selectivity(&self, filters: &[MetadataFilter]) -> AuroraResult<SelectivityEstimate> {
        let mut combined_selectivity = 1.0;
        let mut estimated_rows = self.metadata_index.total_vectors() as f64;

        for filter in filters {
            let selectivity = self.metadata_index.estimate_filter_selectivity(filter)?;
            combined_selectivity *= selectivity;
            estimated_rows *= selectivity;
        }

        Ok(SelectivityEstimate {
            selectivity: combined_selectivity,
            estimated_rows: estimated_rows as usize,
            confidence: 0.85, // Mock confidence level
        })
    }

    /// Update metadata for a vector (for real-time updates)
    pub fn update_metadata(&mut self, vector_id: usize, metadata: HashMap<String, MetadataValue>) -> AuroraResult<()> {
        self.metadata_index.update_metadata(vector_id, metadata)?;
        self.filter_cache.invalidate(); // Invalidate cache on updates
        Ok(())
    }

    /// Remove metadata for a vector
    pub fn remove_metadata(&mut self, vector_id: usize) -> AuroraResult<()> {
        self.metadata_index.remove_metadata(vector_id)?;
        self.filter_cache.invalidate();
        Ok(())
    }

    /// Get filter statistics
    pub fn get_statistics(&self) -> &FilterStatistics {
        &self.stats
    }

    /// Compute cache key for filters
    fn compute_cache_key(&self, filters: &[MetadataFilter]) -> u64 {
        use std::collections::hash_map::DefaultHasher;
        use std::hash::{Hash, Hasher};

        let mut hasher = DefaultHasher::new();
        for filter in filters {
            filter.hash(&mut hasher);
        }
        hasher.finish()
    }
}

/// Metadata index for fast filtering
pub struct MetadataIndex {
    /// Index by attribute name and value
    attribute_index: HashMap<String, BTreeMap<MetadataValue, HashSet<usize>>>,
    /// Reverse index: vector_id -> metadata
    vector_metadata: HashMap<usize, HashMap<String, MetadataValue>>,
    /// Statistics for each attribute
    attribute_stats: HashMap<String, AttributeStatistics>,
}

impl MetadataIndex {
    fn new() -> Self {
        Self {
            attribute_index: HashMap::new(),
            vector_metadata: HashMap::new(),
            attribute_stats: HashMap::new(),
        }
    }

    fn add_metadata(&mut self, vector_id: usize, metadata: HashMap<String, MetadataValue>) -> AuroraResult<()> {
        // Store reverse mapping
        self.vector_metadata.insert(vector_id, metadata.clone());

        // Update forward index
        for (key, value) in metadata {
            self.attribute_index
                .entry(key.clone())
                .or_insert_with(BTreeMap::new)
                .entry(value.clone())
                .or_insert_with(HashSet::new)
                .insert(vector_id);

            // Update statistics
            self.attribute_stats.entry(key.clone())
                .or_insert_with(AttributeStatistics::new)
                .update(&value);
        }

        Ok(())
    }

    fn update_metadata(&mut self, vector_id: usize, new_metadata: HashMap<String, MetadataValue>) -> AuroraResult<()> {
        // Remove old metadata
        if let Some(old_metadata) = self.vector_metadata.remove(&vector_id) {
            for (key, value) in old_metadata {
                if let Some(attr_index) = self.attribute_index.get_mut(&key) {
                    if let Some(value_set) = attr_index.get_mut(&value) {
                        value_set.remove(&vector_id);
                        if value_set.is_empty() {
                            attr_index.remove(&value);
                        }
                    }
                }
            }
        }

        // Add new metadata
        self.add_metadata(vector_id, new_metadata)
    }

    fn remove_metadata(&mut self, vector_id: usize) -> AuroraResult<()> {
        if let Some(metadata) = self.vector_metadata.remove(&vector_id) {
            for (key, value) in metadata {
                if let Some(attr_index) = self.attribute_index.get_mut(&key) {
                    if let Some(value_set) = attr_index.get_mut(&value) {
                        value_set.remove(&vector_id);
                        if value_set.is_empty() {
                            attr_index.remove(&value);
                        }
                    }
                }
            }
        }

        Ok(())
    }

    fn apply_filter(&self, filter: &MetadataFilter) -> AuroraResult<HashSet<usize>> {
        match filter {
            MetadataFilter::Equal { attribute, value } => {
                Ok(self.attribute_index
                    .get(attribute)
                    .and_then(|attr_index| attr_index.get(value))
                    .cloned()
                    .unwrap_or_default())
            }
            MetadataFilter::NotEqual { attribute, value } => {
                let all_vectors: HashSet<usize> = self.vector_metadata.keys().cloned().collect();
                let matching = self.attribute_index
                    .get(attribute)
                    .and_then(|attr_index| attr_index.get(value))
                    .cloned()
                    .unwrap_or_default();

                Ok(all_vectors.difference(&matching).cloned().collect())
            }
            MetadataFilter::In { attribute, values } => {
                let mut result = HashSet::new();
                if let Some(attr_index) = self.attribute_index.get(attribute) {
                    for value in values {
                        if let Some(vectors) = attr_index.get(value) {
                            result.extend(vectors);
                        }
                    }
                }
                Ok(result)
            }
            MetadataFilter::Range { attribute, min, max } => {
                let mut result = HashSet::new();
                if let Some(attr_index) = self.attribute_index.get(attribute) {
                    for (value, vectors) in attr_index.range(min.clone()..=max.clone()) {
                        result.extend(vectors);
                    }
                }
                Ok(result)
            }
            MetadataFilter::Contains { attribute, substring } => {
                let mut result = HashSet::new();
                if let Some(attr_index) = self.attribute_index.get(attribute) {
                    for (value, vectors) in attr_index.iter() {
                        if let MetadataValue::String(s) = value {
                            if s.contains(substring) {
                                result.extend(vectors);
                            }
                        }
                    }
                }
                Ok(result)
            }
            MetadataFilter::Regex { attribute, pattern } => {
                let mut result = HashSet::new();
                if let Some(attr_index) = self.attribute_index.get(attribute) {
                    let regex = regex::Regex::new(pattern)?;
                    for (value, vectors) in attr_index.iter() {
                        if let MetadataValue::String(s) = value {
                            if regex.is_match(s) {
                                result.extend(vectors);
                            }
                        }
                    }
                }
                Ok(result)
            }
        }
    }

    fn estimate_filter_selectivity(&self, filter: &MetadataFilter) -> AuroraResult<f64> {
        let total_vectors = self.total_vectors() as f64;
        if total_vectors == 0.0 {
            return Ok(1.0);
        }

        match filter {
            MetadataFilter::Equal { attribute, value } => {
                if let Some(attr_index) = self.attribute_index.get(attribute) {
                    if let Some(vectors) = attr_index.get(value) {
                        Ok(vectors.len() as f64 / total_vectors)
                    } else {
                        Ok(0.0) // No vectors match
                    }
                } else {
                    Ok(1.0) // Unknown attribute, assume no filtering
                }
            }
            MetadataFilter::Range { attribute, min, max } => {
                if let Some(stats) = self.attribute_stats.get(attribute) {
                    stats.estimate_range_selectivity(min, max)
                } else {
                    Ok(0.5) // Default estimate
                }
            }
            _ => Ok(0.1), // Conservative estimate for complex filters
        }
    }

    fn total_vectors(&self) -> usize {
        self.vector_metadata.len()
    }
}

/// Filter optimizer for query planning
pub struct FilterOptimizer;

impl FilterOptimizer {
    fn new() -> Self {
        Self
    }

    fn optimize_filters(&self, filters: &[MetadataFilter]) -> AuroraResult<Vec<MetadataFilter>> {
        let mut optimized = filters.to_vec();

        // Sort by estimated selectivity (most selective first)
        // In a real implementation, this would use cost-based optimization
        optimized.sort_by(|a, b| {
            // Simple heuristic: prefer equality filters over range filters
            match (a, b) {
                (MetadataFilter::Equal { .. }, MetadataFilter::Range { .. }) => std::cmp::Ordering::Less,
                (MetadataFilter::Range { .. }, MetadataFilter::Equal { .. }) => std::cmp::Ordering::Greater,
                _ => std::cmp::Ordering::Equal,
            }
        });

        Ok(optimized)
    }
}

/// Filter cache for performance
pub struct FilterCache {
    cache: HashMap<u64, HashSet<usize>>,
    max_size: usize,
}

impl FilterCache {
    fn new() -> Self {
        Self {
            cache: HashMap::new(),
            max_size: 1000,
        }
    }

    fn get(&self, key: &u64) -> Option<&HashSet<usize>> {
        self.cache.get(key)
    }

    fn put(&mut self, key: u64, value: HashSet<usize>) {
        if self.cache.len() >= self.max_size {
            // Simple eviction: remove a random entry
            if let Some(&first_key) = self.cache.keys().next() {
                self.cache.remove(&first_key);
            }
        }
        self.cache.insert(key, value);
    }

    fn invalidate(&mut self) {
        self.cache.clear();
    }
}

/// Metadata values that can be indexed
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum MetadataValue {
    String(String),
    Integer(i64),
    Float(f64),
    Boolean(bool),
}

impl From<String> for MetadataValue {
    fn from(s: String) -> Self {
        MetadataValue::String(s)
    }
}

impl From<i64> for MetadataValue {
    fn from(i: i64) -> Self {
        MetadataValue::Integer(i)
    }
}

impl From<f64> for MetadataValue {
    fn from(f: f64) -> Self {
        MetadataValue::Float(f)
    }
}

impl From<bool> for MetadataValue {
    fn from(b: bool) -> Self {
        MetadataValue::Boolean(b)
    }
}

/// Metadata filters for advanced querying
#[derive(Debug, Clone)]
pub enum MetadataFilter {
    Equal { attribute: String, value: MetadataValue },
    NotEqual { attribute: String, value: MetadataValue },
    In { attribute: String, values: Vec<MetadataValue> },
    Range { attribute: String, min: MetadataValue, max: MetadataValue },
    Contains { attribute: String, substring: String },
    Regex { attribute: String, pattern: String },
}

impl std::hash::Hash for MetadataFilter {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        match self {
            MetadataFilter::Equal { attribute, value } => {
                0.hash(state);
                attribute.hash(state);
                value.hash(state);
            }
            MetadataFilter::NotEqual { attribute, value } => {
                1.hash(state);
                attribute.hash(state);
                value.hash(state);
            }
            MetadataFilter::In { attribute, values } => {
                2.hash(state);
                attribute.hash(state);
                values.hash(state);
            }
            MetadataFilter::Range { attribute, min, max } => {
                3.hash(state);
                attribute.hash(state);
                min.hash(state);
                max.hash(state);
            }
            MetadataFilter::Contains { attribute, substring } => {
                4.hash(state);
                attribute.hash(state);
                substring.hash(state);
            }
            MetadataFilter::Regex { attribute, pattern } => {
                5.hash(state);
                attribute.hash(state);
                pattern.hash(state);
            }
        }
    }
}

/// Selectivity estimate for query planning
#[derive(Debug, Clone)]
pub struct SelectivityEstimate {
    pub selectivity: f64,
    pub estimated_rows: usize,
    pub confidence: f64,
}

/// Filter statistics for monitoring
#[derive(Debug, Clone, Default)]
pub struct FilterStatistics {
    pub total_queries: u64,
    pub cache_hits: u64,
    pub cache_misses: u64,
    pub total_results: usize,
    pub total_filter_time_ms: f64,
}

impl FilterStatistics {
    pub fn cache_hit_rate(&self) -> f64 {
        if self.total_queries == 0 {
            0.0
        } else {
            self.cache_hits as f64 / self.total_queries as f64
        }
    }

    pub fn avg_filter_time_ms(&self) -> f64 {
        if self.total_queries == 0 {
            0.0
        } else {
            self.total_filter_time_ms / self.total_queries as f64
        }
    }
}

/// Attribute statistics for selectivity estimation
#[derive(Debug, Clone)]
struct AttributeStatistics {
    value_counts: HashMap<MetadataValue, usize>,
    total_values: usize,
    min_value: Option<MetadataValue>,
    max_value: Option<MetadataValue>,
}

impl AttributeStatistics {
    fn new() -> Self {
        Self {
            value_counts: HashMap::new(),
            total_values: 0,
            min_value: None,
            max_value: None,
        }
    }

    fn update(&mut self, value: &MetadataValue) {
        *self.value_counts.entry(value.clone()).or_insert(0) += 1;
        self.total_values += 1;

        // Update min/max for range queries
        self.min_value = match (&self.min_value, value) {
            (None, _) => Some(value.clone()),
            (Some(current), new) if new < current => Some(new.clone()),
            _ => self.min_value.clone(),
        };

        self.max_value = match (&self.max_value, value) {
            (None, _) => Some(value.clone()),
            (Some(current), new) if new > current => Some(new.clone()),
            _ => self.max_value.clone(),
        };
    }

    fn estimate_range_selectivity(&self, min: &MetadataValue, max: &MetadataValue) -> AuroraResult<f64> {
        if self.total_values == 0 {
            return Ok(1.0);
        }

        // Simple estimation based on value distribution
        // In a real implementation, this would use histograms
        let total_range = match (&self.min_value, &self.max_value) {
            (Some(min_val), Some(max_val)) => {
                // Rough estimate: assume uniform distribution
                0.5 // 50% selectivity for range queries
            }
            _ => 1.0,
        };

        Ok(total_range.min(1.0))
    }
}

/// Hybrid vector search combining filtering and similarity
pub struct HybridVectorSearch {
    vector_index: Box<dyn super::super::vector_index::VectorIndex>,
    metadata_filter: AdvancedVectorFilter,
}

impl HybridVectorSearch {
    pub fn new(
        vector_index: Box<dyn super::super::vector_index::VectorIndex>,
        metadata_filter: AdvancedVectorFilter,
    ) -> Self {
        Self {
            vector_index,
            metadata_filter,
        }
    }

    /// Perform hybrid search: filter first, then vector search
    pub fn hybrid_search(
        &self,
        query_vector: &[f32],
        filters: &[MetadataFilter],
        k: usize,
    ) -> AuroraResult<Vec<(usize, f32)>> {
        // Apply metadata filters first
        let candidate_ids = self.metadata_filter.apply_filters(filters)?;

        if candidate_ids.is_empty() {
            return Ok(Vec::new());
        }

        // Perform vector search on filtered candidates
        // In a real implementation, this would need to be modified to search only within candidates
        // For now, we do a regular search and filter results
        let all_results = self.vector_index.search(query_vector, k * 2)?; // Get more results

        // Filter to only include candidates
        let filtered_results: Vec<(usize, f32)> = all_results.into_iter()
            .filter(|(id, _)| candidate_ids.contains(id))
            .take(k)
            .collect();

        Ok(filtered_results)
    }

    /// Estimate cost of hybrid search
    pub fn estimate_cost(&self, filters: &[MetadataFilter], k: usize) -> AuroraResult<SearchCost> {
        let selectivity = self.metadata_filter.estimate_selectivity(filters)?;
        let filter_cost = selectivity.estimated_rows as f64 * 0.001; // Cost per filtered item
        let vector_cost = selectivity.estimated_rows as f64 * 0.01; // Cost per vector search

        Ok(SearchCost {
            filter_cost,
            vector_cost,
            total_cost: filter_cost + vector_cost,
            estimated_candidates: selectivity.estimated_rows,
            estimated_results: k.min(selectivity.estimated_rows),
        })
    }
}

/// Search cost estimation
#[derive(Debug, Clone)]
pub struct SearchCost {
    pub filter_cost: f64,
    pub vector_cost: f64,
    pub total_cost: f64,
    pub estimated_candidates: usize,
    pub estimated_results: usize,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_metadata_indexing() {
        let mut filter = AdvancedVectorFilter::new();

        // Add metadata for vectors
        let mut metadata1 = HashMap::new();
        metadata1.insert("category".to_string(), MetadataValue::String("electronics".to_string()));
        metadata1.insert("price".to_string(), MetadataValue::Float(99.99));
        metadata1.insert("in_stock".to_string(), MetadataValue::Boolean(true));

        filter.add_metadata(1, metadata1).unwrap();

        let mut metadata2 = HashMap::new();
        metadata2.insert("category".to_string(), MetadataValue::String("books".to_string()));
        metadata2.insert("price".to_string(), MetadataValue::Float(29.99));
        metadata2.insert("in_stock".to_string(), MetadataValue::Boolean(false));

        filter.add_metadata(2, metadata2).unwrap();

        // Test equality filter
        let filters = vec![
            MetadataFilter::Equal {
                attribute: "category".to_string(),
                value: MetadataValue::String("electronics".to_string()),
            }
        ];

        let results = filter.apply_filters(&filters).unwrap();
        assert_eq!(results.len(), 1);
        assert!(results.contains(&1));

        // Test range filter
        let range_filters = vec![
            MetadataFilter::Range {
                attribute: "price".to_string(),
                min: MetadataValue::Float(50.0),
                max: MetadataValue::Float(150.0),
            }
        ];

        let range_results = filter.apply_filters(&range_filters).unwrap();
        assert_eq!(range_results.len(), 1);
        assert!(range_results.contains(&1));
    }

    #[test]
    fn test_filter_caching() {
        let mut filter = AdvancedVectorFilter::new();

        // Add some metadata
        let mut metadata = HashMap::new();
        metadata.insert("status".to_string(), MetadataValue::String("active".to_string()));
        filter.add_metadata(1, metadata).unwrap();

        // Apply filter twice
        let test_filter = MetadataFilter::Equal {
            attribute: "status".to_string(),
            value: MetadataValue::String("active".to_string()),
        };

        let filters = vec![test_filter.clone()];

        let results1 = filter.apply_filters(&filters).unwrap();
        let stats1 = filter.get_statistics().clone();

        let results2 = filter.apply_filters(&filters).unwrap();
        let stats2 = filter.get_statistics().clone();

        // Results should be the same
        assert_eq!(results1, results2);

        // Second query should be a cache hit
        assert_eq!(stats2.cache_hits, stats1.cache_hits + 1);
    }

    #[test]
    fn test_selectivity_estimation() {
        let mut filter = AdvancedVectorFilter::new();

        // Add metadata
        for i in 0..100 {
            let mut metadata = HashMap::new();
            let category = if i < 50 { "electronics" } else { "books" };
            metadata.insert("category".to_string(), MetadataValue::String(category.to_string()));
            filter.add_metadata(i, metadata).unwrap();
        }

        // Test selectivity estimation
        let test_filter = MetadataFilter::Equal {
            attribute: "category".to_string(),
            value: MetadataValue::String("electronics".to_string()),
        };

        let estimate = filter.estimate_selectivity(&[test_filter]).unwrap();

        // Should estimate ~50% selectivity (50 electronics out of 100 items)
        assert!(estimate.selectivity > 0.4 && estimate.selectivity < 0.6);
        assert_eq!(estimate.estimated_rows, 50);
    }

    #[test]
    fn test_complex_filters() {
        let mut filter = AdvancedVectorFilter::new();

        // Add test data
        for i in 0..10 {
            let mut metadata = HashMap::new();
            metadata.insert("price".to_string(), MetadataValue::Float(i as f64 * 10.0));
            metadata.insert("category".to_string(), MetadataValue::String(
                if i < 5 { "cheap" } else { "expensive" }.to_string()
            ));
            metadata.insert("rating".to_string(), MetadataValue::Float(i as f64));
            filter.add_metadata(i, metadata).unwrap();
        }

        // Test complex filter combination
        let filters = vec![
            MetadataFilter::Range {
                attribute: "price".to_string(),
                min: MetadataValue::Float(20.0),
                max: MetadataValue::Float(70.0),
            },
            MetadataFilter::Equal {
                attribute: "category".to_string(),
                value: MetadataValue::String("expensive".to_string()),
            }
        ];

        let results = filter.apply_filters(&filters).unwrap();

        // Should match items 5, 6, 7 (price 50-70, expensive category)
        assert_eq!(results.len(), 3);
        assert!(results.contains(&5));
        assert!(results.contains(&6));
        assert!(results.contains(&7));
    }

    #[test]
    fn test_metadata_updates() {
        let mut filter = AdvancedVectorFilter::new();

        // Add initial metadata
        let mut metadata = HashMap::new();
        metadata.insert("status".to_string(), MetadataValue::String("active".to_string()));
        filter.add_metadata(1, metadata).unwrap();

        // Update metadata
        let mut new_metadata = HashMap::new();
        new_metadata.insert("status".to_string(), MetadataValue::String("inactive".to_string()));
        new_metadata.insert("priority".to_string(), MetadataValue::Integer(5));
        filter.update_metadata(1, new_metadata).unwrap();

        // Test updated filter
        let filters = vec![MetadataFilter::Equal {
            attribute: "status".to_string(),
            value: MetadataValue::String("inactive".to_string()),
        }];

        let results = filter.apply_filters(&filters).unwrap();
        assert_eq!(results.len(), 1);
        assert!(results.contains(&1));

        // Test new attribute filter
        let priority_filters = vec![MetadataFilter::Equal {
            attribute: "priority".to_string(),
            value: MetadataValue::Integer(5),
        }];

        let priority_results = filter.apply_filters(&priority_filters).unwrap();
        assert_eq!(priority_results.len(), 1);
        assert!(priority_results.contains(&1));
    }

    #[test]
    fn test_regex_filtering() {
        let mut filter = AdvancedVectorFilter::new();

        // Add test data with text fields
        let test_data = vec![
            ("product_123", "electronics"),
            ("product_456", "books"),
            ("product_789", "electronics"),
        ];

        for (i, (name, category)) in test_data.iter().enumerate() {
            let mut metadata = HashMap::new();
            metadata.insert("name".to_string(), MetadataValue::String(name.to_string()));
            metadata.insert("category".to_string(), MetadataValue::String(category.to_string()));
            filter.add_metadata(i, metadata).unwrap();
        }

        // Test regex filter
        let regex_filter = MetadataFilter::Regex {
            attribute: "name".to_string(),
            pattern: r"product_\d+".to_string(),
        };

        let results = filter.apply_filters(&[regex_filter]).unwrap();
        assert_eq!(results.len(), 3); // All products match

        // Test more specific regex
        let specific_regex = MetadataFilter::Regex {
            attribute: "name".to_string(),
            pattern: r"product_1\d+.".to_string(),
        };

        let specific_results = filter.apply_filters(&[specific_regex]).unwrap();
        assert_eq!(specific_results.len(), 1); // Only product_123 matches
    }

    #[test]
    fn test_filter_statistics() {
        let filter = AdvancedVectorFilter::new();
        let stats = filter.get_statistics();

        // Initial stats should be zero
        assert_eq!(stats.total_queries, 0);
        assert_eq!(stats.cache_hits, 0);
        assert_eq!(stats.cache_misses, 0);
        assert_eq!(stats.total_results, 0);
    }

    #[test]
    fn test_hybrid_search_cost_estimation() {
        let mut metadata_filter = AdvancedVectorFilter::new();

        // Add some test metadata
        for i in 0..100 {
            let mut metadata = HashMap::new();
            metadata.insert("category".to_string(), MetadataValue::String("test".to_string()));
            metadata_filter.add_metadata(i, metadata).unwrap();
        }

        // Create mock vector index for testing
        // In real implementation, this would be a proper vector index
        let mock_index = Box::new(MockVectorIndex::new());

        let hybrid_search = HybridVectorSearch::new(mock_index, metadata_filter);

        // Test cost estimation
        let filters = vec![MetadataFilter::Equal {
            attribute: "category".to_string(),
            value: MetadataValue::String("test".to_string()),
        }];

        let cost = hybrid_search.estimate_cost(&filters, 10).unwrap();

        // Should estimate costs
        assert!(cost.total_cost > 0.0);
        assert_eq!(cost.estimated_candidates, 100); // All items match
        assert_eq!(cost.estimated_results, 10); // Limited by k
    }

    // Mock vector index for testing
    struct MockVectorIndex;

    impl MockVectorIndex {
        fn new() -> Self {
            Self
        }
    }

    impl super::super::vector_index::VectorIndex for MockVectorIndex {
        fn insert(&mut self, _id: usize, _vector: Vec<f32>) -> AuroraResult<()> {
            Ok(())
        }

        fn search(&self, _query: &[f32], _k: usize) -> AuroraResult<Vec<(usize, f32)>> {
            Ok(vec![(1, 0.9), (2, 0.8), (3, 0.7)])
        }

        fn delete(&mut self, _id: usize) -> AuroraResult<()> {
            Ok(())
        }

        fn stats(&self) -> super::super::vector_index::IndexStats {
            super::super::vector_index::IndexStats {
                index_type: "mock".to_string(),
                dimension: 384,
                metric: super::super::distance_metrics::DistanceMetric::Cosine,
                total_vectors: 100,
                memory_usage_mb: 50.0,
                build_time_ms: 1000.0,
                avg_query_time_ms: 5.0,
                index_specific_stats: HashMap::new(),
            }
        }

        fn optimize(&mut self) -> AuroraResult<()> {
            Ok(())
        }

        fn build(&mut self, _vectors: HashMap<usize, Vec<f32>>) -> AuroraResult<()> {
            Ok(())
        }
    }
}
