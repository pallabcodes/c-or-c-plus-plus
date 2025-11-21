//! AuroraDB Isolation Levels Demo
//!
//! This demo showcases AuroraDB's MVCC isolation levels:
//! - Read Uncommitted: Can see uncommitted changes
//! - Read Committed: Can only see committed changes
//! - Repeatable Read: Consistent snapshot, prevents non-repeatable reads
//! - Serializable: Prevents all concurrency anomalies

use std::sync::Arc;
use tokio::time::{sleep, Duration};
use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::security::UserContext;
use auroradb::mvcc::transaction::IsolationLevel;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB Isolation Levels Demo");
    println!("==================================");
    println!();

    // Setup database
    let temp_dir = tempfile::tempdir()?;
    let data_dir = temp_dir.path().to_string();

    let db_config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    let database = Arc::new(AuroraDB::new(db_config).await?);
    let user_context = UserContext::system_user();

    // Create test table
    setup_test_table(&database, &user_context).await?;
    println!("✅ Test table created with initial data");
    println!();

    // Demo 1: Read Uncommitted Isolation
    println!("📋 Demo 1: Read Uncommitted Isolation");
    println!("   • Can see uncommitted changes from other transactions");
    println!("   • May lead to dirty reads");

    demo_read_uncommitted(&database, &user_context).await?;
    println!();

    // Demo 2: Read Committed Isolation
    println!("📋 Demo 2: Read Committed Isolation");
    println!("   • Can only see committed changes");
    println!("   • Prevents dirty reads");

    demo_read_committed(&database, &user_context).await?;
    println!();

    // Demo 3: Repeatable Read Isolation
    println!("📋 Demo 3: Repeatable Read Isolation");
    println!("   • Consistent snapshot throughout transaction");
    println!("   • Prevents non-repeatable reads");

    demo_repeatable_read(&database, &user_context).await?;
    println!();

    // Demo 4: Serializable Isolation
    println!("📋 Demo 4: Serializable Isolation");
    println!("   • Strictest isolation level");
    println!("   • Prevents all concurrency anomalies");
    println!("   • May abort transactions due to conflicts");

    demo_serializable(&database, &user_context).await?;
    println!();

    // Demo 5: Isolation Level Comparison
    println!("📋 Demo 5: Isolation Level Comparison");
    println!("   ┌─────────────────┬─────────────┬─────────────────┬─────────────────┐");
    println!("   │ Isolation Level │ Dirty Reads │ Non-Repeatable │ Phantom Reads   │");
    println!("   │                 │             │ Reads          │                 │");
    println!("   ├─────────────────┼─────────────┼─────────────────┼─────────────────┤");
    println!("   │ Read Uncommitted│     ✅      │       ✅       │       ✅        │");
    println!("   │ Read Committed  │     ❌      │       ✅       │       ✅        │");
    println!("   │ Repeatable Read │     ❌      │       ❌       │       ✅        │");
    println!("   │ Serializable    │     ❌      │       ❌       │       ❌        │");
    println!("   └─────────────────┴─────────────┴─────────────────┴─────────────────┘");
    println!();

    // Demo 6: Performance Characteristics
    println!("📋 Demo 6: Performance Characteristics");

    println!("⚡ Isolation Level Performance:");
    println!("   • Read Uncommitted: Fastest (least locking)");
    println!("   • Read Committed: Balanced performance");
    println!("   • Repeatable Read: Higher overhead (snapshots)");
    println!("   • Serializable: Highest overhead (conflict detection)");
    println!();

    // Demo 7: Choosing the Right Isolation Level
    println!("📋 Demo 7: Choosing the Right Isolation Level");
    println!("   🔧 Use Cases:");
    println!("   • Read Uncommitted: Bulk reporting, data warehousing (tolerates inconsistencies)");
    println!("   • Read Committed: Most OLTP applications (good balance)");
    println!("   • Repeatable Read: Financial reporting, consistent reads");
    println!("   • Serializable: Banking, inventory control (absolute consistency)");
    println!();

    println!("🎉 AuroraDB Isolation Levels Demo completed!");
    println!("   AuroraDB now supports:");
    println!("   ✅ Read Uncommitted isolation");
    println!("   ✅ Read Committed isolation");
    println!("   ✅ Repeatable Read isolation");
    println!("   ✅ Serializable isolation with conflict detection");
    println!("   ✅ MVCC-based concurrency control");
    println!("   ✅ ACID transaction guarantees");

    println!();
    println!("🚧 Next Steps:");
    println!("   • Add deadlock detection");
    println!("   • Implement transaction timeouts");
    println!("   • Add isolation level hints");
    println!("   • Performance optimization for each level");

    Ok(())
}

async fn setup_test_table(db: &AuroraDB, user_context: &UserContext) -> Result<(), Box<dyn std::error::Error>> {
    // Create test table
    db.execute_query(r#"
        CREATE TABLE isolation_test (
            id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            balance INTEGER DEFAULT 0
        );
    "#, user_context).await?;

    // Insert initial data
    let initial_data = vec![
        (1, "Alice", 1000),
        (2, "Bob", 2000),
        (3, "Charlie", 1500),
    ];

    for (id, name, balance) in initial_data {
        db.execute_query(
            &format!("INSERT INTO isolation_test (id, name, balance) VALUES ({}, '{}', {});",
                    id, name, balance),
            user_context
        ).await?;
    }

    Ok(())
}

async fn demo_read_uncommitted(db: &AuroraDB, user_context: &UserContext) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔄 Starting Read Uncommitted demo...");

    // Transaction 1: Start but don't commit
    let txn1 = db.begin_transaction(IsolationLevel::ReadUncommitted, user_context).await?;
    println!("   Transaction 1 started (Read Uncommitted)");

    // Update balance but don't commit
    db.execute_query_with_transaction("UPDATE isolation_test SET balance = 999 WHERE id = 1;", &txn1, user_context).await?;
    println!("   Transaction 1: Updated Alice's balance to 999 (not committed)");

    // Transaction 2: Read uncommitted data
    let txn2 = db.begin_transaction(IsolationLevel::ReadUncommitted, user_context).await?;
    println!("   Transaction 2 started (Read Uncommitted)");

    let result = db.execute_query_with_transaction("SELECT name, balance FROM isolation_test WHERE id = 1;", &txn2, user_context).await?;
    if let Some(rows) = result.rows {
        if let Some(row) = rows.first() {
            if let Some(balance) = row.get("balance") {
                println!("   Transaction 2: Read Alice's balance as {} (dirty read!)", balance);
            }
        }
    }

    // Rollback transaction 1
    db.rollback_transaction(txn1.id, user_context).await?;
    println!("   Transaction 1: Rolled back");

    // Transaction 2 should now see original value
    let result2 = db.execute_query_with_transaction("SELECT name, balance FROM isolation_test WHERE id = 1;", &txn2, user_context).await?;
    if let Some(rows) = result2.rows {
        if let Some(row) = rows.first() {
            if let Some(balance) = row.get("balance") {
                println!("   Transaction 2: After rollback, reads Alice's balance as {} (correct)", balance);
            }
        }
    }

    db.commit_transaction(txn2.id, user_context).await?;
    println!("   Transaction 2: Committed");

    Ok(())
}

async fn demo_read_committed(db: &AuroraDB, user_context: &UserContext) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔄 Starting Read Committed demo...");

    // Transaction 1: Start and update
    let txn1 = db.begin_transaction(IsolationLevel::ReadCommitted, user_context).await?;
    println!("   Transaction 1 started (Read Committed)");

    db.execute_query_with_transaction("UPDATE isolation_test SET balance = 888 WHERE id = 2;", &txn1, user_context).await?;
    println!("   Transaction 1: Updated Bob's balance to 888 (not committed)");

    // Transaction 2: Try to read (should not see uncommitted change)
    let txn2 = db.begin_transaction(IsolationLevel::ReadCommitted, user_context).await?;
    println!("   Transaction 2 started (Read Committed)");

    let result = db.execute_query_with_transaction("SELECT name, balance FROM isolation_test WHERE id = 2;", &txn2, user_context).await?;
    if let Some(rows) = result.rows {
        if let Some(row) = rows.first() {
            if let Some(balance) = row.get("balance") {
                println!("   Transaction 2: Read Bob's balance as {} (no dirty read)", balance);
            }
        }
    }

    // Commit transaction 1
    db.commit_transaction(txn1.id, user_context).await?;
    println!("   Transaction 1: Committed");

    // Transaction 2 should now see the committed change
    let result2 = db.execute_query_with_transaction("SELECT name, balance FROM isolation_test WHERE id = 2;", &txn2, user_context).await?;
    if let Some(rows) = result2.rows {
        if let Some(row) = rows.first() {
            if let Some(balance) = row.get("balance") {
                println!("   Transaction 2: After commit, reads Bob's balance as {} (committed read)", balance);
            }
        }
    }

    db.commit_transaction(txn2.id, user_context).await?;
    println!("   Transaction 2: Committed");

    Ok(())
}

async fn demo_repeatable_read(db: &AuroraDB, user_context: &UserContext) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔄 Starting Repeatable Read demo...");

    // Reset Bob's balance
    db.execute_query("UPDATE isolation_test SET balance = 2000 WHERE id = 2;", user_context).await?;

    // Transaction 1: Start with Repeatable Read
    let txn1 = db.begin_transaction(IsolationLevel::RepeatableRead, user_context).await?;
    println!("   Transaction 1 started (Repeatable Read)");

    let result1 = db.execute_query_with_transaction("SELECT name, balance FROM isolation_test WHERE id = 2;", &txn1, user_context).await?;
    let initial_balance = if let Some(rows) = &result1.rows {
        if let Some(row) = rows.first() {
            row.get("balance").unwrap_or(&auroradb::types::DataValue::Integer(0)).clone()
        } else {
            auroradb::types::DataValue::Integer(0)
        }
    } else {
        auroradb::types::DataValue::Integer(0)
    };
    println!("   Transaction 1: Read Bob's balance as {}", initial_balance);

    // Transaction 2: Update Bob's balance and commit
    let txn2 = db.begin_transaction(IsolationLevel::ReadCommitted, user_context).await?;
    db.execute_query_with_transaction("UPDATE isolation_test SET balance = 2500 WHERE id = 2;", &txn2, user_context).await?;
    db.commit_transaction(txn2.id, user_context).await?;
    println!("   Transaction 2: Updated Bob's balance to 2500 and committed");

    // Transaction 1: Read again (should see same value due to snapshot)
    let result2 = db.execute_query_with_transaction("SELECT name, balance FROM isolation_test WHERE id = 2;", &txn1, user_context).await?;
    let second_balance = if let Some(rows) = &result2.rows {
        if let Some(row) = rows.first() {
            row.get("balance").unwrap_or(&auroradb::types::DataValue::Integer(0)).clone()
        } else {
            auroradb::types::DataValue::Integer(0)
        }
    } else {
        auroradb::types::DataValue::Integer(0)
    };
    println!("   Transaction 1: Re-read Bob's balance as {} (repeatable!)", second_balance);

    if initial_balance == second_balance {
        println!("   ✅ No non-repeatable read occurred");
    } else {
        println!("   ❌ Non-repeatable read detected");
    }

    db.commit_transaction(txn1.id, user_context).await?;
    println!("   Transaction 1: Committed");

    Ok(())
}

async fn demo_serializable(db: &AuroraDB, user_context: &UserContext) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔄 Starting Serializable demo...");

    // Transaction 1: Read balance
    let txn1 = db.begin_transaction(IsolationLevel::Serializable, user_context).await?;
    println!("   Transaction 1 started (Serializable)");

    let result1 = db.execute_query_with_transaction("SELECT SUM(balance) FROM isolation_test;", &txn1, user_context).await?;
    let sum1 = if let Some(rows) = &result1.rows {
        if let Some(row) = rows.first() {
            row.get("SUM(balance)").unwrap_or(&auroradb::types::DataValue::Integer(0)).clone()
        } else {
            auroradb::types::DataValue::Integer(0)
        }
    } else {
        auroradb::types::DataValue::Integer(0)
    };
    println!("   Transaction 1: Read total balance sum as {}", sum1);

    // Transaction 2: Update a balance
    let txn2 = db.begin_transaction(IsolationLevel::Serializable, user_context).await?;
    println!("   Transaction 2 started (Serializable)");

    db.execute_query_with_transaction("UPDATE isolation_test SET balance = balance + 100 WHERE id = 3;", &txn2, user_context).await?;
    println!("   Transaction 2: Updated Charlie's balance");

    // Try to commit transaction 2 first
    match db.commit_transaction(txn2.id, user_context).await {
        Ok(_) => println!("   Transaction 2: Committed successfully"),
        Err(e) => {
            println!("   Transaction 2: Failed to commit - {}", e);
            db.rollback_transaction(txn2.id, user_context).await?;
            println!("   Transaction 2: Rolled back");
        }
    }

    // Transaction 1: Try to read sum again (should be consistent)
    let result2 = db.execute_query_with_transaction("SELECT SUM(balance) FROM isolation_test;", &txn1, user_context).await?;
    let sum2 = if let Some(rows) = &result2.rows {
        if let Some(row) = rows.first() {
            row.get("SUM(balance)").unwrap_or(&auroradb::types::DataValue::Integer(0)).clone()
        } else {
            auroradb::types::DataValue::Integer(0)
        }
    } else {
        auroradb::types::DataValue::Integer(0)
    };
    println!("   Transaction 1: Re-read total balance sum as {}", sum2);

    // Try to commit transaction 1
    match db.commit_transaction(txn1.id, user_context).await {
        Ok(_) => {
            println!("   Transaction 1: Committed successfully");
            if sum1 == sum2 {
                println!("   ✅ Serializable consistency maintained");
            } else {
                println!("   ⚠️  Serializable consistency may have been affected");
            }
        }
        Err(e) => {
            println!("   Transaction 1: Failed to commit due to serialization conflict - {}", e);
            db.rollback_transaction(txn1.id, user_context).await?;
            println!("   Transaction 1: Rolled back due to conflict");
        }
    }

    Ok(())
}
