//! AuroraDB DDL (Data Definition Language) Demo
//!
//! This demo shows AuroraDB's working DDL capabilities:
//! - CREATE TABLE with various column types and constraints
//! - DROP TABLE operations
//! - Catalog persistence across sessions

use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::security::UserContext;
use std::io::{self, Write};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB DDL Demo");
    println!("====================");
    println!();

    // Use a temporary directory for this demo
    let temp_dir = tempfile::tempdir()?;
    let data_dir = temp_dir.path().to_string();

    println!("📁 Using data directory: {}", data_dir);

    let config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    let database = AuroraDB::new(config).await?;
    let user_context = UserContext::system_user();

    println!("✅ AuroraDB initialized successfully");
    println!();

    // Demo 1: Create a simple table
    println!("📋 Demo 1: Creating a simple table");
    let create_sql = r#"
        CREATE TABLE users (
            id INTEGER PRIMARY KEY,
            username TEXT NOT NULL,
            email TEXT UNIQUE,
            age INTEGER,
            active BOOLEAN DEFAULT true
        );
    "#;

    match database.execute_query(create_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Table 'users' created successfully");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Failed to create table: {}", e);
            return Ok(());
        }
    }

    // Verify table was created
    let tables = database.catalog.list_tables().await;
    println!("📊 Current tables: {:?}", tables);
    println!();

    // Demo 2: Create a more complex table
    println!("📋 Demo 2: Creating a complex table with constraints");
    let create_complex_sql = r#"
        CREATE TABLE products (
            product_id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            description TEXT,
            price FLOAT NOT NULL,
            category TEXT,
            in_stock BOOLEAN DEFAULT true,
            created_at TEXT
        );
    "#;

    match database.execute_query(create_complex_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Table 'products' created successfully");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Failed to create products table: {}", e);
            return Ok(());
        }
    }

    // Show table details
    if let Ok(metadata) = database.catalog.get_table("users").await {
        if let Some(meta) = metadata {
            println!("📋 Table 'users' schema:");
            for column in &meta.columns {
                println!("   - {}: {:?} (nullable: {}, position: {})",
                    column.name,
                    column.data_type,
                    column.nullable,
                    column.ordinal_position
                );
            }
            println!("   Constraints: {} table constraints", meta.constraints.len());
        }
    }

    // Demo 3: List all tables
    println!();
    println!("📋 Demo 3: Current database schema");
    let all_tables = database.catalog.list_tables().await;
    println!("   Total tables: {}", all_tables.len());

    for table_name in &all_tables {
        if let Ok(Some(metadata)) = database.catalog.get_table(table_name).await {
            println!("   📄 {}: {} columns", table_name, metadata.columns.len());
        }
    }

    // Demo 4: Drop a table
    println!();
    println!("📋 Demo 4: Dropping a table");
    let drop_sql = "DROP TABLE products;";

    match database.execute_query(drop_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Table 'products' dropped successfully");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Failed to drop table: {}", e);
        }
    }

    // Verify table was dropped
    let remaining_tables = database.catalog.list_tables().await;
    println!("📊 Remaining tables: {:?}", remaining_tables);

    // Demo 5: Try to drop non-existent table (should fail)
    println!();
    println!("📋 Demo 5: Attempting to drop non-existent table (should fail)");
    let drop_nonexistent_sql = "DROP TABLE nonexistent_table;";

    match database.execute_query(drop_nonexistent_sql, &user_context).await {
        Ok(result) => {
            println!("⚠️  Unexpected success: {:?}", result);
        }
        Err(e) => {
            println!("✅ Expected failure: {}", e);
        }
    }

    // Demo 6: Drop table with IF EXISTS (should succeed)
    println!();
    println!("📋 Demo 6: Drop table with IF EXISTS (should succeed)");
    let drop_if_exists_sql = "DROP TABLE IF EXISTS nonexistent_table;";

    match database.execute_query(drop_if_exists_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Drop IF EXISTS succeeded (table didn't exist)");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Unexpected failure: {}", e);
        }
    }

    // Demo 7: Catalog persistence
    println!();
    println!("📋 Demo 7: Testing catalog persistence");

    // Create another table
    let create_persistent_sql = "CREATE TABLE persistent_test (id INTEGER, data TEXT);";
    database.execute_query(create_persistent_sql, &user_context).await?;
    println!("✅ Created 'persistent_test' table");

    // Create a new database instance (simulating restart)
    println!("🔄 Simulating database restart...");
    let database2 = AuroraDB::new(DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    }).await?;

    let tables_after_restart = database2.catalog.list_tables().await;
    println!("📊 Tables after restart: {:?}", tables_after_restart);

    if tables_after_restart.contains(&"persistent_test".to_string()) {
        println!("✅ Catalog persistence works! Table survived restart.");
    } else {
        println!("❌ Catalog persistence failed.");
    }

    // Final statistics
    println!();
    println!("📊 Final Statistics");
    let stats = database.catalog.stats().await;
    println!("   Total tables: {}", stats.total_tables);
    println!("   Total columns: {}", stats.total_columns);
    println!("   Catalog size: {} bytes", stats.catalog_size_bytes);

    println!();
    println!("🎉 DDL Demo completed successfully!");
    println!("   AuroraDB now supports:");
    println!("   ✅ CREATE TABLE with column types and constraints");
    println!("   ✅ DROP TABLE and DROP TABLE IF EXISTS");
    println!("   ✅ Catalog persistence across sessions");
    println!("   ✅ Schema validation and metadata management");

    Ok(())
}
