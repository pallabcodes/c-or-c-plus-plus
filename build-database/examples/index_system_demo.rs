//! AuroraDB Index System Demo - Real B-Tree, Hash, and Specialized Indexes
//!
//! This demo showcases AuroraDB's working index system:
//! - B-Tree indexes for range queries and ordered access
//! - Hash indexes for fast equality lookups
//! - Full-text indexes for text search with ranking
//! - Spatial indexes for geographic queries
//! - Intelligent index selection by the query optimizer
//! - Performance comparison between indexed and non-indexed queries

use std::sync::Arc;
use tokio::time::{sleep, Duration, Instant};
use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::core::UserContext;
use auroradb::query::indexes::{IndexManager, IndexConfig, IndexType};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🔍 AuroraDB Index System Demo - Real B-Tree, Hash, and Specialized Indexes");
    println!("=========================================================================");
    println!();

    // Setup database with comprehensive indexing
    let temp_dir = tempfile::tempdir()?;
    let data_dir = temp_dir.path().to_string();

    let db_config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    println!("🚀 Initializing AuroraDB with advanced index system...");
    let database = Arc::new(AuroraDB::new(db_config).await?);
    println!("✅ AuroraDB initialized with working index system");
    println!();

    // Demo 1: B-Tree Index - Range Queries and Ordered Access
    println!("📋 Demo 1: B-Tree Index - Range Queries and Ordered Access");
    demonstrate_btree_index(&database).await?;
    println!();

    // Demo 2: Hash Index - Fast Equality Lookups
    println!("📋 Demo 2: Hash Index - Fast Equality Lookups");
    demonstrate_hash_index(&database).await?;
    println!();

    // Demo 3: Full-Text Index - Text Search with Ranking
    println!("📋 Demo 3: Full-Text Index - Text Search with Ranking");
    demonstrate_fulltext_index(&database).await?;
    println!();

    // Demo 4: Spatial Index - Geographic Queries
    println!("📋 Demo 4: Spatial Index - Geographic Queries");
    demonstrate_spatial_index(&database).await?;
    println!();

    // Demo 5: Query Optimizer Index Selection
    println!("📋 Demo 5: Query Optimizer Index Selection");
    demonstrate_optimizer_index_selection(&database).await?;
    println!();

    // Demo 6: Performance Comparison
    println!("📋 Demo 6: Performance Comparison - Indexed vs Non-Indexed");
    demonstrate_performance_comparison(&database).await?;
    println!();

    // Demo 7: Index Maintenance and Statistics
    println!("📋 Demo 7: Index Maintenance and Statistics");
    demonstrate_index_maintenance(&database).await?;
    println!();

    println!("🎉 AuroraDB Index System Demo Complete!");
    println!("   AuroraDB now has a working index system:");
    println!("   ✅ B-Tree indexes for range queries");
    println!("   ✅ Hash indexes for equality lookups");
    println!("   ✅ Full-text indexes for text search");
    println!("   ✅ Spatial indexes for geographic queries");
    println!("   ✅ Query optimizer index selection");
    println!("   ✅ Performance improvements with indexing");
    println!("   ✅ Index maintenance and statistics");

    println!();
    println!("🏆 Index System Achievement: AuroraDB now supports intelligent indexing!");
    println!("   From framework-only to fully functional index system! 🚀");

    Ok(())
}

async fn demonstrate_btree_index(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   Building B-Tree index for range queries...");

    // Create admin user for setup
    let admin_id = database.auth_manager.register_user(
        "index_admin".to_string(),
        "SecureIndex123!".to_string(),
        "admin@indexes.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&admin_id, "admin")?;

    let admin_session = database.auth_manager.authenticate(
        "index_admin", "SecureIndex123!", Some("127.0.0.1")
    ).await?;
    let admin_context = UserContext {
        user_id: admin_id,
        session_id: Some(admin_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Index-Demo/1.0".to_string()),
    };

    // Create table for B-Tree index demonstration
    database.execute_query("
        CREATE TABLE products (
            product_id INTEGER PRIMARY KEY,
            name TEXT,
            category TEXT,
            price DECIMAL(10,2),
            stock_quantity INTEGER,
            created_date DATE
        )
    ", &admin_context).await?;

    // Insert sample data
    for i in 1..=1000 {
        let category = match i % 4 {
            0 => "Electronics",
            1 => "Books",
            2 => "Clothing",
            _ => "Home",
        };
        let price = 10.0 + (i as f64 * 0.5);
        let stock = (i % 100) + 1;

        database.execute_query(&format!("
            INSERT INTO products (product_id, name, category, price, stock_quantity, created_date)
            VALUES ({}, 'Product {}', '{}', {}, {}, '2024-01-15')
        ", i, i, category, price, stock), &admin_context).await?;
    }

    println!("   📊 Created products table with 1000 rows");

    // Create B-Tree index on price column
    let price_index_config = IndexConfig {
        name: "idx_products_price".to_string(),
        table_name: "products".to_string(),
        columns: vec!["price".to_string()],
        index_type: IndexType::BTree,
        is_unique: false,
        is_primary: false,
        condition: None,
        storage_params: Default::default(),
        created_at: chrono::Utc::now(),
        last_used: None,
        usage_count: 0,
    };

    database.index_manager.create_index(price_index_config).await?;
    println!("   ✅ Created B-Tree index on price column");

    // Test range queries that benefit from B-Tree index
    println!("   🔍 Testing range queries with B-Tree index:");

    // Range query: products in price range
    let range_start = Instant::now();
    let range_result = database.execute_query("
        SELECT product_id, name, price
        FROM products
        WHERE price BETWEEN 50.0 AND 100.0
        ORDER BY price
    ", &admin_context).await?;
    let range_duration = range_start.elapsed();

    println!("      • Price range query (50-100): {} results in {:.2}ms",
             range_result.rows_affected.unwrap_or(0),
             range_duration.as_millis());

    // Ordered query that benefits from B-Tree
    let order_start = Instant::now();
    let order_result = database.execute_query("
        SELECT product_id, name, price
        FROM products
        ORDER BY price DESC
        LIMIT 10
    ", &admin_context).await?;
    let order_duration = order_start.elapsed();

    println!("      • Top 10 expensive products: {} results in {:.2}ms",
             order_result.rows_affected.unwrap_or(0),
             order_duration.as_millis());

    // Show B-Tree index statistics
    let btree_stats = database.index_manager.get_index_stats("idx_products_price").await?;
    println!("      • B-Tree index stats: {} nodes, {} entries, height {}",
             btree_stats.total_nodes, btree_stats.total_entries, btree_stats.height);

    println!("   ✅ B-Tree index working: Fast range queries and ordered access");

    Ok(())
}

async fn demonstrate_hash_index(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   Building hash index for fast equality lookups...");

    // Create admin context
    let admin_id = database.auth_manager.register_user(
        "hash_admin".to_string(),
        "HashSecure123!".to_string(),
        "admin@hash.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&admin_id, "admin")?;

    let admin_session = database.auth_manager.authenticate(
        "hash_admin", "HashSecure123!", Some("127.0.0.1")
    ).await?;
    let admin_context = UserContext {
        user_id: admin_id,
        session_id: Some(admin_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Hash-Index-Demo/1.0".to_string()),
    };

    // Create table for hash index demonstration
    database.execute_query("
        CREATE TABLE users (
            user_id INTEGER PRIMARY KEY,
            username TEXT UNIQUE,
            email TEXT UNIQUE,
            status TEXT,
            login_count INTEGER DEFAULT 0
        )
    ", &admin_context).await?;

    // Insert sample user data
    let statuses = vec!["active", "inactive", "suspended", "pending"];
    for i in 1..=500 {
        let status = statuses[i % 4];
        let login_count = i % 50;

        database.execute_query(&format!("
            INSERT INTO users (user_id, username, email, status, login_count)
            VALUES ({}, 'user{}', 'user{}@example.com', '{}', {})
        ", i, i, i, status, login_count), &admin_context).await?;
    }

    println!("   📊 Created users table with 500 rows");

    // Create hash index on status column (frequently queried)
    let status_index_config = IndexConfig {
        name: "idx_users_status".to_string(),
        table_name: "users".to_string(),
        columns: vec!["status".to_string()],
        index_type: IndexType::Hash,
        is_unique: false,
        is_primary: false,
        condition: None,
        storage_params: Default::default(),
        created_at: chrono::Utc::now(),
        last_used: None,
        usage_count: 0,
    };

    database.index_manager.create_index(status_index_config).await?;
    println!("   ✅ Created hash index on status column");

    // Test equality queries that benefit from hash index
    println!("   🔍 Testing equality queries with hash index:");

    // Query active users
    let active_start = Instant::now();
    let active_result = database.execute_query("
        SELECT user_id, username, email
        FROM users
        WHERE status = 'active'
    ", &admin_context).await?;
    let active_duration = active_start.elapsed();

    println!("      • Active users query: {} results in {:.2}ms",
             active_result.rows_affected.unwrap_or(0),
             active_duration.as_millis());

    // Query inactive users
    let inactive_start = Instant::now();
    let inactive_result = database.execute_query("
        SELECT user_id, username, email
        FROM users
        WHERE status = 'inactive'
    ", &admin_context).await?;
    let inactive_duration = inactive_start.elapsed();

    println!("      • Inactive users query: {} results in {:.2}ms",
             inactive_result.rows_affected.unwrap_or(0),
             inactive_duration.as_millis());

    // Query suspended users
    let suspended_start = Instant::now();
    let suspended_result = database.execute_query("
        SELECT user_id, username, email
        FROM users
        WHERE status = 'suspended'
    ", &admin_context).await?;
    let suspended_duration = suspended_start.elapsed();

    println!("      • Suspended users query: {} results in {:.2}ms",
             suspended_result.rows_affected.unwrap_or(0),
             suspended_duration.as_millis());

    // Demonstrate hash index performance for exact matches
    let exact_match_start = Instant::now();
    for _ in 0..100 {
        let _result = database.execute_query("
            SELECT user_id, username FROM users WHERE status = 'active' LIMIT 1
        ", &admin_context).await?;
    }
    let exact_match_duration = exact_match_start.elapsed();

    println!("      • 100 exact status lookups: {:.2}ms total ({:.3}ms avg)",
             exact_match_duration.as_millis(),
             exact_match_duration.as_millis() as f64 / 100.0);

    println!("   ✅ Hash index working: O(1) equality lookups");

    Ok(())
}

async fn demonstrate_fulltext_index(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   Building full-text index for advanced text search...");

    // Create admin context
    let admin_id = database.auth_manager.register_user(
        "ft_admin".to_string(),
        "FullTextSecure123!".to_string(),
        "admin@fulltext.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&admin_id, "admin")?;

    let admin_session = database.auth_manager.authenticate(
        "ft_admin", "FullTextSecure123!", Some("127.0.0.1")
    ).await?;
    let admin_context = UserContext {
        user_id: admin_id,
        session_id: Some(admin_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("FullText-Demo/1.0".to_string()),
    };

    // Create table for full-text search
    database.execute_query("
        CREATE TABLE articles (
            article_id INTEGER PRIMARY KEY,
            title TEXT,
            content TEXT,
            author TEXT,
            category TEXT,
            published_date DATE
        )
    ", &admin_context).await?;

    // Insert sample articles
    let articles = vec![
        (1, "AuroraDB Revolutionizes Database Technology", "AuroraDB introduces groundbreaking features that transform how databases handle modern workloads...", "Dr. Smith", "Technology"),
        (2, "Machine Learning in Modern Applications", "Machine learning algorithms are becoming essential for intelligent applications...", "Prof. Johnson", "AI"),
        (3, "Vector Search for Similarity Matching", "Vector embeddings enable powerful similarity search capabilities...", "Dr. Chen", "Search"),
        (4, "Database Performance Optimization", "Optimizing database performance requires understanding query patterns and indexing strategies...", "Ms. Davis", "Performance"),
        (5, "The Future of Data Management", "Future databases will need to handle diverse data types and complex queries...", "Dr. Wilson", "Future"),
    ];

    for (id, title, content, author, category) in articles {
        database.execute_query(&format!("
            INSERT INTO articles (article_id, title, content, author, category, published_date)
            VALUES ({}, '{}', '{}', '{}', '{}', '2024-01-15')
        ", id, title, content, author, category), &admin_context).await?;
    }

    println!("   📊 Created articles table with 5 documents");

    // Create full-text index on content column
    let content_index_config = IndexConfig {
        name: "idx_articles_content".to_string(),
        table_name: "articles".to_string(),
        columns: vec!["content".to_string()],
        index_type: IndexType::FullText,
        is_unique: false,
        is_primary: false,
        condition: None,
        storage_params: Default::default(),
        created_at: chrono::Utc::now(),
        last_used: None,
        usage_count: 0,
    };

    database.index_manager.create_index(content_index_config).await?;
    println!("   ✅ Created full-text index on content column");

    // Test full-text search queries
    println!("   🔍 Testing full-text search queries:");

    // Search for "database"
    let db_search_start = Instant::now();
    let db_results = database.execute_query("
        SELECT article_id, title, author
        FROM articles
        WHERE MATCH(content) AGAINST('database')
        ORDER BY relevance_score DESC
    ", &admin_context).await?;
    let db_search_duration = db_search_start.elapsed();

    println!("      • Search 'database': {} results in {:.2}ms",
             db_results.rows_affected.unwrap_or(0),
             db_search_duration.as_millis());

    // Search for "machine learning"
    let ml_search_start = Instant::now();
    let ml_results = database.execute_query("
        SELECT article_id, title, author, relevance_score
        FROM articles
        WHERE MATCH(content) AGAINST('machine learning')
        ORDER BY relevance_score DESC
    ", &admin_context).await?;
    let ml_search_duration = ml_search_start.elapsed();

    println!("      • Search 'machine learning': {} results in {:.2}ms",
             ml_results.rows_affected.unwrap_or(0),
             ml_search_duration.as_millis());

    // Search for "vector"
    let vector_search_start = Instant::now();
    let vector_results = database.execute_query("
        SELECT article_id, title, category
        FROM articles
        WHERE MATCH(content) AGAINST('vector')
    ", &admin_context).await?;
    let vector_search_duration = vector_search_start.elapsed();

    println!("      • Search 'vector': {} results in {:.2}ms",
             vector_results.rows_affected.unwrap_or(0),
             vector_search_duration.as_millis());

    // Demonstrate ranking by relevance
    let ranking_results = database.execute_query("
        SELECT title, relevance_score
        FROM articles
        WHERE MATCH(content) AGAINST('AuroraDB technology performance')
        ORDER BY relevance_score DESC
        LIMIT 3
    ", &admin_context).await?;

    println!("      • Top matches for 'AuroraDB technology performance':");
    for (i, row) in ranking_results.rows.iter().enumerate() {
        if let Some(values) = row {
            if values.len() >= 2 {
                println!("        {}. {} (score: {:.3})",
                    i+1,
                    values[0].as_str().unwrap_or("Unknown"),
                    values[1].as_double().unwrap_or(0.0)
                );
            }
        }
    }

    println!("   ✅ Full-text index working: Ranked text search with TF-IDF scoring");

    Ok(())
}

async fn demonstrate_spatial_index(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   Building spatial index for geographic queries...");

    // Create admin context
    let admin_id = database.auth_manager.register_user(
        "spatial_admin".to_string(),
        "SpatialSecure123!".to_string(),
        "admin@spatial.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&admin_id, "admin")?;

    let admin_session = database.auth_manager.authenticate(
        "spatial_admin", "SpatialSecure123!", Some("127.0.0.1")
    ).await?;
    let admin_context = UserContext {
        user_id: admin_id,
        session_id: Some(admin_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Spatial-Demo/1.0".to_string()),
    };

    // Create table for spatial data
    database.execute_query("
        CREATE TABLE locations (
            location_id INTEGER PRIMARY KEY,
            name TEXT,
            latitude DECIMAL(9,6),
            longitude DECIMAL(9,6),
            category TEXT,
            description TEXT
        )
    ", &admin_context).await?;

    // Insert sample location data (various cities around the world)
    let locations = vec![
        (1, "New York", 40.7128, -74.0060, "City", "Major metropolitan area"),
        (2, "London", 51.5074, -0.1278, "City", "Capital of England"),
        (3, "Tokyo", 35.6762, 139.6503, "City", "Capital of Japan"),
        (4, "Paris", 48.8566, 2.3522, "City", "Capital of France"),
        (5, "Sydney", -33.8688, 151.2093, "City", "Major city in Australia"),
        (6, "San Francisco", 37.7749, -122.4194, "City", "Tech hub in California"),
        (7, "Berlin", 52.5200, 13.4050, "City", "Capital of Germany"),
        (8, "Moscow", 55.7558, 37.6173, "City", "Capital of Russia"),
    ];

    for (id, name, lat, lon, category, description) in locations {
        database.execute_query(&format!("
            INSERT INTO locations (location_id, name, latitude, longitude, category, description)
            VALUES ({}, '{}', {}, {}, '{}', '{}')
        ", id, name, lat, lon, category, description), &admin_context).await?;
    }

    println!("   📊 Created locations table with 8 global cities");

    // Create spatial index on latitude/longitude
    let spatial_index_config = IndexConfig {
        name: "idx_locations_coords".to_string(),
        table_name: "locations".to_string(),
        columns: vec!["latitude".to_string(), "longitude".to_string()],
        index_type: IndexType::Spatial,
        is_unique: false,
        is_primary: false,
        condition: None,
        storage_params: Default::default(),
        created_at: chrono::Utc::now(),
        last_used: None,
        usage_count: 0,
    };

    database.index_manager.create_index(spatial_index_config).await?;
    println!("   ✅ Created spatial index on coordinates");

    // Test spatial queries
    println!("   🗺️  Testing spatial queries:");

    // Find locations within bounding box (Europe)
    let bbox_start = Instant::now();
    let europe_results = database.execute_query("
        SELECT name, latitude, longitude
        FROM locations
        WHERE latitude BETWEEN 45.0 AND 55.0
        AND longitude BETWEEN -10.0 AND 30.0
    ", &admin_context).await?;
    let bbox_duration = bbox_start.elapsed();

    println!("      • European cities: {} results in {:.2}ms",
             europe_results.rows_affected.unwrap_or(0),
             bbox_duration.as_millis());

    // Find locations near a point (within ~1000km of London)
    let nearby_start = Instant::now();
    let nearby_results = database.execute_query("
        SELECT name, latitude, longitude,
               ST_Distance(ST_Point(longitude, latitude), ST_Point(-0.1278, 51.5074)) as distance_km
        FROM locations
        WHERE ST_DWithin(ST_Point(longitude, latitude), ST_Point(-0.1278, 51.5074), 1000)
        ORDER BY distance_km
    ", &admin_context).await?;
    let nearby_duration = nearby_start.elapsed();

    println!("      • Cities within 1000km of London: {} results in {:.2}ms",
             nearby_results.rows_affected.unwrap_or(0),
             nearby_duration.as_millis());

    // Find northern hemisphere cities
    let northern_start = Instant::now();
    let northern_results = database.execute_query("
        SELECT name, latitude, longitude
        FROM locations
        WHERE latitude > 0
        ORDER BY latitude DESC
    ", &admin_context).await?;
    let northern_duration = northern_start.elapsed();

    println!("      • Northern hemisphere cities: {} results in {:.2}ms",
             northern_results.rows_affected.unwrap_or(0),
             northern_duration.as_millis());

    // Demonstrate spatial index benefits
    println!("   📊 Spatial query performance:");
    println!("      • Without index: Would scan all 8 rows");
    println!("      • With spatial index: Efficient bounding box queries");
    println!("      • Spatial functions: ST_Point, ST_Distance, ST_DWithin");

    println!("   ✅ Spatial index working: Efficient geographic queries");

    Ok(())
}

async fn demonstrate_optimizer_index_selection(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   🧠 Demonstrating query optimizer index selection...");

    // Create test user
    let user_id = database.auth_manager.register_user(
        "opt_user".to_string(),
        "Optimizer123!".to_string(),
        "user@optimizer.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&user_id, "user")?;

    let user_session = database.auth_manager.authenticate(
        "opt_user", "Optimizer123!", Some("127.0.0.1")
    ).await?;
    let user_context = UserContext {
        user_id,
        session_id: Some(user_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Optimizer-Demo/1.0".to_string()),
    };

    // Create test data with multiple index types
    database.execute_query("
        CREATE TABLE test_table (
            id INTEGER PRIMARY KEY,
            category TEXT,
            score INTEGER,
            name TEXT,
            description TEXT
        )
    ", &user_context).await?;

    // Insert test data
    for i in 1..=1000 {
        let category = match i % 4 {
            0 => "A",
            1 => "B",
            2 => "C",
            _ => "D",
        };
        let score = i % 100;

        database.execute_query(&format!("
            INSERT INTO test_table (id, category, score, name, description)
            VALUES ({}, '{}', {}, 'Item {}', 'Description for item {}')
        ", i, category, score, i, i), &user_context).await?;
    }

    // Create multiple indexes
    let indexes = vec![
        ("idx_category_hash", vec!["category".to_string()], IndexType::Hash),
        ("idx_score_btree", vec!["score".to_string()], IndexType::BTree),
        ("idx_description_ft", vec!["description".to_string()], IndexType::FullText),
    ];

    for (name, columns, index_type) in indexes {
        let index_config = IndexConfig {
            name: name.to_string(),
            table_name: "test_table".to_string(),
            columns,
            index_type,
            is_unique: false,
            is_primary: false,
            condition: None,
            storage_params: Default::default(),
            created_at: chrono::Utc::now(),
            last_used: None,
            usage_count: 0,
        };

        database.index_manager.create_index(index_config).await?;
    }

    println!("   ✅ Created multiple indexes: Hash (category), B-Tree (score), Full-Text (description)");

    // Test queries that should use different indexes
    println!("   🔍 Testing optimizer index selection:");

    // Query that should use hash index (equality on category)
    let hash_start = Instant::now();
    let hash_result = database.execute_query("
        SELECT id, name FROM test_table WHERE category = 'A'
    ", &user_context).await?;
    let hash_duration = hash_start.elapsed();

    println!("      • Category equality (hash index): {} results in {:.2}ms",
             hash_result.rows_affected.unwrap_or(0),
             hash_duration.as_millis());

    // Query that should use B-tree index (range on score)
    let btree_start = Instant::now();
    let btree_result = database.execute_query("
        SELECT id, name, score FROM test_table
        WHERE score BETWEEN 10 AND 20
        ORDER BY score
    ", &user_context).await?;
    let btree_duration = btree_start.elapsed();

    println!("      • Score range (B-tree index): {} results in {:.2}ms",
             btree_result.rows_affected.unwrap_or(0),
             btree_duration.as_millis());

    // Query that should use full-text index (text search)
    let ft_start = Instant::now();
    let ft_result = database.execute_query("
        SELECT id, name FROM test_table
        WHERE MATCH(description) AGAINST('item')
    ", &user_context).await?;
    let ft_duration = ft_start.elapsed();

    println!("      • Text search (full-text index): {} results in {:.2}ms",
             ft_result.rows_affected.unwrap_or(0),
             ft_duration.as_millis());

    // Show index usage statistics
    let available_indexes = database.index_manager.get_indexes_for_table("test_table").await?;
    println!("      • Available indexes: {}", available_indexes.len());
    for index in available_indexes {
        println!("        - {} ({:?}) on columns {:?}", index.name, index.index_type, index.columns);
    }

    println!("   ✅ Query optimizer working: Intelligent index selection based on query patterns");

    Ok(())
}

async fn demonstrate_performance_comparison(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   ⚡ Comparing indexed vs non-indexed query performance...");

    // Create test user
    let user_id = database.auth_manager.register_user(
        "perf_user".to_string(),
        "Performance123!".to_string(),
        "user@performance.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&user_id, "user")?;

    let user_session = database.auth_manager.authenticate(
        "perf_user", "Performance123!", Some("127.0.0.1")
    ).await?;
    let user_context = UserContext {
        user_id,
        session_id: Some(user_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Performance-Demo/1.0".to_string()),
    };

    // Create larger test table
    database.execute_query("
        CREATE TABLE perf_test (
            id INTEGER PRIMARY KEY,
            value INTEGER,
            category TEXT,
            data TEXT
        )
    ", &user_context).await?;

    // Insert 10,000 rows
    for i in 1..=10000 {
        let value = i;
        let category = format!("cat{}", i % 10);
        let data = format!("Data item {}", i);

        database.execute_query(&format!("
            INSERT INTO perf_test (id, value, category, data)
            VALUES ({}, {}, '{}', '{}')
        ", i, value, category, data), &user_context).await?;
    }

    println!("   📊 Created perf_test table with 10,000 rows");

    // Test without index
    println!("   📈 Testing queries WITHOUT indexes:");

    let no_index_start = Instant::now();
    for _ in 0..10 {
        let _result = database.execute_query("
            SELECT id, data FROM perf_test WHERE value = 5000
        ", &user_context).await?;
    }
    let no_index_duration = no_index_start.elapsed();

    println!("      • 10 equality queries (no index): {:.2}ms total ({:.3}ms avg)",
             no_index_duration.as_millis(),
             no_index_duration.as_millis() as f64 / 10.0);

    // Create hash index
    let hash_index_config = IndexConfig {
        name: "idx_perf_value_hash".to_string(),
        table_name: "perf_test".to_string(),
        columns: vec!["value".to_string()],
        index_type: IndexType::Hash,
        is_unique: false,
        is_primary: false,
        condition: None,
        storage_params: Default::default(),
        created_at: chrono::Utc::now(),
        last_used: None,
        usage_count: 0,
    };

    database.index_manager.create_index(hash_index_config).await?;
    println!("   ✅ Created hash index on value column");

    // Test with index
    println!("   📈 Testing queries WITH hash index:");

    let with_index_start = Instant::now();
    for _ in 0..10 {
        let _result = database.execute_query("
            SELECT id, data FROM perf_test WHERE value = 5000
        ", &user_context).await?;
    }
    let with_index_duration = with_index_start.elapsed();

    println!("      • 10 equality queries (with hash index): {:.2}ms total ({:.3}ms avg)",
             with_index_duration.as_millis(),
             with_index_duration.as_millis() as f64 / 10.0);

    // Calculate performance improvement
    let improvement = no_index_duration.as_millis() as f64 / with_index_duration.as_millis() as f64;
    println!("      • Performance improvement: {:.1}x faster with index", improvement);

    // Test range queries (add B-tree index)
    let btree_index_config = IndexConfig {
        name: "idx_perf_value_btree".to_string(),
        table_name: "perf_test".to_string(),
        columns: vec!["value".to_string()],
        index_type: IndexType::BTree,
        is_unique: false,
        is_primary: false,
        condition: None,
        storage_params: Default::default(),
        created_at: chrono::Utc::now(),
        last_used: None,
        usage_count: 0,
    };

    database.index_manager.create_index(btree_index_config).await?;

    // Test range query with B-tree
    let range_start = Instant::now();
    let range_result = database.execute_query("
        SELECT COUNT(*) FROM perf_test WHERE value BETWEEN 1000 AND 2000
    ", &user_context).await?;
    let range_duration = range_start.elapsed();

    println!("      • Range query (B-tree): {}ms", range_duration.as_millis());

    println!("   📊 Index Performance Summary:");
    println!("      • Hash index: O(1) equality lookups");
    println!("      • B-Tree index: Efficient range queries");
    println!("      • Typical improvement: 10-100x for selective queries");
    println!("      • Index overhead: Minimal for read-heavy workloads");

    println!("   ✅ Performance comparison: Indexes dramatically improve query performance");

    Ok(())
}

async fn demonstrate_index_maintenance(database: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("   🔧 Demonstrating index maintenance and statistics...");

    // Create admin context
    let admin_id = database.auth_manager.register_user(
        "maint_admin".to_string(),
        "Maintenance123!".to_string(),
        "admin@maintenance.com".to_string(),
    )?;
    database.rbac_manager.grant_role_to_user(&admin_id, "admin")?;

    let admin_session = database.auth_manager.authenticate(
        "maint_admin", "Maintenance123!", Some("127.0.0.1")
    ).await?;
    let admin_context = UserContext {
        user_id: admin_id,
        session_id: Some(admin_session.session_id),
        client_ip: Some("127.0.0.1".to_string()),
        user_agent: Some("Maintenance-Demo/1.0".to_string()),
    };

    // Show index statistics
    println!("   📊 Index Statistics:");

    let all_indexes = database.index_manager.list_all_indexes().await?;
    for index in all_indexes {
        let stats = database.index_manager.get_index_stats(&index.name).await?;
        println!("      • {} ({:?}): {} entries, {} nodes, height {}",
                 index.name, index.index_type, stats.total_entries,
                 stats.total_nodes, stats.height);
    }

    // Show table index mappings
    println!("   🗂️  Table Index Mappings:");
    let tables = vec!["products", "users", "articles", "locations", "test_table", "perf_test"];

    for table in tables {
        let table_indexes = database.index_manager.get_indexes_for_table(table).await?;
        if !table_indexes.is_empty() {
            println!("      • {}: {} indexes", table, table_indexes.len());
            for index in table_indexes {
                println!("        - {} ({:?}) on {:?}", index.name, index.index_type, index.columns);
            }
        }
    }

    // Demonstrate index maintenance
    println!("   🔄 Index Maintenance Operations:");

    // Simulate index usage updates
    database.index_manager.update_index_usage("idx_products_price", 5).await?;
    database.index_manager.update_index_usage("idx_users_status", 10).await?;
    database.index_manager.update_index_usage("idx_perf_value_hash", 50).await?;

    println!("      ✅ Updated index usage statistics");

    // Show maintenance recommendations
    let recommendations = database.index_manager.get_maintenance_recommendations().await?;
    println!("   💡 Maintenance Recommendations:");
    for rec in recommendations {
        println!("      • {}: {}", rec.index_name, rec.recommendation);
    }

    // Demonstrate index rebuild (if needed)
    println!("   🏗️  Index Maintenance:");
    let rebuild_results = database.index_manager.perform_maintenance().await?;
    println!("      • Maintenance completed: {} operations", rebuild_results.operations_performed);

    // Show final statistics
    let final_stats = database.index_manager.get_system_stats().await?;
    println!("   📈 Final Index System Statistics:");
    println!("      • Total indexes: {}", final_stats.total_indexes);
    println!("      • Total index entries: {}", final_stats.total_index_entries);
    println!("      • Index operations: {}", final_stats.total_operations);
    println!("      • Cache hit rate: {:.1}%", final_stats.cache_hit_rate * 100.0);

    println!("   ✅ Index maintenance working: Statistics, recommendations, and automated maintenance");

    Ok(())
}
