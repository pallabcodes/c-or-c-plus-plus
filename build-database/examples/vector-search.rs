//! AuroraDB Vector Search Example
//!
//! Demonstrates AuroraDB's UNIQUENESS vector search capabilities
//! with similarity search and embedding-based queries.

use aurora_db::{AuroraDB, ConnectionConfig, VectorResult};
use std::time::Instant;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB Vector Search Example");
    println!("==================================");

    // Connect to AuroraDB
    let config = ConnectionConfig {
        host: "localhost".to_string(),
        port: 5432,
        user: "aurora".to_string(),
        password: Some("aurora".to_string()),
        database: "aurora".to_string(),
        ..Default::default()
    };

    let db = AuroraDB::new(config).await?;
    println!("✅ Connected to AuroraDB");

    // Create products table with vector embeddings
    setup_products_table(&db).await?;

    // Insert sample products with embeddings
    insert_sample_products(&db).await?;

    // Perform vector similarity search
    perform_similarity_search(&db).await?;

    // Advanced vector operations
    perform_advanced_vector_ops(&db).await?;

    // Performance demonstration
    demonstrate_performance(&db).await?;

    println!("🎉 Vector search example completed!");
    Ok(())
}

async fn setup_products_table(db: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n📋 Setting up products table...");

    let create_table_sql = r#"
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY,
            name VARCHAR(255) NOT NULL,
            category VARCHAR(100),
            price DECIMAL(10,2),
            description TEXT,
            embedding VECTOR(384),  -- 384-dimensional embedding
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    "#;

    db.execute_query(create_table_sql).await?;
    println!("✅ Products table created with vector support");

    // Create vector index for performance
    let create_index_sql = r#"
        CREATE INDEX IF NOT EXISTS products_embedding_idx
        ON products USING hnsw (embedding)
        WITH (ef_construction = 200, m = 16)
    "#;

    db.execute_query(create_index_sql).await?;
    println!("✅ HNSW vector index created");

    Ok(())
}

async fn insert_sample_products(db: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n📦 Inserting sample products...");

    let products = vec![
        (1, "Wireless Bluetooth Headphones", "Electronics", 199.99, "Premium noise-cancelling wireless headphones", generate_embedding("headphones")),
        (2, "Ergonomic Office Chair", "Furniture", 349.99, "Adjustable ergonomic chair for long work sessions", generate_embedding("chair")),
        (3, "Smart Fitness Watch", "Electronics", 299.99, "Advanced fitness tracking with heart rate monitor", generate_embedding("watch")),
        (4, "Organic Coffee Beans", "Food", 24.99, "Premium arabica coffee beans from Ethiopia", generate_embedding("coffee")),
        (5, "Yoga Mat", "Sports", 49.99, "Non-slip yoga mat with carrying strap", generate_embedding("yoga")),
        (6, "Wireless Gaming Mouse", "Electronics", 79.99, "High-precision gaming mouse with RGB lighting", generate_embedding("mouse")),
        (7, "Standing Desk Converter", "Furniture", 149.99, "Height-adjustable standing desk converter", generate_embedding("desk")),
        (8, "Protein Powder", "Food", 39.99, "Whey protein powder for muscle recovery", generate_embedding("protein")),
        (9, "Wireless Earbuds", "Electronics", 149.99, "True wireless earbuds with active noise cancellation", generate_embedding("earbuds")),
        (10, "Meditation Cushion", "Sports", 34.99, "Comfortable cushion for meditation practice", generate_embedding("meditation")),
    ];

    for (id, name, category, price, description, embedding) in products {
        let insert_sql = r#"
            INSERT INTO products (id, name, category, price, description, embedding)
            VALUES (?, ?, ?, ?, ?, ?)
        "#;

        db.execute_query(insert_sql, &[&id, &name, &category, &price, &description, &embedding]).await?;
    }

    println!("✅ Inserted {} sample products with embeddings", products.len());
    Ok(())
}

async fn perform_similarity_search(db: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n🔍 Performing similarity search...");

    // Search for products similar to "wireless audio device"
    let query_embedding = generate_embedding("wireless audio device");

    let search_start = Instant::now();
    let results = db.vector_search(&query_embedding, 5, "products", "embedding").await?;
    let search_time = search_start.elapsed();

    println!("🎯 Found {} similar products in {:.2}ms:", results.vectors.len(), search_time.as_millis());

    // Get product details for the top results
    for (i, (vector, distance)) in results.vectors.iter().zip(results.distances.iter()).enumerate() {
        // Get product info (simplified - in practice you'd join with the table)
        let product_query = format!("SELECT name, category, price FROM products LIMIT 1 OFFSET {}", i);
        let product_info = db.execute_query(&product_query).await?;

        if let Some(product_row) = product_info.data.first() {
            let parts: Vec<&str> = product_row.split('|').collect();
            if parts.len() >= 3 {
                println!("  {}. {} ({}) - ${} (distance: {:.4})",
                    i + 1, parts[0], parts[1], parts[2], distance);
            }
        }
    }

    Ok(())
}

async fn perform_advanced_vector_ops(db: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n⚡ Advanced vector operations...");

    // Search within specific category
    let electronics_embedding = generate_embedding("electronics");
    let category_results = db.vector_search_filtered(
        &electronics_embedding,
        3,
        "products",
        "embedding",
        "category = 'Electronics'"
    ).await?;

    println!("📱 Electronics products similar to 'electronics':");
    for (i, distance) in category_results.distances.iter().enumerate() {
        let query = format!("SELECT name, price FROM products WHERE category = 'Electronics' LIMIT 1 OFFSET {}", i);
        let result = db.execute_query(&query).await?;
        if let Some(row) = result.data.first() {
            let parts: Vec<&str> = row.split('|').collect();
            println!("  {}. {} - ${} (distance: {:.4})", i + 1, parts[0], parts[1], distance);
        }
    }

    // Batch search - find similar products for multiple queries
    let query_embeddings = vec![
        generate_embedding("audio equipment"),
        generate_embedding("office furniture"),
        generate_embedding("fitness gear"),
    ];

    let batch_results = db.vector_search_batch(&query_embeddings, 2, "products", "embedding").await?;
    println!("\n📊 Batch search results:");

    let query_names = vec!["Audio", "Office", "Fitness"];
    for (i, (query_name, results)) in query_names.iter().zip(batch_results.iter()).enumerate() {
        println!("  {} query results:", query_name);
        for (j, distance) in results.distances.iter().enumerate() {
            let query = format!("SELECT name FROM products LIMIT 1 OFFSET {}", i * 2 + j);
            let result = db.execute_query(&query).await?;
            if let Some(row) = result.data.first() {
                println!("    {}. {} (distance: {:.4})", j + 1, row, distance);
            }
        }
    }

    Ok(())
}

async fn demonstrate_performance(db: &AuroraDB) -> Result<(), Box<dyn std::error::Error>> {
    println!("\n⚡ Performance demonstration...");

    let query_embedding = generate_embedding("electronics");

    // Measure multiple searches to show JIT compilation benefits
    println!("🔥 Measuring performance improvements with JIT compilation:");

    let mut times = Vec::new();

    for i in 0..10 {
        let start = Instant::now();
        let _results = db.vector_search(&query_embedding, 5, "products", "embedding").await?;
        let duration = start.elapsed();
        times.push(duration.as_millis() as f64);

        if i == 0 {
            println!("  1st query: {:.2}ms (cold start)", duration.as_millis());
        } else if i == 9 {
            println!("  10th query: {:.2}ms (JIT optimized)", duration.as_millis());
        }
    }

    let avg_time = times.iter().sum::<f64>() / times.len() as f64;
    let speedup = times[0] / times.last().unwrap();

    println!("  Average: {:.2}ms", avg_time);
    println!("  Speedup: {:.1}x (JIT compilation benefit)", speedup);

    // Show JIT statistics
    let jit_status = db.get_jit_status().await?;
    println!("\n🚀 JIT Statistics:");
    println!("  Compilations: {}", jit_status.total_compilations);
    println!("  Cache hits: {}", jit_status.cache_hits);
    println!("  Cache hit rate: {:.1}%", jit_status.cache_hit_rate * 100.0);

    Ok(())
}

/// Generate a mock embedding vector for a text string
/// In practice, this would use a real embedding model like BERT or OpenAI
fn generate_embedding(text: &str) -> Vec<f32> {
    // Simple hash-based mock embedding for demonstration
    // Real embeddings would be 384-dimensional vectors from ML models
    use std::collections::hash_map::DefaultHasher;
    use std::hash::{Hash, Hasher};

    let mut hasher = DefaultHasher::new();
    text.hash(&mut hasher);
    let hash = hasher.finish();

    // Generate pseudo-random but deterministic vector
    let mut embedding = Vec::with_capacity(384);
    let mut current = hash;

    for _ in 0..384 {
        // Generate values between -1.0 and 1.0
        current = current.wrapping_mul(1103515245).wrapping_add(12345);
        let value = (current % 2001) as f32 / 1000.0 - 1.0;
        embedding.push(value);
    }

    embedding
}
