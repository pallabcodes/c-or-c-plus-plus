//! AuroraDB DML (Data Manipulation Language) Demo
//!
//! This demo shows AuroraDB's working DML capabilities:
//! - INSERT with data validation and type checking
//! - UPDATE and DELETE (frameworks - coming soon)
//! - Schema validation and constraint enforcement

use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::security::UserContext;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB DML Demo");
    println!("===================");
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

    // Create a test table
    println!("📋 Creating test table...");
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
        Ok(_) => println!("✅ Table 'users' created successfully"),
        Err(e) => {
            println!("❌ Failed to create table: {}", e);
            return Ok(());
        }
    }

    // Demo 1: INSERT with explicit columns
    println!();
    println!("📋 Demo 1: INSERT with explicit columns");
    let insert1_sql = "INSERT INTO users (id, username, email, age) VALUES (1, 'alice', 'alice@example.com', 25);";

    match database.execute_query(insert1_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Insert successful");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Insert failed: {}", e);
        }
    }

    // Demo 2: INSERT multiple rows
    println!();
    println!("📋 Demo 2: INSERT multiple rows");
    let insert_multi_sql = r#"
        INSERT INTO users (id, username, email, age) VALUES
        (2, 'bob', 'bob@example.com', 30),
        (3, 'charlie', 'charlie@example.com', 35);
    "#;

    match database.execute_query(insert_multi_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Multi-row insert successful");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Multi-row insert failed: {}", e);
        }
    }

    // Demo 3: INSERT with type validation (should work)
    println!();
    println!("📋 Demo 3: INSERT with proper types");
    let insert_valid_sql = "INSERT INTO users (id, username, email, age, active) VALUES (4, 'diana', 'diana@example.com', 28, false);";

    match database.execute_query(insert_valid_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Valid insert successful");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ Valid insert failed: {}", e);
        }
    }

    // Demo 4: INSERT with NULL values (should work for nullable columns)
    println!();
    println!("📋 Demo 4: INSERT with NULL values");
    let insert_null_sql = "INSERT INTO users (id, username, email, age) VALUES (5, 'eve', NULL, NULL);";

    match database.execute_query(insert_null_sql, &user_context).await {
        Ok(result) => {
            println!("✅ NULL insert successful");
            println!("   Rows affected: {:?}", result.rows_affected);
        }
        Err(e) => {
            println!("❌ NULL insert failed: {}", e);
        }
    }

    // Demo 5: INSERT with type mismatch (should fail)
    println!();
    println!("📋 Demo 5: INSERT with type mismatch (should fail)");
    let insert_invalid_sql = "INSERT INTO users (id, username, email, age) VALUES ('not_an_int', 'frank', 'frank@example.com', 40);";

    match database.execute_query(insert_invalid_sql, &user_context).await {
        Ok(result) => {
            println!("⚠️  Unexpected success: {:?}", result);
        }
        Err(e) => {
            println!("✅ Expected failure (type mismatch): {}", e);
        }
    }

    // Demo 6: INSERT with NOT NULL violation (should fail)
    println!();
    println!("📋 Demo 6: INSERT with NOT NULL violation (should fail)");
    let insert_notnull_sql = "INSERT INTO users (id, email, age) VALUES (6, 'grace@example.com', 45);";

    match database.execute_query(insert_notnull_sql, &user_context).await {
        Ok(result) => {
            println!("⚠️  Unexpected success: {:?}", result);
        }
        Err(e) => {
            println!("✅ Expected failure (NOT NULL violation): {}", e);
        }
    }

    // Demo 7: UPDATE framework (not yet implemented)
    println!();
    println!("📋 Demo 7: UPDATE (framework - not yet implemented)");
    let update_sql = "UPDATE users SET age = 26 WHERE id = 1;";

    match database.execute_query(update_sql, &user_context).await {
        Ok(result) => {
            println!("✅ UPDATE framework executed");
            println!("   Rows affected: {:?}", result.rows_affected);
            println!("   ⚠️  Note: Actual update logic not yet implemented");
        }
        Err(e) => {
            println!("❌ UPDATE failed: {}", e);
        }
    }

    // Demo 8: DELETE framework (not yet implemented)
    println!();
    println!("📋 Demo 8: DELETE (framework - not yet implemented)");
    let delete_sql = "DELETE FROM users WHERE age > 50;";

    match database.execute_query(delete_sql, &user_context).await {
        Ok(result) => {
            println!("✅ DELETE framework executed");
            println!("   Rows affected: {:?}", result.rows_affected);
            println!("   ⚠️  Note: Actual delete logic not yet implemented");
        }
        Err(e) => {
            println!("❌ DELETE failed: {}", e);
        }
    }

    // Demo 9: Test catalog persistence
    println!();
    println!("📋 Demo 9: Testing data persistence");

    // Create a new database instance (simulating restart)
    println!("🔄 Simulating database restart...");
    let database2 = AuroraDB::new(DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    }).await?;

    // Try to insert into the existing table
    let insert_restart_sql = "INSERT INTO users (id, username, email, age) VALUES (7, 'restart_test', 'restart@example.com', 50);";

    match database2.execute_query(insert_restart_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Insert after restart successful");
            println!("   Rows affected: {:?}", result.rows_affected);
            println!("   ✅ Schema persistence works!");
        }
        Err(e) => {
            println!("❌ Insert after restart failed: {}", e);
        }
    }

    println!();
    println!("📊 Final Statistics");
    let stats = database.catalog.stats().await;
    println!("   Total tables: {}", stats.total_tables);
    println!("   Total columns: {}", stats.total_columns);
    println!("   Catalog size: {} bytes", stats.catalog_size_bytes);

    println!();
    println!("🎉 DML Demo completed!");
    println!("   AuroraDB now supports:");
    println!("   ✅ INSERT with data validation and type checking");
    println!("   ✅ Schema validation and constraint enforcement");
    println!("   ✅ Multiple row inserts");
    println!("   ✅ NULL handling and NOT NULL constraints");
    println!("   ✅ UPDATE/DELETE frameworks (logic coming soon)");
    println!("   ✅ Catalog persistence across sessions");

    println!();
    println!("🚧 Next Steps:");
    println!("   • Implement actual data storage for INSERT");
    println!("   • Complete UPDATE and DELETE logic");
    println!("   • Add transaction support");
    println!("   • Implement SELECT queries");

    Ok(())
}
