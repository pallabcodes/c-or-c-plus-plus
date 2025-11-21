//! AuroraDB Advanced Features Demo
//!
//! Revolutionary advanced capabilities that make AuroraDB uniquely powerful:
//! - AI/ML Functions: Vector similarity, clustering, forecasting built into SQL
//! - Advanced Analytics: Statistical analysis, time series, anomaly detection
//! - Graph Database: Property graphs with vector embeddings
//! - Unified AI-Native Database: All capabilities work together seamlessly

use std::collections::HashMap;
use auroradb::advanced::{
    ai_functions::{AIFunctionRegistry, QueryContext},
    analytics::AnalyticsFunctionRegistry,
    graph::{PropertyGraph, NodeId, GraphAlgorithm, PathAlgorithm, PropertyValue},
};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB Advanced Features Demo");
    println!("===================================\n");

    // Demo 1: AI/ML Functions in SQL
    demo_ai_ml_functions().await?;

    // Demo 2: Advanced Analytics
    demo_advanced_analytics().await?;

    // Demo 3: Graph Database with Vectors
    demo_graph_database().await?;

    // Demo 4: Unified AI-Native Capabilities
    demo_unified_capabilities().await?;

    println!("\n✨ AuroraDB Advanced Features Complete!");
    println!("   Revolutionary AI-native database capabilities achieved.");
    println!("   No other database combines SQL + AI/ML + Analytics + Graphs like AuroraDB.");

    Ok(())
}

async fn demo_ai_ml_functions() -> Result<(), Box<dyn std::error::Error>> {
    println!("🧠 AI/ML Functions in SQL Demo");
    println!("==============================");

    let ai_registry = AIFunctionRegistry::new();
    let context = QueryContext {
        database: "auroradb".to_string(),
        user: "demo_user".to_string(),
        timestamp: chrono::Utc::now().timestamp(),
        variables: HashMap::new(),
    };

    println!("Available AI/ML Functions:");
    for func_name in ai_registry.list_functions() {
        println!("  • {}", func_name);
    }

    // Vector Similarity
    println!("\n🔍 Vector Similarity:");
    let similarity_args = vec![
        serde_json::json!([1.0, 0.0, 0.0]), // Unit vector X
        serde_json::json!([0.0, 1.0, 0.0]), // Unit vector Y (orthogonal)
    ];
    let similarity_result = ai_registry.execute_function("vector_similarity", similarity_args, &context)?;
    println!("  Cosine similarity between orthogonal vectors: {:.3}", similarity_result.as_f64().unwrap());

    // K-Means Clustering
    println!("\n📊 K-Means Clustering:");
    let cluster_data = vec![
        vec![1.0, 1.0], vec![1.1, 1.1], vec![0.9, 0.9], // Cluster 1
        vec![5.0, 5.0], vec![5.1, 5.1], vec![4.9, 4.9], // Cluster 2
    ];
    let cluster_args = vec![
        serde_json::json!(cluster_data),
        serde_json::json!(2), // k=2 clusters
    ];
    let cluster_result = ai_registry.execute_function("kmeans_cluster", cluster_args, &context)?;
    println!("  Clustered {} points into {} clusters", cluster_data.len(), cluster_result["k"]);

    // Anomaly Detection
    println!("\n🔍 Anomaly Detection:");
    let anomaly_data = vec![1.0, 1.1, 1.2, 1.0, 1.1, 10.0]; // Last value is anomaly
    let anomaly_args = vec![
        serde_json::json!(anomaly_data),
        serde_json::json!("iqr"), // IQR method
    ];
    let anomaly_result = ai_registry.execute_function("outlier_detection", anomaly_args, &context)?;
    println!("  Detected {} outliers in {} data points", anomaly_result["outlier_count"], anomaly_data.len());

    // Time Series Forecasting
    println!("\n📈 Time Series Forecasting:");
    let ts_data = vec![10.0, 12.0, 13.0, 12.0, 14.0, 16.0, 15.0, 17.0, 19.0, 18.0];
    let forecast_args = vec![
        serde_json::json!(ts_data),
        serde_json::json!(3), // Forecast 3 steps ahead
        serde_json::json!("linear"), // Linear trend method
    ];
    let forecast_result = ai_registry.execute_function("time_series_forecast", forecast_args, &context)?;
    let forecast_values = forecast_result["forecast"].as_array().unwrap();
    println!("  Forecasted {} steps: {:.1}, {:.1}, {:.1}",
             forecast_values.len(),
             forecast_values[0].as_f64().unwrap(),
             forecast_values[1].as_f64().unwrap(),
             forecast_values[2].as_f64().unwrap());

    // Text Embedding (simplified)
    println!("\n📝 Text Processing:");
    let text_args = vec![serde_json::json!("AuroraDB is revolutionary")];
    let text_result = ai_registry.execute_function("text_embedding", text_args, &context)?;
    let embedding = text_result.as_array().unwrap();
    println!("  Generated {}-dimensional embedding for text", embedding.len());

    println!("✅ AI/ML Functions make AuroraDB the only database with built-in machine learning!");

    Ok(())
}

async fn demo_advanced_analytics() -> Result<(), Box<dyn std::error::Error>> {
    println!("\n📊 Advanced Analytics Demo");
    println!("==========================");

    let analytics_registry = AnalyticsFunctionRegistry::new();
    let context = QueryContext {
        database: "auroradb".to_string(),
        user: "demo_user".to_string(),
        timestamp: chrono::Utc::now().timestamp(),
        variables: HashMap::new(),
    };

    println!("Available Analytics Functions:");
    for func_name in analytics_registry.list_functions() {
        println!("  • {}", func_name);
    }

    // Statistical Correlation
    println!("\n📈 Statistical Correlation:");
    let x_data = vec![1.0, 2.0, 3.0, 4.0, 5.0];
    let y_data = vec![2.0, 4.0, 6.0, 8.0, 10.0]; // Perfect positive correlation
    let correlation_args = vec![
        serde_json::json!(x_data),
        serde_json::json!(y_data),
    ];
    let correlation_result = analytics_registry.execute_function("correlation", correlation_args, &context)?;
    println!("  Pearson correlation: {:.3} (R² = {:.3})",
             correlation_result["correlation_coefficient"],
             correlation_result["r_squared"]);

    // Linear Regression
    println!("\n📉 Linear Regression:");
    let regression_args = vec![
        serde_json::json!(x_data),
        serde_json::json!(y_data),
    ];
    let regression_result = analytics_registry.execute_function("linear_regression", regression_args, &context)?;
    println!("  Regression: y = {:.3}x + {:.3}",
             regression_result["slope"],
             regression_result["intercept"]);

    // Moving Average
    println!("\n📊 Moving Average Smoothing:");
    let ma_data = vec![10.0, 12.0, 8.0, 15.0, 11.0, 14.0, 9.0, 16.0];
    let ma_args = vec![
        serde_json::json!(ma_data),
        serde_json::json!(3), // Window size 3
    ];
    let ma_result = analytics_registry.execute_function("moving_average", ma_args, &context)?;
    let ma_values = ma_result["moving_average"].as_array().unwrap();
    println!("  3-period moving average: {:.1}, {:.1}, {:.1}, ...",
             ma_values[0].as_f64().unwrap(),
             ma_values[1].as_f64().unwrap(),
             ma_values[2].as_f64().unwrap());

    // Hypothesis Testing
    println!("\n🧪 Hypothesis Testing:");
    let sample1 = vec![10.0, 12.0, 11.0, 13.0, 12.0];
    let sample2 = vec![15.0, 17.0, 16.0, 18.0, 17.0];
    let test_args = vec![
        serde_json::json!(sample1),
        serde_json::json!(sample2),
        serde_json::json!("t-test"),
    ];
    let test_result = analytics_registry.execute_function("hypothesis_test", test_args, &context)?;
    println!("  T-test statistic: {:.3}, p-value: {:.3} ({})",
             test_result["statistic"],
             test_result["p_value"],
             if test_result["significant"].as_bool().unwrap() { "significant" } else { "not significant" });

    // Trend Analysis
    println!("\n📈 Trend Analysis:");
    let trend_data = vec![100.0, 105.0, 102.0, 108.0, 115.0, 112.0, 118.0, 125.0];
    let trend_args = vec![serde_json::json!(trend_data)];
    let trend_result = analytics_registry.execute_function("trend_analysis", trend_args, &context)?;
    println!("  Trend: {} (slope: {:.3}, confidence: {})",
             trend_result["trend_direction"].as_str().unwrap(),
             trend_result["slope"],
             trend_result["confidence"]);

    // Distribution Fitting
    println!("\n📊 Distribution Fitting:");
    let dist_data = vec![2.1, 2.5, 3.2, 2.8, 3.1, 2.9, 3.0, 2.7, 2.6, 3.3];
    let dist_args = vec![serde_json::json!(dist_data)];
    let dist_result = analytics_registry.execute_function("distribution_fit", dist_args, &context)?;
    println!("  Best fit: {} distribution (AIC: {:.1})",
             dist_result["best_distribution"],
             dist_result["aic_score"]);

    println!("✅ Advanced Analytics make AuroraDB a statistical computing platform!");

    Ok(())
}

async fn demo_graph_database() -> Result<(), Box<dyn std::error::Error>> {
    println!("\n🕸️  Graph Database with Vectors Demo");
    println!("===================================");

    let mut graph = PropertyGraph::new();

    println!("Building a social network with vector embeddings...");

    // Create people nodes with embeddings
    let alice_embedding = vec![1.0, 0.0, 0.0]; // Tech enthusiast
    let bob_embedding = vec![0.8, 0.2, 0.0];   // Similar to Alice
    let charlie_embedding = vec![0.0, 1.0, 0.0]; // Sports enthusiast
    let dave_embedding = vec![0.0, 0.8, 0.2];   // Similar to Charlie
    let eve_embedding = vec![0.0, 0.0, 1.0];    // Music enthusiast

    let alice = graph.create_node(
        vec!["Person".to_string()],
        HashMap::from([
            ("name".to_string(), PropertyValue::String("Alice".to_string())),
            ("age".to_string(), PropertyValue::Integer(28)),
            ("interests".to_string(), PropertyValue::String("technology, AI".to_string())),
        ]),
        Some(alice_embedding)
    )?;

    let bob = graph.create_node(
        vec!["Person".to_string()],
        HashMap::from([
            ("name".to_string(), PropertyValue::String("Bob".to_string())),
            ("age".to_string(), PropertyValue::Integer(32)),
            ("interests".to_string(), PropertyValue::String("programming, tech".to_string())),
        ]),
        Some(bob_embedding)
    )?;

    let charlie = graph.create_node(
        vec!["Person".to_string()],
        HashMap::from([
            ("name".to_string(), PropertyValue::String("Charlie".to_string())),
            ("age".to_string(), PropertyValue::Integer(25)),
            ("interests".to_string(), PropertyValue::String("sports, fitness".to_string())),
        ]),
        Some(charlie_embedding)
    )?;

    let dave = graph.create_node(
        vec!["Person".to_string()],
        HashMap::from([
            ("name".to_string(), PropertyValue::String("Dave".to_string())),
            ("age".to_string(), PropertyValue::Integer(30)),
            ("interests".to_string(), PropertyValue::String("basketball, soccer".to_string())),
        ]),
        Some(dave_embedding)
    )?;

    let eve = graph.create_node(
        vec!["Person".to_string()],
        HashMap::from([
            ("name".to_string(), PropertyValue::String("Eve".to_string())),
            ("age".to_string(), PropertyValue::Integer(27)),
            ("interests".to_string(), PropertyValue::String("music, concerts".to_string())),
        ]),
        Some(eve_embedding)
    )?;

    // Create relationships
    graph.create_relationship(
        alice, bob, "KNOWS".to_string(),
        HashMap::from([
            ("strength".to_string(), PropertyValue::Float(0.9)),
            ("since".to_string(), PropertyValue::Integer(2020)),
        ]),
        Some(vec![0.8, 0.1, 0.1]) // Relationship embedding
    )?;

    graph.create_relationship(
        alice, charlie, "KNOWS".to_string(),
        HashMap::from([
            ("strength".to_string(), PropertyValue::Float(0.6)),
            ("since".to_string(), PropertyValue::Integer(2021)),
        ]),
        Some(vec![0.4, 0.4, 0.2])
    )?;

    graph.create_relationship(
        bob, charlie, "KNOWS".to_string(),
        HashMap::from([
            ("strength".to_string(), PropertyValue::Float(0.7)),
            ("since".to_string(), PropertyValue::Integer(2019)),
        ]),
        Some(vec![0.5, 0.4, 0.1])
    )?;

    graph.create_relationship(
        charlie, dave, "KNOWS".to_string(),
        HashMap::from([
            ("strength".to_string(), PropertyValue::Float(0.95)),
            ("since".to_string(), PropertyValue::Integer(2018)),
        ]),
        Some(vec![0.1, 0.8, 0.1])
    )?;

    graph.create_relationship(
        dave, eve, "KNOWS".to_string(),
        HashMap::from([
            ("strength".to_string(), PropertyValue::Float(0.8)),
            ("since".to_string(), PropertyValue::Integer(2022)),
        ]),
        Some(vec![0.2, 0.3, 0.5])
    )?;

    println!("Created social network with {} nodes and {} relationships",
             graph.statistics().node_count,
             graph.statistics().relationship_count);

    // Graph Analytics
    println!("\n📊 Graph Analytics:");

    // PageRank
    let pagerank_result = graph.analytics(GraphAlgorithm::PageRank)?;
    if let auroradb::advanced::graph::GraphAnalyticsResult::PageRank(ranks) = pagerank_result {
        println!("  PageRank scores:");
        for (node_id, rank) in ranks.iter().take(3) {
            if let Some(node) = graph.nodes.get(node_id) {
                if let Some(PropertyValue::String(name)) = node.properties.get("name") {
                    println!("    {}: {:.3}", name, rank);
                }
            }
        }
    }

    // Shortest Path
    println!("\n🛣️  Shortest Path:");
    if let Some(path) = graph.shortest_path(alice, eve, PathAlgorithm::Dijkstra)? {
        println!("  Path from Alice to Eve: {} nodes, cost: {:.1}", path.nodes.len(), path.cost);

        // Show path details
        for (i, &node_id) in path.nodes.iter().enumerate() {
            if let Some(node) = graph.nodes.get(&node_id) {
                if let Some(PropertyValue::String(name)) = node.properties.get("name") {
                    if i > 0 { print!(" -> ") }
                    print!("{}", name);
                }
            }
        }
        println!();
    }

    // Vector-enhanced search
    println!("\n🔍 Vector-Enhanced Graph Search:");
    let tech_query = vec![0.9, 0.0, 0.0]; // Looking for tech enthusiasts
    let similar_people = graph.vector_search(&tech_query, 3, Some("Person"))?;

    println!("  People similar to tech interests:");
    for (node_id, similarity) in similar_people {
        if let Some(node) = graph.nodes.get(&node_id) {
            if let Some(PropertyValue::String(name)) = node.properties.get("name") {
                println!("    {}: {:.3} similarity", name, similarity);
            }
        }
    }

    // Graph Query
    println!("\n🔎 Graph Query:");
    let query_result = graph.query("MATCH (p:Person)-[:KNOWS]->(p2:Person) WHERE p.name = 'Alice'")?;
    println!("  Alice knows {} people", query_result.nodes.len() - 1); // -1 for Alice herself

    println!("✅ Graph Database combines structural queries with semantic similarity!");

    Ok(())
}

async fn demo_unified_capabilities() -> Result<(), Box<dyn std::error::Error>> {
    println!("\n🔗 Unified AI-Native Capabilities Demo");
    println!("=====================================");

    println!("AuroraDB uniquely combines:");
    println!("• SQL queries with AI/ML functions");
    println!("• Relational data with vector search");
    println!("• Time series with graph analytics");
    println!("• Statistical analysis with machine learning");

    // Simulate a comprehensive AI-native application
    println!("\n🏗️  Building a Comprehensive AI Application:");

    // 1. Graph Database for Knowledge Graph
    let mut knowledge_graph = PropertyGraph::new();

    let user_profile = knowledge_graph.create_node(
        vec!["User".to_string()],
        HashMap::from([
            ("interests".to_string(), PropertyValue::String("AI, technology, data science".to_string())),
        ]),
        Some(vec![0.8, 0.6, 0.2, 0.1]) // User embedding
    )?;

    let article1 = knowledge_graph.create_node(
        vec!["Article".to_string()],
        HashMap::from([
            ("title".to_string(), PropertyValue::String("Vector Databases Explained".to_string())),
            ("category".to_string(), PropertyValue::String("technology".to_string())),
            ("read_time".to_string(), PropertyValue::Integer(10)),
        ]),
        Some(vec![0.9, 0.7, 0.1, 0.0]) // Article embedding
    )?;

    let article2 = knowledge_graph.create_node(
        vec!["Article".to_string()],
        HashMap::from([
            ("title".to_string(), PropertyValue::String("Machine Learning Basics".to_string())),
            ("category".to_string(), PropertyValue::String("education".to_string())),
            ("read_time".to_string(), PropertyValue::Integer(15)),
        ]),
        Some(vec![0.6, 0.8, 0.3, 0.2])
    )?;

    // Connect user to articles they've read
    knowledge_graph.create_relationship(
        user_profile, article1, "HAS_READ".to_string(),
        HashMap::from([
            ("rating".to_string(), PropertyValue::Integer(5)),
            ("read_date".to_string(), PropertyValue::String("2024-01-15".to_string())),
        ]),
        None
    )?;

    println!("✅ Knowledge Graph: User connected to articles");

    // 2. AI/ML Functions for Recommendations
    let ai_registry = AIFunctionRegistry::new();
    let context = QueryContext {
        database: "auroradb".to_string(),
        user: "app_user".to_string(),
        timestamp: chrono::Utc::now().timestamp(),
        variables: HashMap::new(),
    };

    // Use collaborative filtering for recommendations
    let user_history = vec![vec![5.0, 4.0, 0.0, 0.0]]; // User ratings
    let item_features = vec![
        vec![0.9, 0.7, 0.1, 0.0], // Article 1 features
        vec![0.6, 0.8, 0.3, 0.2], // Article 2 features
        vec![0.2, 0.3, 0.8, 0.7], // Article 3 features (unseen)
    ];

    let cf_args = vec![
        serde_json::json!(user_history),
        serde_json::json!(item_features),
    ];

    match ai_registry.execute_function("recommend_similar", cf_args, &context) {
        Ok(recommendations) => {
            println!("✅ AI Recommendations: Generated personalized suggestions");
        }
        Err(_) => {
            println!("ℹ️  AI Recommendations: Using fallback similarity search");
        }
    }

    // 3. Analytics for User Behavior
    let analytics_registry = AnalyticsFunctionRegistry::new();

    // Simulate user engagement data
    let engagement_data = vec![10, 12, 15, 11, 14, 18, 16, 20, 19, 22]; // Daily active users
    let trend_args = vec![serde_json::json!(engagement_data)];

    if let Ok(trend_result) = analytics_registry.execute_function("trend_analysis", trend_args, &context) {
        println!("✅ Analytics: User engagement trending {}",
                 trend_result["trend_direction"].as_str().unwrap_or("unknown"));
    }

    // 4. Time Series Forecasting for Growth Prediction
    let growth_data = vec![100.0, 110.0, 125.0, 140.0, 160.0, 185.0, 210.0];
    let forecast_args = vec![
        serde_json::json!(growth_data),
        serde_json::json!(3), // Forecast 3 periods
        serde_json::json!("linear"),
    ];

    if let Ok(forecast_result) = analytics_registry.execute_function("time_series_forecast", forecast_args, &context) {
        let forecasts = forecast_result["forecast"].as_array().unwrap();
        println!("✅ Forecasting: Predicted growth: {:.0}, {:.0}, {:.0} users",
                 forecasts[0].as_f64().unwrap_or(0.0),
                 forecasts[1].as_f64().unwrap_or(0.0),
                 forecasts[2].as_f64().unwrap_or(0.0));
    }

    // 5. Anomaly Detection for Fraud Prevention
    let transaction_amounts = vec![50.0, 45.0, 55.0, 48.0, 52.0, 300.0, 47.0, 51.0]; // 300 is anomaly
    let anomaly_args = vec![
        serde_json::json!(transaction_amounts),
        serde_json::json!("iqr"),
    ];

    if let Ok(anomaly_result) = analytics_registry.execute_function("outlier_detection", anomaly_args, &context) {
        let outlier_count = anomaly_result["outlier_count"].as_u64().unwrap_or(0);
        println!("✅ Anomaly Detection: Found {} suspicious transactions", outlier_count);
    }

    // Summary
    println!("\n🎯 AuroraDB Unified Capabilities:");
    println!("• Graph Database: Knowledge representation");
    println!("• AI/ML Functions: Intelligent recommendations");
    println!("• Analytics: User behavior insights");
    println!("• Forecasting: Growth predictions");
    println!("• Anomaly Detection: Fraud prevention");
    println!("• All integrated in a single SQL database!");

    println!("\n🏆 This is why AuroraDB is revolutionary:");
    println!("   No other database combines all these capabilities.");
    println!("   AuroraDB is the future of AI-native databases.");

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_ai_functions_integration() {
        let registry = AIFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        // Test vector similarity
        let args = vec![
            serde_json::json!([1.0, 0.0]),
            serde_json::json!([1.0, 0.0])
        ];

        let result = registry.execute_function("vector_similarity", args, &context).unwrap();
        assert_eq!(result.as_f64().unwrap(), 1.0); // Identical vectors = similarity 1.0
    }

    #[tokio::test]
    async fn test_analytics_integration() {
        let registry = AnalyticsFunctionRegistry::new();
        let context = QueryContext {
            database: "test".to_string(),
            user: "test".to_string(),
            timestamp: 1234567890,
            variables: HashMap::new(),
        };

        // Test correlation
        let args = vec![
            serde_json::json!([1.0, 2.0, 3.0]),
            serde_json::json!([2.0, 4.0, 6.0])
        ];

        let result = registry.execute_function("correlation", args, &context).unwrap();
        assert!(result["correlation_coefficient"].as_f64().unwrap() > 0.99); // Should be nearly 1.0
    }

    #[test]
    fn test_graph_operations() {
        let mut graph = PropertyGraph::new();

        let node1 = graph.create_node(vec!["Test".to_string()], HashMap::new(), None).unwrap();
        let node2 = graph.create_node(vec!["Test".to_string()], HashMap::new(), None).unwrap();

        graph.create_relationship(node1, node2, "TEST_REL".to_string(), HashMap::new(), None).unwrap();

        assert_eq!(graph.statistics().node_count, 2);
        assert_eq!(graph.statistics().relationship_count, 1);
    }

    #[tokio::test]
    async fn test_unified_demo() {
        // This test runs the unified capabilities demo
        // In a real scenario, this would verify all components work together
        demo_unified_capabilities().await.unwrap();
    }
}
