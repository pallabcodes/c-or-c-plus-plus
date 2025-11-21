//! AuroraDB CRUD Demo: Complete Data Operations
//!
//! This demo showcases AuroraDB's working data storage and retrieval:
//! - INSERT: Data validation and persistence
//! - SELECT: Data retrieval with filtering
//! - UPDATE: Data modification (framework)
//! - DELETE: Data removal (framework)
//! - Real data persistence across sessions

use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::security::UserContext;
use std::collections::HashMap;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB CRUD Demo: Complete Data Operations");
    println!("================================================");
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
    println!("📋 Creating test table 'employees'...");
    let create_sql = r#"
        CREATE TABLE employees (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            department TEXT,
            salary FLOAT,
            active BOOLEAN DEFAULT true
        );
    "#;

    match database.execute_query(create_sql, &user_context).await {
        Ok(_) => println!("✅ Table 'employees' created successfully"),
        Err(e) => {
            println!("❌ Failed to create table: {}", e);
            return Ok(());
        }
    }

    // Demo 1: INSERT operations with real data storage
    println!();
    println!("📋 Demo 1: INSERT operations - Real data storage");
    let insert_statements = vec![
        "INSERT INTO employees (id, name, department, salary) VALUES (1, 'Alice Johnson', 'Engineering', 95000.00);",
        "INSERT INTO employees (id, name, department, salary) VALUES (2, 'Bob Smith', 'Marketing', 75000.00);",
        "INSERT INTO employees (id, name, department, salary) VALUES (3, 'Charlie Brown', 'Sales', 80000.00);",
        "INSERT INTO employees (id, name, department, salary, active) VALUES (4, 'Diana Prince', 'HR', 70000.00, false);",
    ];

    for (i, sql) in insert_statements.iter().enumerate() {
        match database.execute_query(sql, &user_context).await {
            Ok(result) => {
                println!("✅ Insert {} successful - {} rows affected", i + 1, result.rows_affected.unwrap_or(0));
            }
            Err(e) => {
                println!("❌ Insert {} failed: {}", i + 1, e);
            }
        }
    }

    // Demo 2: SELECT operations - Real data retrieval
    println!();
    println!("📋 Demo 2: SELECT operations - Real data retrieval");
    let select_sql = "SELECT * FROM employees;";

    match database.execute_query(select_sql, &user_context).await {
        Ok(result) => {
            println!("✅ SELECT executed successfully");
            if let Some(rows) = result.rows {
                println!("📊 Retrieved {} rows:", rows.len());
                for (i, row) in rows.iter().enumerate() {
                    println!("   Row {}: {:?}", i + 1, row);
                }

                // Show individual column access
                if let Some(first_row) = rows.first() {
                    if let Some(name) = first_row.get("name") {
                        println!("   First employee name: {:?}", name);
                    }
                    if let Some(salary) = first_row.get("salary") {
                        println!("   First employee salary: {:?}", salary);
                    }
                }
            } else {
                println!("   ⚠️  No rows returned");
            }
        }
        Err(e) => {
            println!("❌ SELECT failed: {}", e);
        }
    }

    // Demo 3: SELECT with WHERE clause
    println!();
    println!("📋 Demo 3: SELECT with WHERE clause");
    let select_where_sql = "SELECT * FROM employees WHERE id = 1;";

    match database.execute_query(select_where_sql, &user_context).await {
        Ok(result) => {
            println!("✅ WHERE clause SELECT executed");
            if let Some(rows) = result.rows {
                println!("📊 Filtered results: {} rows", rows.len());
                for row in &rows {
                    println!("   {:?}", row);
                }
            }
        }
        Err(e) => {
            println!("❌ WHERE SELECT failed: {}", e);
        }
    }

    // Demo 4: SELECT specific columns
    println!();
    println!("📋 Demo 4: SELECT specific columns");
    let select_columns_sql = "SELECT name, department, salary FROM employees WHERE department = 'Engineering';";

    match database.execute_query(select_columns_sql, &user_context).await {
        Ok(result) => {
            println!("✅ Column-specific SELECT executed");
            if let Some(rows) = result.rows {
                println!("📊 Engineering department:");
                for row in &rows {
                    println!("   {} ({}) - ${:?}",
                        row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string())),
                        row.get("department").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string())),
                        row.get("salary").unwrap_or(&auroradb::types::DataValue::Float(0.0))
                    );
                }
            }
        }
        Err(e) => {
            println!("❌ Column SELECT failed: {}", e);
        }
    }

    // Demo 5: Data persistence verification
    println!();
    println!("📋 Demo 5: Data persistence verification");

    // Count current rows
    let count_before = match database.execute_query("SELECT * FROM employees;", &user_context).await {
        Ok(result) => result.rows.map(|r| r.len()).unwrap_or(0),
        Err(_) => 0,
    };
    println!("   Rows before restart: {}", count_before);

    // Simulate database restart
    println!("🔄 Simulating database restart...");
    let database2 = AuroraDB::new(DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    }).await?;

    // Verify data persists
    let count_after = match database2.execute_query("SELECT * FROM employees;", &user_context).await {
        Ok(result) => result.rows.map(|r| r.len()).unwrap_or(0),
        Err(_) => 0,
    };
    println!("   Rows after restart: {}", count_after);

    if count_after == count_before && count_after > 0 {
        println!("✅ Data persistence works! All {} rows survived restart.", count_after);
    } else {
        println!("❌ Data persistence failed. Expected {}, got {}", count_before, count_after);
    }

    // Demo 6: UPDATE operations (framework)
    println!();
    println!("📋 Demo 6: UPDATE operations (framework - not fully implemented)");
    let update_sql = "UPDATE employees SET salary = 100000 WHERE id = 1;";

    match database2.execute_query(update_sql, &user_context).await {
        Ok(result) => {
            println!("✅ UPDATE framework executed");
            println!("   Rows affected: {:?}", result.rows_affected);
            println!("   ⚠️  Note: Actual update logic not yet implemented");
        }
        Err(e) => {
            println!("❌ UPDATE failed: {}", e);
        }
    }

    // Demo 7: DELETE operations (framework)
    println!();
    println!("📋 Demo 7: DELETE operations (framework - not fully implemented)");
    let delete_sql = "DELETE FROM employees WHERE department = 'HR';";

    match database2.execute_query(delete_sql, &user_context).await {
        Ok(result) => {
            println!("✅ DELETE framework executed");
            println!("   Rows affected: {:?}", result.rows_affected);
            println!("   ⚠️  Note: Actual delete logic not yet implemented");
        }
        Err(e) => {
            println!("❌ DELETE failed: {}", e);
        }
    }

    // Demo 8: Schema validation demo
    println!();
    println!("📋 Demo 8: Schema validation demo");

    // Try to insert invalid data
    let invalid_inserts = vec![
        "INSERT INTO employees (id, name) VALUES ('invalid_id', 'Test');", // Wrong type for id
        "INSERT INTO employees (department, salary) VALUES ('Test', 50000);", // Missing NOT NULL name
    ];

    for (i, sql) in invalid_inserts.iter().enumerate() {
        match database2.execute_query(sql, &user_context).await {
            Ok(_) => println!("⚠️  Invalid insert {} unexpectedly succeeded", i + 1),
            Err(e) => println!("✅ Invalid insert {} correctly rejected: {}", i + 1, e),
        }
    }

    // Final statistics
    println!();
    println!("📊 Final Statistics");
    let final_count = match database2.execute_query("SELECT * FROM employees;", &user_context).await {
        Ok(result) => result.rows.map(|r| r.len()).unwrap_or(0),
        Err(_) => 0,
    };
    println!("   Total rows in database: {}", final_count);

    let stats = database2.catalog.stats().await;
    println!("   Tables: {}", stats.total_tables);
    println!("   Columns: {}", stats.total_columns);
    println!("   Catalog size: {} bytes", stats.catalog_size_bytes);

    println!();
    println!("🎉 CRUD Demo completed!");
    println!("   AuroraDB now supports:");
    println!("   ✅ INSERT with real data storage and validation");
    println!("   ✅ SELECT with data retrieval and WHERE filtering");
    println!("   ✅ UPDATE/DELETE frameworks (logic coming soon)");
    println!("   ✅ Data persistence across database sessions");
    println!("   ✅ Schema validation and constraint enforcement");
    println!("   ✅ Type safety and NOT NULL constraints");

    println!();
    println!("🚧 Next Steps:");
    println!("   • Complete UPDATE and DELETE implementations");
    println!("   • Add complex WHERE clauses and JOINs");
    println!("   • Implement transactions and ACID compliance");
    println!("   • Add indexing for performance");
    println!("   • Complete production benchmarking");

    Ok(())
}
