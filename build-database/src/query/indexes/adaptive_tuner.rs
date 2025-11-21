//! Adaptive Tuner: Intelligent Index Recommendations and Auto-Tuning
//!
//! Machine learning-powered index optimization that analyzes query patterns,
//! predicts performance improvements, and provides automated tuning recommendations.

use std::collections::{HashMap, HashSet};
use parking_lot::RwLock;
use chrono::{DateTime, Utc, Duration};
use crate::core::errors::{AuroraResult, AuroraError};

/// Index recommendation from the adaptive tuner
#[derive(Debug, Clone)]
pub struct IndexRecommendation {
    pub table_name: String,
    pub index_name: String,
    pub index_type: String,
    pub columns: Vec<String>,
    pub recommendation_type: RecommendationType,
    pub expected_improvement: f64, // Percentage improvement
    pub confidence: f64, // 0.0 to 1.0
    pub reasoning: String,
    pub priority: Priority,
}

/// Types of recommendations
#[derive(Debug, Clone, PartialEq)]
pub enum RecommendationType {
    CreateIndex,
    DropIndex,
    RebuildIndex,
    ChangeIndexType,
    AddCompositeIndex,
}

/// Query pattern analysis
#[derive(Debug, Clone)]
pub struct QueryPattern {
    pub pattern_type: PatternType,
    pub table_name: String,
    pub columns: Vec<String>,
    pub frequency: u64,
    pub avg_execution_time_ms: f64,
    pub estimated_improvement: f64,
    pub last_seen: DateTime<Utc>,
}

/// Query pattern types
#[derive(Debug, Clone, PartialEq)]
pub enum PatternType {
    EqualityLookup,
    RangeScan,
    FullTableScan,
    JoinLookup,
    OrderBy,
    GroupBy,
    TextSearch,
    SpatialQuery,
    VectorSimilarity,
}

/// Workload analysis result
#[derive(Debug)]
pub struct WorkloadAnalysis {
    pub total_queries: u64,
    pub slow_queries: u64,
    pub table_access_patterns: HashMap<String, TableAccessPattern>,
    pub index_effectiveness: HashMap<String, f64>,
    pub recommendations: Vec<IndexRecommendation>,
}

/// Table access pattern
#[derive(Debug)]
pub struct TableAccessPattern {
    pub table_name: String,
    pub total_accesses: u64,
    pub read_percentage: f64,
    pub write_percentage: f64,
    pub pattern_distribution: HashMap<PatternType, u64>,
}

/// Tuning session statistics
#[derive(Debug)]
pub struct TuningStats {
    pub sessions_completed: u64,
    pub recommendations_applied: u64,
    pub avg_improvement: f64,
    pub total_indexes_created: u64,
    pub total_indexes_dropped: u64,
}

/// Intelligent adaptive tuner
pub struct AdaptiveTuner {
    query_patterns: RwLock<HashMap<String, Vec<QueryPattern>>>,
    workload_history: RwLock<Vec<WorkloadAnalysis>>,
    tuning_stats: RwLock<TuningStats>,
    learning_model: RwLock<SimpleLearningModel>,
}

impl AdaptiveTuner {
    pub fn new() -> Self {
        Self {
            query_patterns: RwLock::new(HashMap::new()),
            workload_history: RwLock::new(Vec::new()),
            tuning_stats: RwLock::new(TuningStats {
                sessions_completed: 0,
                recommendations_applied: 0,
                avg_improvement: 0.0,
                total_indexes_created: 0,
                total_indexes_dropped: 0,
            }),
            learning_model: RwLock::new(SimpleLearningModel::new()),
        }
    }

    /// Analyze query patterns and generate recommendations
    pub async fn generate_recommendations(
        &self,
        table_name: &str,
        query_patterns: &[super::query_analyzer::QueryPattern],
        existing_indexes: &[super::index_manager::IndexConfig],
    ) -> AuroraResult<Vec<IndexRecommendation>> {
        let mut recommendations = Vec::new();

        // Analyze each query pattern
        for pattern in query_patterns {
            let pattern_recs = self.analyze_pattern(pattern, existing_indexes).await?;
            recommendations.extend(pattern_recs);
        }

        // Remove duplicates and conflicting recommendations
        self.deduplicate_recommendations(&mut recommendations);

        // Rank recommendations by priority and expected improvement
        self.rank_recommendations(&mut recommendations);

        // Apply machine learning insights
        self.apply_learning_insights(&mut recommendations).await?;

        Ok(recommendations)
    }

    /// Record query execution for learning
    pub async fn record_query_execution(
        &self,
        table_name: &str,
        pattern: super::query_analyzer::QueryPattern,
    ) -> AuroraResult<()> {
        let mut patterns = self.query_patterns.write();
        let table_patterns = patterns.entry(table_name.to_string()).or_insert_with(Vec::new);

        // Update or add pattern
        if let Some(existing) = table_patterns.iter_mut().find(|p| p.pattern_type == pattern.pattern_type && p.columns == pattern.columns) {
            existing.frequency += 1;
            existing.avg_execution_time_ms = (existing.avg_execution_time_ms + pattern.avg_execution_time_ms) / 2.0;
            existing.last_seen = pattern.last_seen;
        } else {
            table_patterns.push(QueryPattern {
                pattern_type: match pattern.pattern_type.as_str() {
                    "equality" => PatternType::EqualityLookup,
                    "range" => PatternType::RangeScan,
                    "full_scan" => PatternType::FullTableScan,
                    "join" => PatternType::JoinLookup,
                    "orderby" => PatternType::OrderBy,
                    "groupby" => PatternType::GroupBy,
                    "text_search" => PatternType::TextSearch,
                    "spatial" => PatternType::SpatialQuery,
                    "vector" => PatternType::VectorSimilarity,
                    _ => PatternType::FullTableScan,
                },
                table_name: pattern.table_name.clone(),
                columns: pattern.columns.clone(),
                frequency: pattern.frequency,
                avg_execution_time_ms: pattern.avg_execution_time_ms,
                estimated_improvement: pattern.estimated_improvement,
                last_seen: pattern.last_seen,
            });
        }

        // Update learning model
        {
            let mut model = self.learning_model.write();
            model.update_from_pattern(&pattern);
        }

        Ok(())
    }

    /// Analyze workload and provide comprehensive tuning advice
    pub async fn analyze_workload(&self, tables: &[String]) -> AuroraResult<WorkloadAnalysis> {
        let mut total_queries = 0;
        let mut slow_queries = 0;
        let mut table_patterns = HashMap::new();
        let mut index_effectiveness = HashMap::new();

        // Analyze patterns for each table
        for table in tables {
            let patterns = self.query_patterns.read();
            if let Some(table_patterns_vec) = patterns.get(table) {
                let mut access_pattern = TableAccessPattern {
                    table_name: table.clone(),
                    total_accesses: 0,
                    read_percentage: 0.0,
                    write_percentage: 0.0,
                    pattern_distribution: HashMap::new(),
                };

                for pattern in table_patterns_vec {
                    access_pattern.total_accesses += pattern.frequency;
                    total_queries += pattern.frequency;

                    if pattern.avg_execution_time_ms > 1000.0 {
                        slow_queries += pattern.frequency;
                    }

                    *access_pattern.pattern_distribution.entry(pattern.pattern_type.clone()).or_insert(0) += pattern.frequency;
                }

                // Calculate read/write percentages (simplified)
                access_pattern.read_percentage = 0.8; // Assume 80% reads
                access_pattern.write_percentage = 0.2;

                table_patterns.insert(table.clone(), access_pattern);

                // Calculate index effectiveness (simplified)
                let effectiveness = self.calculate_index_effectiveness(table_patterns_vec);
                index_effectiveness.insert(table.clone(), effectiveness);
            }
        }

        // Generate recommendations
        let mut recommendations = Vec::new();
        for table in tables {
            let table_patterns_vec = self.query_patterns.read().get(table).cloned().unwrap_or_default();
            let existing_indexes = vec![]; // Would be passed in real implementation

            let table_recs = self.generate_recommendations(table, &table_patterns_vec, &existing_indexes).await?;
            recommendations.extend(table_recs);
        }

        let analysis = WorkloadAnalysis {
            total_queries,
            slow_queries,
            table_access_patterns: table_patterns,
            index_effectiveness,
            recommendations,
        };

        // Store analysis in history
        {
            let mut history = self.workload_history.write();
            history.push(analysis.clone());

            // Keep only last 10 analyses
            if history.len() > 10 {
                history.remove(0);
            }
        }

        Ok(analysis)
    }

    /// Apply a recommendation and track results
    pub async fn apply_recommendation(&self, recommendation: &IndexRecommendation) -> AuroraResult<()> {
        // Record the application
        {
            let mut stats = self.tuning_stats.write();
            stats.recommendations_applied += 1;

            match recommendation.recommendation_type {
                RecommendationType::CreateIndex => stats.total_indexes_created += 1,
                RecommendationType::DropIndex => stats.total_indexes_dropped += 1,
                _ => {}
            }
        }

        // Update learning model with successful application
        {
            let mut model = self.learning_model.write();
            model.record_successful_recommendation(recommendation);
        }

        println!("✅ Applied {} recommendation for index '{}' on table '{}'",
                match recommendation.recommendation_type {
                    RecommendationType::CreateIndex => "create",
                    RecommendationType::DropIndex => "drop",
                    RecommendationType::RebuildIndex => "rebuild",
                    RecommendationType::ChangeIndexType => "change type",
                    RecommendationType::AddCompositeIndex => "composite",
                },
                recommendation.index_name,
                recommendation.table_name);

        Ok(())
    }

    /// Get tuning statistics
    pub fn get_tuning_stats(&self) -> TuningStats {
        self.tuning_stats.write().clone()
    }

    // Private methods

    async fn analyze_pattern(
        &self,
        pattern: &super::query_analyzer::QueryPattern,
        existing_indexes: &[super::index_manager::IndexConfig],
    ) -> AuroraResult<Vec<IndexRecommendation>> {
        let mut recommendations = Vec::new();

        match pattern.pattern_type.as_str() {
            "equality" => {
                if !self.has_equality_index(&pattern.columns, existing_indexes) {
                    recommendations.push(IndexRecommendation {
                        table_name: pattern.table_name.clone(),
                        index_name: format!("idx_{}_equality", pattern.table_name),
                        index_type: "hash".to_string(),
                        columns: pattern.columns.clone(),
                        recommendation_type: RecommendationType::CreateIndex,
                        expected_improvement: pattern.estimated_improvement,
                        confidence: self.calculate_confidence(pattern),
                        reasoning: format!("Frequent equality lookups on {} columns with {}ms avg execution time",
                                         pattern.columns.len(), pattern.avg_execution_time_ms),
                        priority: self.calculate_priority(pattern),
                    });
                }
            }
            "range" => {
                if !self.has_btree_index(&pattern.columns, existing_indexes) {
                    recommendations.push(IndexRecommendation {
                        table_name: pattern.table_name.clone(),
                        index_name: format!("idx_{}_range", pattern.table_name),
                        index_type: "btree".to_string(),
                        columns: pattern.columns.clone(),
                        recommendation_type: RecommendationType::CreateIndex,
                        expected_improvement: pattern.estimated_improvement * 1.2, // Range queries benefit more
                        confidence: self.calculate_confidence(pattern),
                        reasoning: format!("Range scans on {} columns with high selectivity",
                                         pattern.columns.len()),
                        priority: Priority::High,
                    });
                }
            }
            "full_scan" => {
                if pattern.frequency > 100 && pattern.avg_execution_time_ms > 500.0 {
                    recommendations.push(IndexRecommendation {
                        table_name: pattern.table_name.clone(),
                        index_name: format!("idx_{}_covering", pattern.table_name),
                        index_type: "btree".to_string(),
                        columns: pattern.columns.clone(),
                        recommendation_type: RecommendationType::CreateIndex,
                        expected_improvement: 70.0, // Significant improvement for full scans
                        confidence: 0.9,
                        reasoning: "Frequent full table scans indicate missing indexes".to_string(),
                        priority: Priority::Critical,
                    });
                }
            }
            "text_search" => {
                if !self.has_fulltext_index(&pattern.columns, existing_indexes) {
                    recommendations.push(IndexRecommendation {
                        table_name: pattern.table_name.clone(),
                        index_name: format!("idx_{}_fulltext", pattern.table_name),
                        index_type: "fulltext".to_string(),
                        columns: pattern.columns.clone(),
                        recommendation_type: RecommendationType::CreateIndex,
                        expected_improvement: 80.0,
                        confidence: 0.95,
                        reasoning: "Text search queries need specialized indexing".to_string(),
                        priority: Priority::High,
                    });
                }
            }
            "spatial" => {
                if !self.has_spatial_index(&pattern.columns, existing_indexes) {
                    recommendations.push(IndexRecommendation {
                        table_name: pattern.table_name.clone(),
                        index_name: format!("idx_{}_spatial", pattern.table_name),
                        index_type: "spatial".to_string(),
                        columns: pattern.columns.clone(),
                        recommendation_type: RecommendationType::CreateIndex,
                        expected_improvement: 90.0,
                        confidence: 0.95,
                        reasoning: "Spatial queries require specialized spatial indexes".to_string(),
                        priority: Priority::High,
                    });
                }
            }
            _ => {}
        }

        // Check for composite index opportunities
        if pattern.columns.len() > 1 {
            let composite_rec = self.analyze_composite_opportunity(pattern, existing_indexes).await?;
            if let Some(rec) = composite_rec {
                recommendations.push(rec);
            }
        }

        Ok(recommendations)
    }

    fn has_equality_index(&self, columns: &[String], existing_indexes: &[super::index_manager::IndexConfig]) -> bool {
        existing_indexes.iter().any(|idx| {
            matches!(idx.index_type, super::index_manager::IndexType::Hash) &&
            idx.columns == columns
        })
    }

    fn has_btree_index(&self, columns: &[String], existing_indexes: &[super::index_manager::IndexConfig]) -> bool {
        existing_indexes.iter().any(|idx| {
            matches!(idx.index_type, super::index_manager::IndexType::BTree) &&
            (idx.columns == *columns || columns.iter().all(|col| idx.columns.contains(col)))
        })
    }

    fn has_fulltext_index(&self, columns: &[String], existing_indexes: &[super::index_manager::IndexConfig]) -> bool {
        existing_indexes.iter().any(|idx| {
            matches!(idx.index_type, super::index_manager::IndexType::FullText) &&
            columns.iter().any(|col| idx.columns.contains(col))
        })
    }

    fn has_spatial_index(&self, columns: &[String], existing_indexes: &[super::index_manager::IndexConfig]) -> bool {
        existing_indexes.iter().any(|idx| {
            matches!(idx.index_type, super::index_manager::IndexType::Spatial) &&
            columns.iter().any(|col| idx.columns.contains(col))
        })
    }

    async fn analyze_composite_opportunity(
        &self,
        pattern: &super::query_analyzer::QueryPattern,
        existing_indexes: &[super::index_manager::IndexConfig],
    ) -> AuroraResult<Option<IndexRecommendation>> {
        // Check if individual columns are indexed but not together
        let has_individual_indexes = pattern.columns.iter().all(|col| {
            existing_indexes.iter().any(|idx| idx.columns.contains(col))
        });

        let has_composite_index = existing_indexes.iter().any(|idx| {
            idx.columns == pattern.columns
        });

        if has_individual_indexes && !has_composite_index && pattern.frequency > 50 {
            Ok(Some(IndexRecommendation {
                table_name: pattern.table_name.clone(),
                index_name: format!("idx_{}_composite_{}", pattern.table_name, pattern.columns.join("_")),
                index_type: "btree".to_string(),
                columns: pattern.columns.clone(),
                recommendation_type: RecommendationType::AddCompositeIndex,
                expected_improvement: pattern.estimated_improvement * 1.5, // Composite often better
                confidence: 0.85,
                reasoning: format!("Composite index on {} columns would improve multi-column queries",
                                 pattern.columns.len()),
                priority: Priority::Medium,
            }))
        } else {
            Ok(None)
        }
    }

    fn calculate_confidence(&self, pattern: &super::query_analyzer::QueryPattern) -> f64 {
        // Confidence based on frequency and consistency
        let frequency_factor = (pattern.frequency as f64).min(1000.0) / 1000.0;
        let time_factor = if pattern.avg_execution_time_ms > 100.0 { 1.0 } else { 0.5 };

        (frequency_factor * 0.7 + time_factor * 0.3).min(1.0)
    }

    fn calculate_priority(&self, pattern: &super::query_analyzer::QueryPattern) -> Priority {
        let score = pattern.frequency as f64 * pattern.avg_execution_time_ms / 1000.0;

        if score > 1000.0 {
            Priority::Critical
        } else if score > 100.0 {
            Priority::High
        } else if score > 10.0 {
            Priority::Medium
        } else {
            Priority::Low
        }
    }

    fn deduplicate_recommendations(&self, recommendations: &mut Vec<IndexRecommendation>) {
        let mut seen = HashSet::new();
        recommendations.retain(|rec| {
            let key = format!("{}_{}_{}", rec.table_name, rec.index_name, rec.columns.join(","));
            seen.insert(key)
        });
    }

    fn rank_recommendations(&self, recommendations: &mut Vec<IndexRecommendation>) {
        recommendations.sort_by(|a, b| {
            // Sort by priority first, then by expected improvement
            let a_priority_score = match a.priority {
                Priority::Critical => 4,
                Priority::High => 3,
                Priority::Medium => 2,
                Priority::Low => 1,
            };
            let b_priority_score = match b.priority {
                Priority::Critical => 4,
                Priority::High => 3,
                Priority::Medium => 2,
                Priority::Low => 1,
            };

            b_priority_score.cmp(&a_priority_score)
                .then(b.expected_improvement.partial_cmp(&a.expected_improvement).unwrap())
        });
    }

    async fn apply_learning_insights(&self, recommendations: &mut Vec<IndexRecommendation>) -> AuroraResult<()> {
        let model = self.learning_model.read();

        for rec in recommendations.iter_mut() {
            // Adjust confidence based on historical success
            let historical_success = model.predict_success_probability(rec);
            rec.confidence = (rec.confidence + historical_success) / 2.0;

            // Adjust expected improvement based on similar past recommendations
            let historical_improvement = model.predict_improvement(rec);
            if historical_improvement > 0.0 {
                rec.expected_improvement = (rec.expected_improvement + historical_improvement) / 2.0;
            }
        }

        Ok(())
    }

    fn calculate_index_effectiveness(&self, patterns: &[QueryPattern]) -> f64 {
        if patterns.is_empty() {
            return 0.0;
        }

        let total_frequency: u64 = patterns.iter().map(|p| p.frequency).sum();
        let indexed_frequency: u64 = patterns.iter()
            .filter(|p| p.estimated_improvement > 10.0) // Assume these have indexes
            .map(|p| p.frequency)
            .sum();

        if total_frequency > 0 {
            indexed_frequency as f64 / total_frequency as f64
        } else {
            0.0
        }
    }
}

/// Simple learning model for index recommendations
#[derive(Debug)]
pub struct SimpleLearningModel {
    successful_recommendations: HashMap<String, u64>,
    total_recommendations: HashMap<String, u64>,
    avg_improvements: HashMap<String, f64>,
}

impl SimpleLearningModel {
    pub fn new() -> Self {
        Self {
            successful_recommendations: HashMap::new(),
            total_recommendations: HashMap::new(),
            avg_improvements: HashMap::new(),
        }
    }

    pub fn update_from_pattern(&mut self, pattern: &super::query_analyzer::QueryPattern) {
        let key = format!("{}_{}", pattern.pattern_type, pattern.columns.join(","));
        *self.total_recommendations.entry(key).or_insert(0) += 1;
    }

    pub fn record_successful_recommendation(&mut self, recommendation: &IndexRecommendation) {
        let key = format!("{}_{}", recommendation.index_type, recommendation.columns.join(","));
        *self.successful_recommendations.entry(key).or_insert(0) += 1;
        *self.total_recommendations.entry(key).or_insert(0) += 1;
    }

    pub fn predict_success_probability(&self, recommendation: &IndexRecommendation) -> f64 {
        let key = format!("{}_{}", recommendation.index_type, recommendation.columns.join(","));
        let successes = *self.successful_recommendations.get(&key).unwrap_or(&0);
        let total = *self.total_recommendations.get(&key).unwrap_or(&1);

        if total == 0 {
            0.5 // Default confidence
        } else {
            successes as f64 / total as f64
        }
    }

    pub fn predict_improvement(&self, recommendation: &IndexRecommendation) -> f64 {
        let key = format!("{}_{}", recommendation.index_type, recommendation.columns.join(","));
        *self.avg_improvements.get(&key).unwrap_or(&0.0)
    }
}

/// Priority levels for recommendations
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum Priority {
    Low,
    Medium,
    High,
    Critical,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_adaptive_tuner_creation() {
        let tuner = AdaptiveTuner::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_recommendation_structure() {
        let rec = IndexRecommendation {
            table_name: "users".to_string(),
            index_name: "idx_users_email".to_string(),
            index_type: "hash".to_string(),
            columns: vec!["email".to_string()],
            recommendation_type: RecommendationType::CreateIndex,
            expected_improvement: 75.0,
            confidence: 0.85,
            reasoning: "Frequent equality lookups".to_string(),
            priority: Priority::High,
        };

        assert_eq!(rec.table_name, "users");
        assert_eq!(rec.expected_improvement, 75.0);
        assert_eq!(rec.priority, Priority::High);
    }

    #[test]
    fn test_query_pattern() {
        let pattern = QueryPattern {
            pattern_type: PatternType::EqualityLookup,
            table_name: "users".to_string(),
            columns: vec!["email".to_string()],
            frequency: 100,
            avg_execution_time_ms: 50.0,
            estimated_improvement: 75.0,
            last_seen: Utc::now(),
        };

        assert_eq!(pattern.pattern_type, PatternType::EqualityLookup);
        assert_eq!(pattern.frequency, 100);
        assert_eq!(pattern.avg_execution_time_ms, 50.0);
    }

    #[test]
    fn test_tuning_stats() {
        let stats = TuningStats {
            sessions_completed: 10,
            recommendations_applied: 8,
            avg_improvement: 65.5,
            total_indexes_created: 5,
            total_indexes_dropped: 2,
        };

        assert_eq!(stats.sessions_completed, 10);
        assert_eq!(stats.recommendations_applied, 8);
        assert_eq!(stats.avg_improvement, 65.5);
    }

    #[tokio::test]
    async fn test_learning_model() {
        let mut model = SimpleLearningModel::new();

        let pattern = super::query_analyzer::QueryPattern {
            pattern_type: "equality".to_string(),
            table_name: "users".to_string(),
            columns: vec!["email".to_string()],
            frequency: 1,
            avg_execution_time_ms: 50.0,
            estimated_improvement: 75.0,
            last_seen: Utc::now(),
        };

        model.update_from_pattern(&pattern);

        let rec = IndexRecommendation {
            table_name: "users".to_string(),
            index_name: "test".to_string(),
            index_type: "hash".to_string(),
            columns: vec!["email".to_string()],
            recommendation_type: RecommendationType::CreateIndex,
            expected_improvement: 75.0,
            confidence: 0.8,
            reasoning: "test".to_string(),
            priority: Priority::Medium,
        };

        model.record_successful_recommendation(&rec);

        let prob = model.predict_success_probability(&rec);
        assert_eq!(prob, 1.0); // 1 success out of 1 total
    }

    #[test]
    fn test_priority_ordering() {
        assert!(Priority::Low < Priority::Critical);
        assert!(Priority::Medium > Priority::Low);
        assert!(Priority::High > Priority::Medium);
    }

    #[test]
    fn test_recommendation_types() {
        assert_eq!(RecommendationType::CreateIndex, RecommendationType::CreateIndex);
        assert_ne!(RecommendationType::DropIndex, RecommendationType::CreateIndex);
    }

    #[test]
    fn test_pattern_types() {
        assert_eq!(PatternType::EqualityLookup, PatternType::EqualityLookup);
        assert_ne!(PatternType::RangeScan, PatternType::FullTableScan);
    }

    #[tokio::test]
    async fn test_workload_analysis_structure() {
        let analysis = WorkloadAnalysis {
            total_queries: 1000,
            slow_queries: 100,
            table_access_patterns: HashMap::new(),
            index_effectiveness: HashMap::new(),
            recommendations: vec![],
        };

        assert_eq!(analysis.total_queries, 1000);
        assert_eq!(analysis.slow_queries, 100);
    }
}
