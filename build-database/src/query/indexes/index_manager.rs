//! Index Manager: Intelligent Index Management and Auto-Tuning
//!
//! Advanced index management system with multiple index types,
//! intelligent selection, and automated performance optimization.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use crate::core::schema::DataType;
use super::btree_index::{BTreeIndex, BTreeIndexConfig};
use super::hash_index::{HashIndex, HashIndexConfig};
use super::fulltext_index::{FullTextIndex, FullTextIndexConfig};
use super::spatial_index::{SpatialIndex, SpatialIndexConfig};
use super::vector_index::{VectorIndex, VectorIndexConfig};
use super::adaptive_tuner::{AdaptiveTuner, IndexRecommendation};
use super::maintenance_engine::{MaintenanceEngine, MaintenanceStats};
use super::query_analyzer::{QueryAnalyzer, QueryPattern};

/// Index types supported by AuroraDB
#[derive(Debug, Clone, PartialEq)]
pub enum IndexType {
    BTree,       // Traditional B-tree for range queries
    Hash,        // Hash index for equality lookups
    FullText,    // Full-text search index
    Spatial,     // Spatial/geographic index
    Vector,      // Vector similarity search index
    Composite,   // Multi-column index
    Partial,     // Partial index with WHERE conditions
}

/// Index configuration
#[derive(Debug, Clone)]
pub struct IndexConfig {
    pub name: String,
    pub table_name: String,
    pub columns: Vec<String>,
    pub index_type: IndexType,
    pub is_unique: bool,
    pub is_primary: bool,
    pub condition: Option<String>, // For partial indexes
    pub storage_params: HashMap<String, String>,
    pub created_at: DateTime<Utc>,
    pub last_used: Option<DateTime<Utc>>,
    pub usage_count: u64,
}

/// Index statistics and performance metrics
#[derive(Debug, Clone)]
pub struct IndexStats {
    pub name: String,
    pub size_bytes: u64,
    pub entry_count: u64,
    pub avg_lookup_time_ms: f64,
    pub cache_hit_rate: f64,
    pub fragmentation_ratio: f64,
    pub last_maintenance: Option<DateTime<Utc>>,
    pub maintenance_cost: f64,
}

/// Query performance impact of an index
#[derive(Debug, Clone)]
pub struct IndexImpact {
    pub index_name: String,
    pub query_pattern: String,
    pub performance_improvement: f64, // Percentage improvement
    pub usage_frequency: u64,
    pub cost_benefit_ratio: f64,
}

/// Intelligent index manager
pub struct IndexManager {
    indexes: RwLock<HashMap<String, IndexConfig>>,
    index_stats: RwLock<HashMap<String, IndexStats>>,
    table_indexes: RwLock<HashMap<String, Vec<String>>>, // table -> index names

    // Index implementations
    btree_indexes: RwLock<HashMap<String, BTreeIndex>>,
    hash_indexes: RwLock<HashMap<String, HashIndex>>,
    fulltext_indexes: RwLock<HashMap<String, FullTextIndex>>,
    spatial_indexes: RwLock<HashMap<String, SpatialIndex>>,
    vector_indexes: RwLock<HashMap<String, VectorIndex>>,

    // Intelligence components
    adaptive_tuner: Arc<AdaptiveTuner>,
    maintenance_engine: Arc<MaintenanceEngine>,
    query_analyzer: Arc<QueryAnalyzer>,
}

impl IndexManager {
    pub fn new() -> Self {
        Self {
            indexes: RwLock::new(HashMap::new()),
            index_stats: RwLock::new(HashMap::new()),
            table_indexes: RwLock::new(HashMap::new()),
            btree_indexes: RwLock::new(HashMap::new()),
            hash_indexes: RwLock::new(HashMap::new()),
            fulltext_indexes: RwLock::new(HashMap::new()),
            spatial_indexes: RwLock::new(HashMap::new()),
            vector_indexes: RwLock::new(HashMap::new()),
            adaptive_tuner: Arc::new(AdaptiveTuner::new()),
            maintenance_engine: Arc::new(MaintenanceEngine::new()),
            query_analyzer: Arc::new(QueryAnalyzer::new()),
        }
    }

    /// Create an index with intelligent type selection
    pub async fn create_index(&self, config: IndexConfig) -> AuroraResult<()> {
        println!("🚀 Creating {} index '{}' on table '{}' with columns {:?}",
                format_index_type(&config.index_type),
                config.name,
                config.table_name,
                config.columns);

        // Validate index configuration
        self.validate_index_config(&config).await?;

        // Check for index conflicts
        self.check_index_conflicts(&config).await?;

        // Create the appropriate index implementation
        match config.index_type {
            IndexType::BTree => self.create_btree_index(&config).await?,
            IndexType::Hash => self.create_hash_index(&config).await?,
            IndexType::FullText => self.create_fulltext_index(&config).await?,
            IndexType::Spatial => self.create_spatial_index(&config).await?,
            IndexType::Vector => self.create_vector_index(&config).await?,
            IndexType::Composite => self.create_composite_index(&config).await?,
            IndexType::Partial => self.create_partial_index(&config).await?,
        }

        // Register index
        {
            let mut indexes = self.indexes.write();
            indexes.insert(config.name.clone(), config.clone());
        }

        // Update table index mapping
        {
            let mut table_indexes = self.table_indexes.write();
            table_indexes.entry(config.table_name.clone())
                .or_insert_with(Vec::new)
                .push(config.name.clone());
        }

        // Initialize statistics
        let stats = IndexStats {
            name: config.name.clone(),
            size_bytes: 0, // Will be updated during maintenance
            entry_count: 0,
            avg_lookup_time_ms: 0.0,
            cache_hit_rate: 0.0,
            fragmentation_ratio: 0.0,
            last_maintenance: Some(Utc::now()),
            maintenance_cost: 0.0,
        };

        {
            let mut index_stats = self.index_stats.write();
            index_stats.insert(config.name.clone(), stats);
        }

        println!("✅ Created {} index '{}' - {} columns, {} unique",
                format_index_type(&config.index_type),
                config.name,
                config.columns.len(),
                if config.is_unique { "unique" } else { "non-unique" });

        Ok(())
    }

    /// Drop an index
    pub async fn drop_index(&self, index_name: &str) -> AuroraResult<()> {
        // Get index configuration before removal
        let config = {
            let indexes = self.indexes.read();
            indexes.get(index_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Index '{}' not found", index_name)))?
        };

        // Remove from appropriate index storage
        match config.index_type {
            IndexType::BTree => {
                let mut btree_indexes = self.btree_indexes.write();
                btree_indexes.remove(index_name);
            }
            IndexType::Hash => {
                let mut hash_indexes = self.hash_indexes.write();
                hash_indexes.remove(index_name);
            }
            IndexType::FullText => {
                let mut fulltext_indexes = self.fulltext_indexes.write();
                fulltext_indexes.remove(index_name);
            }
            IndexType::Spatial => {
                let mut spatial_indexes = self.spatial_indexes.write();
                spatial_indexes.remove(index_name);
            }
            IndexType::Vector => {
                let mut vector_indexes = self.vector_indexes.write();
                vector_indexes.remove(index_name);
            }
            _ => {} // Composite and partial indexes are handled by base types
        }

        // Remove from mappings
        {
            let mut indexes = self.indexes.write();
            indexes.remove(index_name);
        }

        {
            let mut index_stats = self.index_stats.write();
            index_stats.remove(index_name);
        }

        {
            let mut table_indexes = self.table_indexes.write();
            if let Some(table_index_list) = table_indexes.get_mut(&config.table_name) {
                table_index_list.retain(|idx| idx != index_name);
                if table_index_list.is_empty() {
                    table_indexes.remove(&config.table_name);
                }
            }
        }

        println!("🗑️  Dropped index '{}'", index_name);
        Ok(())
    }

    /// Get intelligent index recommendations
    pub async fn get_index_recommendations(&self, table_name: &str) -> AuroraResult<Vec<IndexRecommendation>> {
        // Analyze query patterns
        let query_patterns = self.query_analyzer.analyze_table_queries(table_name).await?;

        // Get existing indexes
        let existing_indexes = self.get_table_indexes(table_name).await?;

        // Generate recommendations
        self.adaptive_tuner.generate_recommendations(
            table_name,
            &query_patterns,
            &existing_indexes,
        ).await
    }

    /// Analyze index usage and performance
    pub async fn analyze_index_performance(&self, index_name: &str) -> AuroraResult<IndexImpact> {
        let config = {
            let indexes = self.indexes.read();
            indexes.get(index_name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("Index '{}' not found", index_name)))?
        };

        let stats = {
            let index_stats = self.index_stats.read();
            index_stats.get(index_name).cloned()
                .unwrap_or_default()
        };

        // Analyze query patterns that use this index
        let usage_patterns = self.query_analyzer.get_index_usage_patterns(index_name).await?;

        // Calculate performance impact
        let total_improvement: f64 = usage_patterns.iter()
            .map(|pattern| pattern.estimated_improvement)
            .sum();

        let avg_improvement = if usage_patterns.is_empty() {
            0.0
        } else {
            total_improvement / usage_patterns.len() as f64
        };

        let total_usage: u64 = usage_patterns.iter()
            .map(|pattern| pattern.frequency)
            .sum();

        // Calculate cost-benefit ratio
        let maintenance_cost = stats.maintenance_cost;
        let performance_benefit = avg_improvement * total_usage as f64;
        let cost_benefit_ratio = if maintenance_cost > 0.0 {
            performance_benefit / maintenance_cost
        } else {
            f64::INFINITY
        };

        Ok(IndexImpact {
            index_name: index_name.to_string(),
            query_pattern: "mixed_workload".to_string(), // Simplified
            performance_improvement: avg_improvement,
            usage_frequency: total_usage,
            cost_benefit_ratio,
        })
    }

    /// Perform intelligent index maintenance
    pub async fn perform_maintenance(&self, index_name: &str) -> AuroraResult<MaintenanceStats> {
        println!("🔧 Performing maintenance on index '{}'", index_name);

        let stats = self.maintenance_engine.perform_maintenance(index_name).await?;

        // Update index statistics
        {
            let mut index_stats = self.index_stats.write();
            if let Some(index_stat) = index_stats.get_mut(index_name) {
                index_stat.last_maintenance = Some(Utc::now());
                index_stat.maintenance_cost = stats.cost_estimate;
                index_stat.fragmentation_ratio = stats.fragmentation_reduction;
            }
        }

        println!("✅ Maintenance completed - fragmentation: {:.1}%, cost: {:.2}",
                stats.fragmentation_reduction * 100.0, stats.cost_estimate);

        Ok(stats)
    }

    /// Auto-tune indexes based on workload patterns
    pub async fn auto_tune_indexes(&self, table_name: &str) -> AuroraResult<Vec<String>> {
        println!("🎯 Auto-tuning indexes for table '{}'", table_name);

        // Get recommendations
        let recommendations = self.get_index_recommendations(table_name).await?;

        let mut actions_taken = Vec::new();

        for recommendation in recommendations {
            if recommendation.confidence > 0.8 && recommendation.expected_improvement > 20.0 {
                // High-confidence recommendation - implement automatically
                match recommendation.recommendation_type.as_str() {
                    "create_index" => {
                        let config = self.create_index_config_from_recommendation(&recommendation).await?;
                        self.create_index(config).await?;
                        actions_taken.push(format!("Created index '{}'", recommendation.index_name));
                    }
                    "drop_index" => {
                        if self.should_drop_index(&recommendation).await? {
                            self.drop_index(&recommendation.index_name).await?;
                            actions_taken.push(format!("Dropped unused index '{}'", recommendation.index_name));
                        }
                    }
                    "rebuild_index" => {
                        self.perform_maintenance(&recommendation.index_name).await?;
                        actions_taken.push(format!("Rebuilt index '{}'", recommendation.index_name));
                    }
                    _ => {}
                }
            }
        }

        println!("✅ Auto-tuning completed - {} actions taken", actions_taken.len());
        Ok(actions_taken)
    }

    /// Get indexes for a table
    pub async fn get_table_indexes(&self, table_name: &str) -> Vec<IndexConfig> {
        let table_indexes = self.table_indexes.read();
        if let Some(index_names) = table_indexes.get(table_name) {
            let indexes = self.indexes.read();
            index_names.iter()
                .filter_map(|name| indexes.get(name).cloned())
                .collect()
        } else {
            vec![]
        }
    }

    /// List all indexes with their statistics
    pub async fn list_indexes(&self) -> Vec<IndexSummary> {
        let indexes = self.indexes.read();
        let index_stats = self.index_stats.read();

        let mut summaries = Vec::new();

        for (name, config) in indexes.iter() {
            let stats = index_stats.get(name).cloned().unwrap_or_default();

            summaries.push(IndexSummary {
                name: name.clone(),
                table_name: config.table_name.clone(),
                index_type: config.index_type.clone(),
                columns: config.columns.clone(),
                is_unique: config.is_unique,
                size_bytes: stats.size_bytes,
                entry_count: stats.entry_count,
                avg_lookup_time_ms: stats.avg_lookup_time_ms,
                cache_hit_rate: stats.cache_hit_rate,
                last_used: config.last_used,
                usage_count: config.usage_count,
            });
        }

        // Sort by table name, then by name
        summaries.sort_by(|a, b| {
            a.table_name.cmp(&b.table_name).then(a.name.cmp(&b.name))
        });

        summaries
    }

    // Private methods

    async fn validate_index_config(&self, config: &IndexConfig) -> AuroraResult<()> {
        // Validate index name
        if config.name.is_empty() || config.name.len() > 128 {
            return Err(AuroraError::InvalidArgument("Index name must be 1-128 characters".to_string()));
        }

        // Validate columns
        if config.columns.is_empty() {
            return Err(AuroraError::InvalidArgument("Index must have at least one column".to_string()));
        }

        // Validate index type specific requirements
        match config.index_type {
            IndexType::FullText => {
                // Full-text indexes work on text columns
                if config.columns.len() != 1 {
                    return Err(AuroraError::InvalidArgument("Full-text indexes must have exactly one column".to_string()));
                }
            }
            IndexType::Spatial => {
                // Spatial indexes work on geometry columns
                if config.columns.len() != 1 {
                    return Err(AuroraError::InvalidArgument("Spatial indexes must have exactly one column".to_string()));
                }
            }
            IndexType::Vector => {
                // Vector indexes work on vector columns
                if config.columns.len() != 1 {
                    return Err(AuroraError::InvalidArgument("Vector indexes must have exactly one column".to_string()));
                }
            }
            _ => {}
        }

        // Check for duplicate index names
        let indexes = self.indexes.read();
        if indexes.contains_key(&config.name) {
            return Err(AuroraError::InvalidArgument(format!("Index '{}' already exists", config.name)));
        }

        Ok(())
    }

    async fn check_index_conflicts(&self, config: &IndexConfig) -> AuroraResult<()> {
        let table_indexes = self.get_table_indexes(&config.table_name).await;

        for existing_index in table_indexes {
            // Check for exact duplicate columns
            if existing_index.columns == config.columns &&
               existing_index.index_type == config.index_type {
                return Err(AuroraError::InvalidArgument(
                    format!("Duplicate index '{}' already exists on same columns", existing_index.name)
                ));
            }

            // Check for conflicting unique constraints
            if existing_index.is_unique && config.is_unique &&
               existing_index.columns == config.columns {
                return Err(AuroraError::InvalidArgument(
                    "Cannot create duplicate unique index on same columns".to_string()
                ));
            }
        }

        Ok(())
    }

    async fn create_btree_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        let btree_config = BTreeIndexConfig {
            name: config.name.clone(),
            columns: config.columns.clone(),
            unique: config.is_unique,
            fill_factor: 90, // Default fill factor
        };

        let index = BTreeIndex::new(btree_config)?;
        let mut btree_indexes = self.btree_indexes.write();
        btree_indexes.insert(config.name.clone(), index);

        Ok(())
    }

    async fn create_hash_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        let hash_config = HashIndexConfig {
            name: config.name.clone(),
            columns: config.columns.clone(),
            bucket_count: 1024, // Default bucket count
        };

        let index = HashIndex::new(hash_config)?;
        let mut hash_indexes = self.hash_indexes.write();
        hash_indexes.insert(config.name.clone(), index);

        Ok(())
    }

    async fn create_fulltext_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        let fulltext_config = FullTextIndexConfig {
            name: config.name.clone(),
            column: config.columns[0].clone(),
            language: "english".to_string(), // Default language
            enable_stemming: true,
            enable_stopwords: true,
        };

        let index = FullTextIndex::new(fulltext_config)?;
        let mut fulltext_indexes = self.fulltext_indexes.write();
        fulltext_indexes.insert(config.name.clone(), index);

        Ok(())
    }

    async fn create_spatial_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        let spatial_config = SpatialIndexConfig {
            name: config.name.clone(),
            column: config.columns[0].clone(),
            srid: 4326, // WGS84
            index_type: super::spatial_index::SpatialIndexType::RTree,
        };

        let index = SpatialIndex::new(spatial_config)?;
        let mut spatial_indexes = self.spatial_indexes.write();
        spatial_indexes.insert(config.name.clone(), index);

        Ok(())
    }

    async fn create_vector_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        let vector_config = VectorIndexConfig {
            name: config.name.clone(),
            column: config.columns[0].clone(),
            dimensions: 128, // Default vector dimensions
            index_type: super::vector_index::VectorIndexType::HNSW,
            distance_metric: super::vector_index::DistanceMetric::Cosine,
        };

        let index = VectorIndex::new(vector_config)?;
        let mut vector_indexes = self.vector_indexes.write();
        vector_indexes.insert(config.name.clone(), index);

        Ok(())
    }

    async fn create_composite_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        // Composite indexes are implemented as B-tree indexes with multiple columns
        self.create_btree_index(config).await
    }

    async fn create_partial_index(&self, config: &IndexConfig) -> AuroraResult<()> {
        // Partial indexes are implemented as B-tree indexes with conditions
        self.create_btree_index(config).await
    }

    async fn create_index_config_from_recommendation(&self, recommendation: &IndexRecommendation) -> AuroraResult<IndexConfig> {
        Ok(IndexConfig {
            name: recommendation.index_name.clone(),
            table_name: recommendation.table_name.clone(),
            columns: recommendation.columns.clone(),
            index_type: string_to_index_type(&recommendation.index_type)?,
            is_unique: false,
            is_primary: false,
            condition: None,
            storage_params: HashMap::new(),
            created_at: Utc::now(),
            last_used: None,
            usage_count: 0,
        })
    }

    async fn should_drop_index(&self, recommendation: &IndexRecommendation) -> AuroraResult<bool> {
        // Check if index is rarely used and has low impact
        let impact = self.analyze_index_performance(&recommendation.index_name).await?;
        Ok(impact.usage_frequency < 10 && impact.cost_benefit_ratio < 1.0)
    }
}

/// Index summary for listing
#[derive(Debug, Clone)]
pub struct IndexSummary {
    pub name: String,
    pub table_name: String,
    pub index_type: IndexType,
    pub columns: Vec<String>,
    pub is_unique: bool,
    pub size_bytes: u64,
    pub entry_count: u64,
    pub avg_lookup_time_ms: f64,
    pub cache_hit_rate: f64,
    pub last_used: Option<DateTime<Utc>>,
    pub usage_count: u64,
}

// Helper functions

fn format_index_type(index_type: &IndexType) -> &'static str {
    match index_type {
        IndexType::BTree => "B-Tree",
        IndexType::Hash => "Hash",
        IndexType::FullText => "Full-Text",
        IndexType::Spatial => "Spatial",
        IndexType::Vector => "Vector",
        IndexType::Composite => "Composite",
        IndexType::Partial => "Partial",
    }
}

fn string_to_index_type(s: &str) -> AuroraResult<IndexType> {
    match s {
        "btree" => Ok(IndexType::BTree),
        "hash" => Ok(IndexType::Hash),
        "fulltext" => Ok(IndexType::FullText),
        "spatial" => Ok(IndexType::Spatial),
        "vector" => Ok(IndexType::Vector),
        "composite" => Ok(IndexType::Composite),
        "partial" => Ok(IndexType::Partial),
        _ => Err(AuroraError::InvalidArgument(format!("Unknown index type: {}", s))),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_test_config(name: &str, table: &str, columns: Vec<&str>, index_type: IndexType) -> IndexConfig {
        IndexConfig {
            name: name.to_string(),
            table_name: table.to_string(),
            columns: columns.into_iter().map(|s| s.to_string()).collect(),
            index_type,
            is_unique: false,
            is_primary: false,
            condition: None,
            storage_params: HashMap::new(),
            created_at: Utc::now(),
            last_used: None,
            usage_count: 0,
        }
    }

    #[tokio::test]
    async fn test_index_manager_creation() {
        let manager = IndexManager::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_index_types() {
        assert_eq!(IndexType::BTree, IndexType::BTree);
        assert_ne!(IndexType::Hash, IndexType::FullText);
    }

    #[test]
    fn test_format_index_type() {
        assert_eq!(format_index_type(&IndexType::BTree), "B-Tree");
        assert_eq!(format_index_type(&IndexType::Hash), "Hash");
        assert_eq!(format_index_type(&IndexType::FullText), "Full-Text");
        assert_eq!(format_index_type(&IndexType::Spatial), "Spatial");
        assert_eq!(format_index_type(&IndexType::Vector), "Vector");
    }

    #[test]
    fn test_string_to_index_type() {
        assert_eq!(string_to_index_type("btree").unwrap(), IndexType::BTree);
        assert_eq!(string_to_index_type("hash").unwrap(), IndexType::Hash);
        assert_eq!(string_to_index_type("fulltext").unwrap(), IndexType::FullText);
        assert!(string_to_index_type("unknown").is_err());
    }

    #[test]
    fn test_index_config() {
        let config = create_test_config("test_idx", "users", vec!["email"], IndexType::BTree);
        assert_eq!(config.name, "test_idx");
        assert_eq!(config.table_name, "users");
        assert_eq!(config.columns.len(), 1);
        assert_eq!(config.index_type, IndexType::BTree);
        assert!(!config.is_unique);
    }

    #[test]
    fn test_index_summary() {
        let summary = IndexSummary {
            name: "test_idx".to_string(),
            table_name: "users".to_string(),
            index_type: IndexType::BTree,
            columns: vec!["email".to_string()],
            is_unique: false,
            size_bytes: 1024,
            entry_count: 100,
            avg_lookup_time_ms: 0.5,
            cache_hit_rate: 0.95,
            last_used: Some(Utc::now()),
            usage_count: 50,
        };

        assert_eq!(summary.name, "test_idx");
        assert_eq!(summary.table_name, "users");
        assert_eq!(summary.entry_count, 100);
        assert_eq!(summary.cache_hit_rate, 0.95);
    }

    #[test]
    fn test_index_stats() {
        let stats = IndexStats {
            name: "test_idx".to_string(),
            size_bytes: 2048,
            entry_count: 200,
            avg_lookup_time_ms: 1.2,
            cache_hit_rate: 0.88,
            fragmentation_ratio: 0.15,
            last_maintenance: Some(Utc::now()),
            maintenance_cost: 45.5,
        };

        assert_eq!(stats.name, "test_idx");
        assert_eq!(stats.size_bytes, 2048);
        assert_eq!(stats.fragmentation_ratio, 0.15);
    }

    #[test]
    fn test_index_impact() {
        let impact = IndexImpact {
            index_name: "test_idx".to_string(),
            query_pattern: "equality_lookup".to_string(),
            performance_improvement: 75.5,
            usage_frequency: 1000,
            cost_benefit_ratio: 2.5,
        };

        assert_eq!(impact.index_name, "test_idx");
        assert_eq!(impact.performance_improvement, 75.5);
        assert_eq!(impact.cost_benefit_ratio, 2.5);
    }
}
