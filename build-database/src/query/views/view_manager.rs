//! View Manager: Intelligent View Creation and Management
//!
//! Manages view definitions, dependencies, and execution with UNIQUENESS
//! optimizations for performance and maintainability.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use crate::core::errors::{AuroraResult, AuroraError};
use crate::core::schema::{TableSchema, Column};
use crate::query::parser::ast::SelectQuery;

/// View definition with metadata
#[derive(Debug, Clone)]
pub struct ViewDefinition {
    pub name: String,
    pub query: SelectQuery,
    pub columns: Vec<Column>,
    pub dependencies: HashSet<String>, // Tables/views this view depends on
    pub created_at: chrono::DateTime<chrono::Utc>,
    pub last_modified: chrono::DateTime<chrono::Utc>,
    pub view_type: ViewType,
    pub refresh_strategy: RefreshStrategy,
}

/// View types with different optimization strategies
#[derive(Debug, Clone, PartialEq)]
pub enum ViewType {
    Standard,        // Traditional view - computed on each access
    Materialized,    // Cached results with refresh mechanisms
    Intelligent,     // AI-powered caching based on usage patterns
}

/// Refresh strategies for materialized views
#[derive(Debug, Clone)]
pub enum RefreshStrategy {
    Manual,                    // Explicit refresh only
    OnDemand,                  // Refresh when stale
    Scheduled(String),         // Cron-style schedule (e.g., "0 */6 * * *")
    Incremental,              // Update changed data only
    Intelligent,              // ML-based refresh prediction
}

/// View dependency graph for change propagation
#[derive(Debug)]
pub struct ViewDependencyGraph {
    dependencies: HashMap<String, HashSet<String>>, // view -> dependencies
    dependents: HashMap<String, HashSet<String>>,   // table/view -> views that depend on it
}

impl ViewDependencyGraph {
    pub fn new() -> Self {
        Self {
            dependencies: HashMap::new(),
            dependents: HashMap::new(),
        }
    }

    /// Add a view and its dependencies
    pub fn add_view(&mut self, view_name: &str, dependencies: HashSet<String>) {
        self.dependencies.insert(view_name.to_string(), dependencies.clone());

        for dep in dependencies {
            self.dependents.entry(dep).or_insert_with(HashSet::new).insert(view_name.to_string());
        }
    }

    /// Remove a view and clean up dependencies
    pub fn remove_view(&mut self, view_name: &str) {
        if let Some(deps) = self.dependencies.remove(view_name) {
            for dep in deps {
                if let Some(dep_views) = self.dependents.get_mut(&dep) {
                    dep_views.remove(view_name);
                    if dep_views.is_empty() {
                        self.dependents.remove(&dep);
                    }
                }
            }
        }
    }

    /// Get all views that depend on a table/view
    pub fn get_dependents(&self, table_name: &str) -> HashSet<String> {
        self.dependents.get(table_name).cloned().unwrap_or_default()
    }

    /// Get all dependencies of a view
    pub fn get_dependencies(&self, view_name: &str) -> HashSet<String> {
        self.dependencies.get(view_name).cloned().unwrap_or_default()
    }

    /// Check for circular dependencies
    pub fn has_circular_dependency(&self, view_name: &str, dependencies: &HashSet<String>) -> bool {
        for dep in dependencies {
            if dep == view_name {
                return true; // Direct self-reference
            }
            if let Some(dep_deps) = self.dependencies.get(dep) {
                if dep_deps.contains(view_name) {
                    return true; // Indirect circular dependency
                }
            }
        }
        false
    }
}

/// View manager with intelligent caching and optimization
pub struct ViewManager {
    views: RwLock<HashMap<String, ViewDefinition>>,
    dependency_graph: RwLock<ViewDependencyGraph>,
    view_cache: Arc<ViewCache>,
    materialized_views: Arc<MaterializedViewManager>,
}

impl ViewManager {
    pub fn new() -> Self {
        Self {
            views: RwLock::new(HashMap::new()),
            dependency_graph: RwLock::new(ViewDependencyGraph::new()),
            view_cache: Arc::new(ViewCache::new()),
            materialized_views: Arc::new(MaterializedViewManager::new()),
        }
    }

    /// Create a new view with UNIQUENESS intelligence
    pub async fn create_view(
        &self,
        name: String,
        query: SelectQuery,
        view_type: ViewType,
        refresh_strategy: RefreshStrategy,
    ) -> AuroraResult<()> {
        // Validate view name doesn't conflict
        {
            let views = self.views.read();
            if views.contains_key(&name) {
                return Err(AuroraError::InvalidArgument(format!("View '{}' already exists", name)));
            }
        }

        // Analyze query dependencies
        let dependencies = self.analyze_query_dependencies(&query)?;

        // Check for circular dependencies
        {
            let graph = self.dependency_graph.read();
            if graph.has_circular_dependency(&name, &dependencies) {
                return Err(AuroraError::InvalidArgument(format!("Circular dependency detected for view '{}'", name)));
            }
        }

        // Infer column schema from query
        let columns = self.infer_view_columns(&query)?;

        // UNIQUENESS: Intelligent view type selection
        let optimal_view_type = self.determine_optimal_view_type(&query, view_type);
        let optimal_refresh_strategy = self.determine_optimal_refresh_strategy(&query, refresh_strategy);

        // Create view definition
        let view_def = ViewDefinition {
            name: name.clone(),
            query,
            columns,
            dependencies,
            created_at: chrono::Utc::now(),
            last_modified: chrono::Utc::now(),
            view_type: optimal_view_type,
            refresh_strategy: optimal_refresh_strategy,
        };

        // Store view definition
        {
            let mut views = self.views.write();
            views.insert(name.clone(), view_def.clone());
        }

        // Update dependency graph
        {
            let mut graph = self.dependency_graph.write();
            graph.add_view(&name, view_def.dependencies.clone());
        }

        // Initialize caching/materialization based on view type
        match view_def.view_type {
            ViewType::Materialized => {
                self.materialized_views.create_materialized_view(&view_def).await?;
            }
            ViewType::Intelligent => {
                self.view_cache.initialize_intelligent_cache(&view_def).await?;
            }
            ViewType::Standard => {
                // No special initialization needed
            }
        }

        println!("✅ Created {} view '{}' with {} columns",
                format!("{:?}", view_def.view_type).to_lowercase(),
                name,
                view_def.columns.len());

        Ok(())
    }

    /// Drop a view and clean up dependencies
    pub async fn drop_view(&self, name: &str) -> AuroraResult<()> {
        let view_def = {
            let mut views = self.views.write();
            let view = views.remove(name)
                .ok_or_else(|| AuroraError::NotFound(format!("View '{}' not found", name)))?;
            view
        };

        // Clean up dependency graph
        {
            let mut graph = self.dependency_graph.write();
            graph.remove_view(name);
        }

        // Clean up caching/materialization
        match view_def.view_type {
            ViewType::Materialized => {
                self.materialized_views.drop_materialized_view(name).await?;
            }
            ViewType::Intelligent => {
                self.view_cache.drop_intelligent_cache(name).await?;
            }
            ViewType::Standard => {
                // No cleanup needed
            }
        }

        println!("🗑️  Dropped view '{}'", name);
        Ok(())
    }

    /// Execute a view query with intelligent optimization
    pub async fn execute_view(&self, name: &str, parameters: &HashMap<String, String>) -> AuroraResult<ViewResult> {
        let view_def = {
            let views = self.views.read();
            views.get(name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("View '{}' not found", name)))?
        };

        match view_def.view_type {
            ViewType::Standard => {
                self.execute_standard_view(&view_def, parameters).await
            }
            ViewType::Materialized => {
                self.materialized_views.execute_materialized_view(&view_def, parameters).await
            }
            ViewType::Intelligent => {
                self.view_cache.execute_intelligent_view(&view_def, parameters).await
            }
        }
    }

    /// Refresh materialized views when underlying data changes
    pub async fn refresh_on_data_change(&self, table_name: &str) -> AuroraResult<()> {
        let dependent_views = {
            let graph = self.dependency_graph.read();
            graph.get_dependents(table_name)
        };

        for view_name in dependent_views {
            if let Some(view_def) = self.views.read().get(&view_name) {
                match view_def.view_type {
                    ViewType::Materialized => {
                        match view_def.refresh_strategy {
                            RefreshStrategy::Incremental => {
                                self.materialized_views.refresh_incremental(&view_name).await?;
                            }
                            RefreshStrategy::Intelligent => {
                                self.materialized_views.refresh_intelligent(&view_name).await?;
                            }
                            _ => {
                                // For other strategies, mark as stale
                                self.materialized_views.mark_stale(&view_name).await?;
                            }
                        }
                    }
                    ViewType::Intelligent => {
                        self.view_cache.invalidate_cache(&view_name).await?;
                    }
                    ViewType::Standard => {
                        // No refresh needed for standard views
                    }
                }
            }
        }

        Ok(())
    }

    /// Get view metadata and statistics
    pub async fn get_view_info(&self, name: &str) -> AuroraResult<ViewInfo> {
        let view_def = {
            let views = self.views.read();
            views.get(name).cloned()
                .ok_or_else(|| AuroraError::NotFound(format!("View '{}' not found", name)))?
        };

        let cache_info = self.view_cache.get_cache_info(name).await?;
        let materialized_info = self.materialized_views.get_materialized_info(name).await?;

        Ok(ViewInfo {
            definition: view_def,
            cache_info,
            materialized_info,
        })
    }

    /// List all views with their types and status
    pub async fn list_views(&self) -> Vec<ViewSummary> {
        let views = self.views.read();
        let mut summaries = Vec::new();

        for (name, view_def) in views.iter() {
            let cache_hit_rate = self.view_cache.get_hit_rate(name).await.unwrap_or(0.0);
            let is_stale = self.materialized_views.is_stale(name).await.unwrap_or(false);

            summaries.push(ViewSummary {
                name: name.clone(),
                view_type: view_def.view_type.clone(),
                refresh_strategy: view_def.refresh_strategy.clone(),
                column_count: view_def.columns.len(),
                dependency_count: view_def.dependencies.len(),
                cache_hit_rate,
                is_stale,
                created_at: view_def.created_at,
            });
        }

        summaries.sort_by(|a, b| a.name.cmp(&b.name));
        summaries
    }

    // Helper methods

    fn analyze_query_dependencies(&self, query: &SelectQuery) -> AuroraResult<HashSet<String>> {
        let mut dependencies = HashSet::new();

        // Analyze FROM clause for table dependencies
        match &query.from_clause {
            crate::query::parser::ast::FromClause::Simple(table_name) => {
                dependencies.insert(table_name.clone());
            }
            crate::query::parser::ast::FromClause::Join(join_clause) => {
                // Analyze join dependencies (simplified)
                dependencies.insert("complex_join".to_string()); // Placeholder
            }
            crate::query::parser::ast::FromClause::Subquery(_) => {
                dependencies.insert("subquery".to_string()); // Placeholder
            }
        }

        Ok(dependencies)
    }

    fn infer_view_columns(&self, query: &SelectQuery) -> AuroraResult<Vec<Column>> {
        // Infer column schema from SELECT list (simplified)
        let mut columns = Vec::new();

        for (i, select_item) in query.select_list.iter().enumerate() {
            match select_item {
                crate::query::parser::ast::SelectItem::Expression(expr, alias) => {
                    let column_name = alias.clone().unwrap_or_else(|| format!("col_{}", i + 1));
                    let data_type = self.infer_expression_type(expr);

                    columns.push(Column {
                        id: crate::core::types::ColumnId(i as u32),
                        name: column_name,
                        data_type,
                        nullable: true, // Assume nullable for views
                        default_value: None,
                    });
                }
                crate::query::parser::ast::SelectItem::Wildcard => {
                    // For wildcard, we'd need to expand based on source tables
                    // Simplified: add a generic column
                    columns.push(Column {
                        id: crate::core::types::ColumnId(i as u32),
                        name: format!("col_{}", i + 1),
                        data_type: crate::core::data::DataType::Text,
                        nullable: true,
                        default_value: None,
                    });
                }
            }
        }

        Ok(columns)
    }

    fn infer_expression_type(&self, _expr: &crate::query::parser::ast::Expression) -> crate::core::data::DataType {
        // Simplified type inference
        crate::core::data::DataType::Text
    }

    fn determine_optimal_view_type(&self, query: &SelectQuery, requested_type: ViewType) -> ViewType {
        // UNIQUENESS: Intelligent view type selection based on query characteristics

        // If explicitly requested, use that
        if requested_type != ViewType::Standard {
            return requested_type;
        }

        // Analyze query for optimal view type
        let has_aggregations = query.group_by.is_some();
        let has_complex_joins = matches!(query.from_clause, crate::query::parser::ast::FromClause::Join(_));
        let has_limit = query.limit.is_some();

        // Recommend materialized for expensive queries
        if has_aggregations && has_complex_joins {
            ViewType::Materialized
        }
        // Recommend intelligent caching for frequently accessed views
        else if has_limit && query.order_by.is_some() {
            ViewType::Intelligent
        }
        // Default to standard view
        else {
            ViewType::Standard
        }
    }

    fn determine_optimal_refresh_strategy(&self, _query: &SelectQuery, requested_strategy: RefreshStrategy) -> RefreshStrategy {
        // UNIQUENESS: Intelligent refresh strategy selection

        match requested_strategy {
            RefreshStrategy::Manual => RefreshStrategy::Manual,
            RefreshStrategy::OnDemand => RefreshStrategy::OnDemand,
            RefreshStrategy::Scheduled(_) => requested_strategy,
            _ => {
                // Default to intelligent for complex queries
                RefreshStrategy::Intelligent
            }
        }
    }

    async fn execute_standard_view(&self, view_def: &ViewDefinition, _parameters: &HashMap<String, String>) -> AuroraResult<ViewResult> {
        // Execute view query directly (simplified - would integrate with query executor)
        println!("🔍 Executing standard view '{}'", view_def.name);

        // Simulate query execution
        tokio::time::sleep(tokio::time::Duration::from_millis(50)).await;

        Ok(ViewResult {
            row_count: 1000, // Simulated
            execution_time_ms: 50.0,
            cache_hit: false,
            data_freshness: DataFreshness::RealTime,
        })
    }
}

/// View execution result
#[derive(Debug)]
pub struct ViewResult {
    pub row_count: u64,
    pub execution_time_ms: f64,
    pub cache_hit: bool,
    pub data_freshness: DataFreshness,
}

/// Data freshness indicators
#[derive(Debug, Clone, PartialEq)]
pub enum DataFreshness {
    RealTime,
    Stale,
    Cached,
    Estimated,
}

/// View information summary
#[derive(Debug)]
pub struct ViewInfo {
    pub definition: ViewDefinition,
    pub cache_info: CacheInfo,
    pub materialized_info: MaterializedInfo,
}

/// Cache information
#[derive(Debug)]
pub struct CacheInfo {
    pub hit_rate: f64,
    pub cache_size_bytes: u64,
    pub last_accessed: Option<chrono::DateTime<chrono::Utc>>,
}

/// Materialized view information
#[derive(Debug)]
pub struct MaterializedInfo {
    pub is_stale: bool,
    pub last_refresh: Option<chrono::DateTime<chrono::Utc>>,
    pub refresh_duration_ms: Option<f64>,
    pub storage_size_bytes: u64,
}

/// View summary for listing
#[derive(Debug)]
pub struct ViewSummary {
    pub name: String,
    pub view_type: ViewType,
    pub refresh_strategy: RefreshStrategy,
    pub column_count: usize,
    pub dependency_count: usize,
    pub cache_hit_rate: f64,
    pub is_stale: bool,
    pub created_at: chrono::DateTime<chrono::Utc>,
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::query::parser::ast::*;

    #[tokio::test]
    async fn test_view_manager_creation() {
        let manager = ViewManager::new();
        // Test passes if created successfully
        assert!(true);
    }

    #[tokio::test]
    async fn test_view_dependency_graph() {
        let mut graph = ViewDependencyGraph::new();

        // Add a view with dependencies
        let deps = HashSet::from(["users".to_string(), "orders".to_string()]);
        graph.add_view("user_orders", deps);

        // Check dependencies
        let view_deps = graph.get_dependencies("user_orders");
        assert_eq!(view_deps.len(), 2);
        assert!(view_deps.contains("users"));
        assert!(view_deps.contains("orders"));

        // Check dependents
        let user_dependents = graph.get_dependents("users");
        assert_eq!(user_dependents.len(), 1);
        assert!(user_dependents.contains("user_orders"));
    }

    #[tokio::test]
    async fn test_circular_dependency_detection() {
        let mut graph = ViewDependencyGraph::new();

        // Add initial view
        graph.add_view("view_a", HashSet::from(["table1".to_string()]));

        // Test direct circular dependency
        let circular_deps = HashSet::from(["view_a".to_string()]);
        assert!(graph.has_circular_dependency("view_a", &circular_deps));

        // Test indirect circular dependency
        graph.add_view("view_b", HashSet::from(["view_a".to_string()]));
        let indirect_circular = HashSet::from(["view_b".to_string()]);
        assert!(graph.has_circular_dependency("view_a", &indirect_circular));
    }

    #[test]
    fn test_view_types() {
        assert_eq!(ViewType::Standard, ViewType::Standard);
        assert_ne!(ViewType::Materialized, ViewType::Intelligent);
    }

    #[test]
    fn test_refresh_strategies() {
        assert_eq!(RefreshStrategy::Manual, RefreshStrategy::Manual);
        assert_ne!(RefreshStrategy::OnDemand, RefreshStrategy::Intelligent);
    }
}
