//! AuroraDB Time Series Indexing: Temporal Data Indexing and Query Optimization
//!
//! Advanced indexing for time series data with AuroraDB UNIQUENESS:
//! - Time-based indexing with hierarchical temporal structures
//! - Automatic downsampling for different time resolutions
//! - Predictive indexing based on query patterns
//! - SIMD-accelerated temporal range queries

use std::collections::{BTreeMap, HashMap, HashSet, VecDeque};
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};

/// Time series index for efficient temporal queries
#[derive(Debug)]
pub struct TimeSeriesIndex {
    /// Series ID to time index mapping
    series_indexes: RwLock<HashMap<u64, SeriesTimeIndex>>,
    /// Global time range index for cross-series queries
    global_time_index: RwLock<GlobalTimeIndex>,
    /// Downsampling indexes for different resolutions
    downsampled_indexes: RwLock<HashMap<TimeResolution, HashMap<u64, SeriesTimeIndex>>>,
    /// Query pattern learning
    query_analyzer: QueryPatternAnalyzer,
}

impl TimeSeriesIndex {
    /// Create a new time series index
    pub fn new() -> Self {
        Self {
            series_indexes: RwLock::new(HashMap::new()),
            global_time_index: RwLock::new(GlobalTimeIndex::new()),
            downsampled_indexes: RwLock::new(HashMap::new()),
            query_analyzer: QueryPatternAnalyzer::new(),
        }
    }

    /// Index a time series data point
    pub fn index_datapoint(&self, series_id: u64, timestamp: i64, value: f64) -> AuroraResult<()> {
        // Update series-specific index
        let mut series_indexes = self.series_indexes.write();
        let series_index = series_indexes.entry(series_id)
            .or_insert_with(|| SeriesTimeIndex::new(series_id));

        series_index.add_datapoint(timestamp, value)?;

        // Update global time index
        let mut global_index = self.global_time_index.write();
        global_index.add_datapoint(series_id, timestamp)?;

        Ok(())
    }

    /// Query time range for a specific series
    pub fn query_series_range(&self, series_id: u64, start_time: i64, end_time: i64) -> AuroraResult<Vec<(i64, f64)>> {
        let series_indexes = self.series_indexes.read();

        if let Some(series_index) = series_indexes.get(&series_id) {
            series_index.query_range(start_time, end_time)
        } else {
            Ok(Vec::new())
        }
    }

    /// Query multiple series in a time range
    pub fn query_multiple_series(&self, series_ids: &[u64], start_time: i64, end_time: i64) -> AuroraResult<HashMap<u64, Vec<(i64, f64)>>> {
        let mut results = HashMap::new();
        let series_indexes = self.series_indexes.read();

        for &series_id in series_ids {
            if let Some(series_index) = series_indexes.get(&series_id) {
                let data = series_index.query_range(start_time, end_time)?;
                if !data.is_empty() {
                    results.insert(series_id, data);
                }
            }
        }

        Ok(results)
    }

    /// Query using downsampled data for performance
    pub fn query_downsampled(&self, series_id: u64, resolution: TimeResolution, start_time: i64, end_time: i64) -> AuroraResult<Vec<(i64, f64)>> {
        let downsampled_indexes = self.downsampled_indexes.read();

        if let Some(resolution_indexes) = downsampled_indexes.get(&resolution) {
            if let Some(series_index) = resolution_indexes.get(&series_id) {
                return series_index.query_range(start_time, end_time);
            }
        }

        // Fallback to regular index if downsampled not available
        self.query_series_range(series_id, start_time, end_time)
    }

    /// Get series that have data in a time range
    pub fn get_series_in_timerange(&self, start_time: i64, end_time: i64) -> Vec<u64> {
        let global_index = self.global_time_index.read();
        global_index.get_series_in_range(start_time, end_time)
    }

    /// Optimize index based on query patterns
    pub fn optimize_for_queries(&mut self) -> AuroraResult<()> {
        let patterns = self.query_analyzer.analyze_patterns();

        // Create downsampled indexes for frequently queried resolutions
        for resolution in patterns.frequent_resolutions {
            self.create_downsampled_index(resolution)?;
        }

        Ok(())
    }

    /// Create downsampled index for a resolution
    fn create_downsampled_index(&self, resolution: TimeResolution) -> AuroraResult<()> {
        let mut downsampled_indexes = self.downsampled_indexes.write();
        let resolution_indexes = downsampled_indexes.entry(resolution)
            .or_insert_with(HashMap::new);

        let series_indexes = self.series_indexes.read();

        for (&series_id, series_index) in series_indexes.iter() {
            let downsampled_data = series_index.downsample(resolution)?;
            let mut downsampled_index = SeriesTimeIndex::new(series_id);

            for (timestamp, value) in downsampled_data {
                downsampled_index.add_datapoint(timestamp, value)?;
            }

            resolution_indexes.insert(series_id, downsampled_index);
        }

        Ok(())
    }

    /// Record query for pattern analysis
    pub fn record_query(&self, query: &TimeSeriesQuery) {
        self.query_analyzer.record_query(query);
    }
}

/// Time series query structure
#[derive(Debug, Clone)]
pub struct TimeSeriesQuery {
    pub series_ids: Vec<u64>,
    pub start_time: i64,
    pub end_time: i64,
    pub resolution: Option<TimeResolution>,
    pub aggregation: Option<AggregationFunction>,
}

/// Series-specific time index
#[derive(Debug)]
pub struct SeriesTimeIndex {
    pub series_id: u64,
    /// Time-ordered data points (timestamp -> value)
    time_data: BTreeMap<i64, f64>,
    /// Statistics for optimization
    stats: IndexStats,
}

impl SeriesTimeIndex {
    fn new(series_id: u64) -> Self {
        Self {
            series_id,
            time_data: BTreeMap::new(),
            stats: IndexStats::default(),
        }
    }

    fn add_datapoint(&mut self, timestamp: i64, value: f64) -> AuroraResult<()> {
        self.time_data.insert(timestamp, value);
        self.stats.update(timestamp, value);
        Ok(())
    }

    fn query_range(&self, start_time: i64, end_time: i64) -> AuroraResult<Vec<(i64, f64)>> {
        let mut results = Vec::new();

        // Use BTreeMap range query for efficiency
        for (&timestamp, &value) in self.time_data.range(start_time..=end_time) {
            results.push((timestamp, value));
        }

        Ok(results)
    }

    fn downsample(&self, resolution: TimeResolution) -> AuroraResult<Vec<(i64, f64)>> {
        if self.time_data.is_empty() {
            return Ok(Vec::new());
        }

        let interval_ms = resolution.interval_ms();
        let mut downsampled = Vec::new();
        let mut current_bucket_start = None;
        let mut bucket_values = Vec::new();

        for (&timestamp, &value) in &self.time_data {
            let bucket_start = (timestamp / interval_ms) * interval_ms;

            if current_bucket_start != Some(bucket_start) {
                // Finish previous bucket
                if let Some(start) = current_bucket_start {
                    if !bucket_values.is_empty() {
                        let avg_value = bucket_values.iter().sum::<f64>() / bucket_values.len() as f64;
                        downsampled.push((start, avg_value));
                    }
                }

                // Start new bucket
                current_bucket_start = Some(bucket_start);
                bucket_values = Vec::new();
            }

            bucket_values.push(value);
        }

        // Finish last bucket
        if let Some(start) = current_bucket_start {
            if !bucket_values.is_empty() {
                let avg_value = bucket_values.iter().sum::<f64>() / bucket_values.len() as f64;
                downsampled.push((start, avg_value));
            }
        }

        Ok(downsampled)
    }
}

/// Global time index for cross-series queries
#[derive(Debug)]
pub struct GlobalTimeIndex {
    /// Series to time range mapping
    series_ranges: HashMap<u64, (i64, i64)>,
    /// Time to series mapping (inverted index)
    time_to_series: BTreeMap<i64, HashSet<u64>>,
}

impl GlobalTimeIndex {
    fn new() -> Self {
        Self {
            series_ranges: HashMap::new(),
            time_to_series: BTreeMap::new(),
        }
    }

    fn add_datapoint(&mut self, series_id: u64, timestamp: i64) -> AuroraResult<()> {
        // Update series range
        let range = self.series_ranges.entry(series_id)
            .or_insert((timestamp, timestamp));

        range.0 = range.0.min(timestamp);
        range.1 = range.1.max(timestamp);

        // Update time to series mapping
        self.time_to_series.entry(timestamp)
            .or_insert_with(HashSet::new)
            .insert(series_id);

        Ok(())
    }

    fn get_series_in_range(&self, start_time: i64, end_time: i64) -> Vec<u64> {
        let mut result_series = HashSet::new();

        // Check series ranges
        for (&series_id, &(series_start, series_end)) in &self.series_ranges {
            if series_start <= end_time && series_end >= start_time {
                result_series.insert(series_id);
            }
        }

        result_series.into_iter().collect()
    }
}

/// Time resolutions for downsampling
#[derive(Debug, Clone, Copy, Hash, Eq, PartialEq)]
pub enum TimeResolution {
    Raw,
    Second,
    Minute,
    Hour,
    Day,
    Week,
    Month,
}

impl TimeResolution {
    fn interval_ms(&self) -> i64 {
        match self {
            TimeResolution::Raw => 1,
            TimeResolution::Second => 1000,
            TimeResolution::Minute => 60000,
            TimeResolution::Hour => 3600000,
            TimeResolution::Day => 86400000,
            TimeResolution::Week => 604800000,
            TimeResolution::Month => 2592000000, // 30 days
        }
    }
}

/// Aggregation functions for downsampling
#[derive(Debug, Clone)]
pub enum AggregationFunction {
    Average,
    Sum,
    Min,
    Max,
    Count,
    Percentile(f64),
}

/// Index statistics for optimization
#[derive(Debug, Clone, Default)]
pub struct IndexStats {
    pub total_points: usize,
    pub time_span_ms: i64,
    pub avg_value: f64,
    pub value_variance: f64,
    pub last_update: i64,
}

impl IndexStats {
    fn update(&mut self, timestamp: i64, value: f64) {
        self.total_points += 1;
        self.last_update = timestamp;

        // Update running average
        let delta = value - self.avg_value;
        self.avg_value += delta / self.total_points as f64;

        // Update variance (Welford's online algorithm)
        if self.total_points > 1 {
            let delta2 = value - self.avg_value;
            self.value_variance += delta * delta2;
        }
    }
}

/// Query pattern analyzer for optimization
#[derive(Debug)]
pub struct QueryPatternAnalyzer {
    recorded_queries: RwLock<VecDeque<TimeSeriesQuery>>,
    max_queries: usize,
}

impl QueryPatternAnalyzer {
    fn new() -> Self {
        Self {
            recorded_queries: RwLock::new(VecDeque::new()),
            max_queries: 1000,
        }
    }

    fn record_query(&self, query: &TimeSeriesQuery) {
        let mut queries = self.recorded_queries.write();

        queries.push_back(query.clone());
        if queries.len() > self.max_queries {
            queries.pop_front();
        }
    }

    fn analyze_patterns(&self) -> QueryPatterns {
        let queries = self.recorded_queries.read();
        let mut resolution_counts = HashMap::new();
        let mut time_range_lengths = Vec::new();

        for query in queries.iter() {
            if let Some(resolution) = query.resolution {
                *resolution_counts.entry(resolution).or_insert(0) += 1;
            }

            let time_range = query.end_time - query.start_time;
            time_range_lengths.push(time_range);
        }

        // Find most frequent resolutions
        let mut frequent_resolutions = resolution_counts.into_iter()
            .collect::<Vec<_>>();
        frequent_resolutions.sort_by(|a, b| b.1.cmp(&a.1));
        let frequent_resolutions = frequent_resolutions.into_iter()
            .take(3)
            .map(|(res, _)| res)
            .collect();

        // Calculate average time range
        let avg_time_range = if !time_range_lengths.is_empty() {
            time_range_lengths.iter().sum::<i64>() as f64 / time_range_lengths.len() as f64
        } else {
            0.0
        };

        QueryPatterns {
            frequent_resolutions,
            avg_time_range_ms: avg_time_range,
            total_queries: queries.len(),
        }
    }
}

/// Analyzed query patterns
#[derive(Debug)]
pub struct QueryPatterns {
    pub frequent_resolutions: Vec<TimeResolution>,
    pub avg_time_range_ms: f64,
    pub total_queries: usize,
}

/// Hierarchical time index for very large datasets
pub struct HierarchicalTimeIndex {
    /// Top-level index (coarse granularity)
    top_level: BTreeMap<i64, Vec<u64>>, // time_bucket -> series_ids
    /// Mid-level indexes
    mid_level: HashMap<i64, BTreeMap<i64, Vec<u64>>>, // top_bucket -> mid_bucket -> series_ids
    /// Bottom-level indexes (fine granularity)
    bottom_level: HashMap<i64, SeriesTimeIndex>, // series_id -> detailed index
    /// Hierarchy levels
    levels: usize,
}

impl HierarchicalTimeIndex {
    /// Create a hierarchical time index
    pub fn new(levels: usize) -> Self {
        Self {
            top_level: BTreeMap::new(),
            mid_level: HashMap::new(),
            bottom_level: HashMap::new(),
            levels,
        }
    }

    /// Add data point to hierarchical index
    pub fn add_datapoint(&mut self, series_id: u64, timestamp: i64, value: f64) -> AuroraResult<()> {
        // Calculate bucket keys for each level
        let top_bucket = timestamp / 86400000; // Day-level buckets
        let mid_bucket = timestamp / 3600000;  // Hour-level buckets

        // Update top level
        self.top_level.entry(top_bucket)
            .or_insert_with(Vec::new)
            .push(series_id);

        // Update mid level
        self.mid_level.entry(top_bucket)
            .or_insert_with(BTreeMap::new)
            .entry(mid_bucket)
            .or_insert_with(Vec::new)
            .push(series_id);

        // Update bottom level (detailed index)
        let series_index = self.bottom_level.entry(series_id)
            .or_insert_with(|| SeriesTimeIndex::new(series_id));
        series_index.add_datapoint(timestamp, value)?;

        // Remove duplicates and sort
        if let Some(series) = self.top_level.get_mut(&top_bucket) {
            series.sort();
            series.dedup();
        }

        if let Some(mid_map) = self.mid_level.get_mut(&top_bucket) {
            if let Some(series) = mid_map.get_mut(&mid_bucket) {
                series.sort();
                series.dedup();
            }
        }

        Ok(())
    }

    /// Query hierarchical index
    pub fn query_range(&self, start_time: i64, end_time: i64) -> AuroraResult<HashMap<u64, Vec<(i64, f64)>>> {
        let mut results = HashMap::new();

        // Calculate bucket ranges
        let start_top = start_time / 86400000;
        let end_top = end_time / 86400000;
        let start_mid = start_time / 3600000;
        let end_mid = end_time / 3600000;

        // Collect candidate series from hierarchical traversal
        let mut candidate_series = HashSet::new();

        for (&top_bucket, series_ids) in self.top_level.range(start_top..=end_top) {
            if let Some(mid_map) = self.mid_level.get(&top_bucket) {
                for (&mid_bucket, mid_series_ids) in mid_map.range(start_mid..=end_mid) {
                    candidate_series.extend(mid_series_ids);
                }
            }
            candidate_series.extend(series_ids);
        }

        // Query detailed data for candidates
        for series_id in candidate_series {
            if let Some(series_index) = self.bottom_level.get(&series_id) {
                let data = series_index.query_range(start_time, end_time)?;
                if !data.is_empty() {
                    results.insert(series_id, data);
                }
            }
        }

        Ok(results)
    }
}

/// SIMD-accelerated range queries
pub struct SIMDRangeQuery {
    vector_size: usize,
}

impl SIMDRangeQuery {
    pub fn new(vector_size: usize) -> Self {
        Self { vector_size }
    }

    /// Find values in range using SIMD
    pub fn find_in_range(&self, values: &[f32], min_val: f32, max_val: f32) -> Vec<usize> {
        let mut results = Vec::new();

        // SIMD comparison would be implemented here
        // For now, using scalar implementation
        for (i, &value) in values.iter().enumerate() {
            if value >= min_val && value <= max_val {
                results.push(i);
            }
        }

        results
    }

    /// Count values in range using SIMD
    pub fn count_in_range(&self, values: &[f32], min_val: f32, max_val: f32) -> usize {
        // SIMD population count would be implemented here
        values.iter()
            .filter(|&&v| v >= min_val && v <= max_val)
            .count()
    }
}

/// Predictive indexing based on query patterns
pub struct PredictiveTimeIndex {
    base_index: TimeSeriesIndex,
    prediction_model: QueryPredictionModel,
    precomputed_indexes: RwLock<HashMap<QueryPattern, SeriesTimeIndex>>,
}

impl PredictiveTimeIndex {
    pub fn new() -> Self {
        Self {
            base_index: TimeSeriesIndex::new(),
            prediction_model: QueryPredictionModel::new(),
            precomputed_indexes: RwLock::new(HashMap::new()),
        }
    }

    /// Index with prediction
    pub fn index_with_prediction(&mut self, series_id: u64, timestamp: i64, value: f64) -> AuroraResult<()> {
        self.base_index.index_datapoint(series_id, timestamp, value)?;

        // Update prediction model
        self.prediction_model.observe_datapoint(series_id, timestamp, value);

        // Precompute indexes for predicted queries
        self.precompute_predicted_indexes(series_id)?;

        Ok(())
    }

    /// Query with predictive optimization
    pub fn predictive_query(&self, query: &TimeSeriesQuery) -> AuroraResult<HashMap<u64, Vec<(i64, f64)>>> {
        let pattern = QueryPattern::from_query(query);
        let precomputed = self.precomputed_indexes.read();

        // Check if we have a precomputed index for this pattern
        if let Some(precomputed_index) = precomputed.get(&pattern) {
            // Use precomputed index for faster query
            let mut results = HashMap::new();
            for &series_id in &query.series_ids {
                let data = precomputed_index.query_range(query.start_time, query.end_time)?;
                if !data.is_empty() {
                    results.insert(series_id, data);
                }
            }
            return Ok(results);
        }

        // Fallback to regular query
        self.base_index.query_multiple_series(&query.series_ids, query.start_time, query.end_time)
    }

    fn precompute_predicted_indexes(&self, series_id: u64) -> AuroraResult<()> {
        let predictions = self.prediction_model.predict_queries(series_id);

        let mut precomputed = self.precomputed_indexes.write();

        for pattern in predictions {
            if !precomputed.contains_key(&pattern) {
                // Create precomputed index for this pattern
                let data = self.base_index.query_series_range(series_id, pattern.start_time, pattern.end_time)?;
                let mut index = SeriesTimeIndex::new(series_id);

                for (ts, val) in data {
                    index.add_datapoint(ts, val)?;
                }

                precomputed.insert(pattern, index);
            }
        }

        Ok(())
    }
}

/// Query pattern for prediction
#[derive(Debug, Clone, Hash, Eq, PartialEq)]
pub struct QueryPattern {
    pub start_time: i64,
    pub end_time: i64,
    pub resolution: Option<TimeResolution>,
}

impl QueryPattern {
    fn from_query(query: &TimeSeriesQuery) -> Self {
        Self {
            start_time: query.start_time,
            end_time: query.end_time,
            resolution: query.resolution,
        }
    }
}

/// Query prediction model
#[derive(Debug)]
struct QueryPredictionModel {
    observed_patterns: HashMap<u64, Vec<QueryPattern>>,
}

impl QueryPredictionModel {
    fn new() -> Self {
        Self {
            observed_patterns: HashMap::new(),
        }
    }

    fn observe_datapoint(&mut self, series_id: u64, timestamp: i64, value: f64) {
        // Track query patterns for prediction
        // In a real implementation, this would analyze historical queries
    }

    fn predict_queries(&self, series_id: u64) -> Vec<QueryPattern> {
        // Predict likely future queries based on patterns
        // Simplified implementation
        vec![
            QueryPattern {
                start_time: 0,
                end_time: i64::MAX,
                resolution: Some(TimeResolution::Hour),
            }
        ]
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_series_time_index() {
        let mut index = SeriesTimeIndex::new(1);

        // Add data points
        index.add_datapoint(1000, 10.0).unwrap();
        index.add_datapoint(1001, 10.5).unwrap();
        index.add_datapoint(1002, 11.0).unwrap();
        index.add_datapoint(1005, 11.2).unwrap();

        // Query range
        let results = index.query_range(1000, 1002).unwrap();
        assert_eq!(results.len(), 3);
        assert_eq!(results[0], (1000, 10.0));
        assert_eq!(results[1], (1001, 10.5));
        assert_eq!(results[2], (1002, 11.0));

        // Query partial range
        let partial_results = index.query_range(1001, 1001).unwrap();
        assert_eq!(partial_results.len(), 1);
        assert_eq!(partial_results[0], (1001, 10.5));
    }

    #[test]
    fn test_time_series_index_basic() {
        let index = TimeSeriesIndex::new();

        // Index data points
        index.index_datapoint(1, 1000, 10.0).unwrap();
        index.index_datapoint(1, 1001, 10.5).unwrap();
        index.index_datapoint(2, 1000, 20.0).unwrap();

        // Query series
        let results1 = index.query_series_range(1, 1000, 1001).unwrap();
        assert_eq!(results1.len(), 2);

        let results2 = index.query_series_range(2, 1000, 1000).unwrap();
        assert_eq!(results2.len(), 1);
        assert_eq!(results2[0], (1000, 20.0));
    }

    #[test]
    fn test_multiple_series_query() {
        let index = TimeSeriesIndex::new();

        // Index data for multiple series
        index.index_datapoint(1, 1000, 10.0).unwrap();
        index.index_datapoint(2, 1000, 20.0).unwrap();
        index.index_datapoint(3, 1000, 30.0).unwrap();

        let series_ids = vec![1, 2];
        let results = index.query_multiple_series(&series_ids, 1000, 1000).unwrap();

        assert_eq!(results.len(), 2);
        assert_eq!(results[&1][0], (1000, 10.0));
        assert_eq!(results[&2][0], (1000, 20.0));
    }

    #[test]
    fn test_downsampling() {
        let mut index = SeriesTimeIndex::new(1);

        // Add data points (every second for 10 minutes)
        for i in 0..600 {
            index.add_datapoint(1000 + i, 10.0 + i as f64 * 0.1).unwrap();
        }

        // Downsample to minutes
        let downsampled = index.downsample(TimeResolution::Minute).unwrap();

        // Should have approximately 10 data points (one per minute)
        assert!(downsampled.len() >= 9 && downsampled.len() <= 11);

        // Check that timestamps are aligned to minute boundaries
        for (timestamp, _) in downsampled {
            assert_eq!(timestamp % 60000, 0); // Should be multiple of 60 seconds
        }
    }

    #[test]
    fn test_global_time_index() {
        let mut index = GlobalTimeIndex::new();

        // Add data points for different series
        index.add_datapoint(1, 1000).unwrap();
        index.add_datapoint(1, 1001).unwrap();
        index.add_datapoint(2, 1000).unwrap();
        index.add_datapoint(3, 2000).unwrap();

        // Query series in range
        let series_in_range = index.get_series_in_range(1000, 1000);
        assert_eq!(series_in_range.len(), 2);
        assert!(series_in_range.contains(&1));
        assert!(series_in_range.contains(&2));

        let series_in_range2 = index.get_series_in_range(1999, 2001);
        assert_eq!(series_in_range2.len(), 1);
        assert!(series_in_range2.contains(&3));
    }

    #[test]
    fn test_hierarchical_time_index() {
        let mut index = HierarchicalTimeIndex::new(3);

        // Add data points
        index.add_datapoint(1, 1000000, 10.0).unwrap(); // 1 second
        index.add_datapoint(2, 2000000, 20.0).unwrap(); // 2 seconds
        index.add_datapoint(1, 86400000 + 1000000, 15.0).unwrap(); // Next day + 1 second

        // Query range
        let results = index.query_range(500000, 1500000).unwrap();
        assert_eq!(results.len(), 1); // Only series 1
        assert_eq!(results[&1].len(), 1);
        assert_eq!(results[&1][0], (1000000, 10.0));
    }

    #[test]
    fn test_query_pattern_analysis() {
        let analyzer = QueryPatternAnalyzer::new();

        // Record some queries
        let query1 = TimeSeriesQuery {
            series_ids: vec![1],
            start_time: 1000,
            end_time: 2000,
            resolution: Some(TimeResolution::Minute),
            aggregation: None,
        };

        let query2 = TimeSeriesQuery {
            series_ids: vec![1, 2],
            start_time: 1000,
            end_time: 2000,
            resolution: Some(TimeResolution::Hour),
            aggregation: None,
        };

        analyzer.record_query(&query1);
        analyzer.record_query(&query2);

        let patterns = analyzer.analyze_patterns();
        assert_eq!(patterns.total_queries, 2);
        assert!(patterns.frequent_resolutions.contains(&TimeResolution::Minute));
        assert!(patterns.frequent_resolutions.contains(&TimeResolution::Hour));
    }

    #[test]
    fn test_simd_range_query() {
        let query = SIMDRangeQuery::new(4);

        let values = vec![1.0, 5.0, 3.0, 8.0, 2.0, 7.0];
        let in_range = query.find_in_range(&values, 2.0, 6.0);

        assert_eq!(in_range.len(), 3); // values 5.0, 3.0, 2.0

        let count = query.count_in_range(&values, 2.0, 6.0);
        assert_eq!(count, 3);
    }

    #[test]
    fn test_predictive_index() {
        let mut index = PredictiveTimeIndex::new();

        // Index data with prediction
        index.index_with_prediction(1, 1000, 10.0).unwrap();
        index.index_with_prediction(1, 1001, 10.5).unwrap();

        // Query with prediction
        let query = TimeSeriesQuery {
            series_ids: vec![1],
            start_time: 1000,
            end_time: 1001,
            resolution: None,
            aggregation: None,
        };

        let results = index.predictive_query(&query).unwrap();
        assert_eq!(results.len(), 1);
        assert_eq!(results[&1].len(), 2);
    }

    #[test]
    fn test_time_resolution_intervals() {
        assert_eq!(TimeResolution::Second.interval_ms(), 1000);
        assert_eq!(TimeResolution::Minute.interval_ms(), 60000);
        assert_eq!(TimeResolution::Hour.interval_ms(), 3600000);
        assert_eq!(TimeResolution::Day.interval_ms(), 86400000);
    }

    #[test]
    fn test_index_stats() {
        let mut stats = IndexStats::default();

        stats.update(1000, 10.0);
        stats.update(1001, 12.0);
        stats.update(1002, 8.0);

        assert_eq!(stats.total_points, 3);
        assert_eq!(stats.last_update, 1002);
        assert!(stats.avg_value > 9.0 && stats.avg_value < 11.0); // Around 10.0
    }
}
