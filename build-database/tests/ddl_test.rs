//! DDL (Data Definition Language) Tests
//!
//! Tests for CREATE TABLE and DROP TABLE functionality.

use auroradb::core::{AuroraResult, AuroraError};
use auroradb::query::processing::SqlParser;
use auroradb::query::parser::ast::Query;
use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::security::UserContext;
use tempfile::tempdir;

#[tokio::test]
async fn test_create_table_parsing() -> AuroraResult<()> {
    let parser = SqlParser::new();

    let sql = "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, age INTEGER);";

    let parsed = parser.parse(sql).await?;
    match parsed {
        Query::CreateTable(create_query) => {
            assert_eq!(create_query.name, "users");
            assert_eq!(create_query.columns.len(), 3);

            // Check first column (id)
            assert_eq!(create_query.columns[0].name, "id");
            assert_eq!(create_query.columns[0].data_type, auroradb::types::DataType::Integer);
            assert!(!create_query.columns[0].nullable);

            // Check second column (name)
            assert_eq!(create_query.columns[1].name, "name");
            assert_eq!(create_query.columns[1].data_type, auroradb::types::DataType::Text);
            assert!(!create_query.columns[1].nullable);

            // Check third column (age)
            assert_eq!(create_query.columns[2].name, "age");
            assert_eq!(create_query.columns[2].data_type, auroradb::types::DataType::Integer);
            assert!(create_query.columns[2].nullable); // Default is nullable

            Ok(())
        }
        _ => Err(AuroraError::new(auroradb::core::ErrorCode::QueryExecutionError, "Expected CreateTable query")),
    }
}

#[tokio::test]
async fn test_drop_table_parsing() -> AuroraResult<()> {
    let parser = SqlParser::new();

    let sql = "DROP TABLE users;";

    let parsed = parser.parse(sql).await?;
    match parsed {
        Query::DropTable(drop_query) => {
            assert_eq!(drop_query.name, "users");
            assert!(!drop_query.if_exists);
            Ok(())
        }
        _ => Err(AuroraError::new(auroradb::core::ErrorCode::QueryExecutionError, "Expected DropTable query")),
    }
}

#[tokio::test]
async fn test_drop_table_if_exists_parsing() -> AuroraResult<()> {
    let parser = SqlParser::new();

    let sql = "DROP TABLE IF EXISTS users;";

    let parsed = parser.parse(sql).await?;
    match parsed {
        Query::DropTable(drop_query) => {
            assert_eq!(drop_query.name, "users");
            assert!(drop_query.if_exists);
            Ok(())
        }
        _ => Err(AuroraError::new(auroradb::core::ErrorCode::QueryExecutionError, "Expected DropTable query")),
    }
}

#[tokio::test]
async fn test_create_table_execution() -> AuroraResult<()> {
    let temp_dir = tempdir()?;
    let data_dir = temp_dir.path().to_string();

    let config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    let database = AuroraDB::new(config).await?;
    let user_context = UserContext::system_user();

    // Create a table
    let sql = "CREATE TABLE test_users (id INTEGER PRIMARY KEY, name TEXT NOT NULL, email TEXT);";
    let result = database.execute_query(sql, &user_context).await?;

    // DDL should not return rows but should succeed
    assert!(result.rows.is_none());
    assert_eq!(result.rows_affected, Some(0));

    // Verify table was created in catalog
    let catalog = &database.catalog; // Assuming we can access it
    assert!(catalog.table_exists("test_users").await);

    let columns = catalog.get_columns("test_users").await?;
    assert_eq!(columns.len(), 3);
    assert_eq!(columns[0].name, "id");
    assert_eq!(columns[1].name, "name");
    assert_eq!(columns[2].name, "email");

    Ok(())
}

#[tokio::test]
async fn test_drop_table_execution() -> AuroraResult<()> {
    let temp_dir = tempdir()?;
    let data_dir = temp_dir.path().to_string();

    let config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    let database = AuroraDB::new(config).await?;
    let user_context = UserContext::system_user();

    // Create a table first
    let create_sql = "CREATE TABLE temp_table (id INTEGER);";
    database.execute_query(create_sql, &user_context).await?;

    // Verify it exists
    assert!(database.catalog.table_exists("temp_table").await);

    // Drop the table
    let drop_sql = "DROP TABLE temp_table;";
    let result = database.execute_query(drop_sql, &user_context).await?;

    // DDL should succeed
    assert!(result.rows.is_none());
    assert_eq!(result.rows_affected, Some(0));

    // Verify table was dropped
    assert!(!database.catalog.table_exists("temp_table").await);

    Ok(())
}

#[tokio::test]
async fn test_drop_table_if_exists() -> AuroraResult<()> {
    let temp_dir = tempdir()?;
    let data_dir = temp_dir.path().to_string();

    let config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    let database = AuroraDB::new(config).await?;
    let user_context = UserContext::system_user();

    // Try to drop a non-existent table with IF EXISTS
    let drop_sql = "DROP TABLE IF EXISTS nonexistent_table;";
    let result = database.execute_query(drop_sql, &user_context).await?;

    // Should succeed even though table doesn't exist
    assert!(result.rows.is_none());
    assert_eq!(result.rows_affected, Some(0));

    Ok(())
}

#[tokio::test]
async fn test_drop_nonexistent_table_error() -> AuroraResult<()> {
    let temp_dir = tempdir()?;
    let data_dir = temp_dir.path().to_string();

    let config = DatabaseConfig {
        data_directory: data_dir.clone(),
        ..DatabaseConfig::default()
    };

    let database = AuroraDB::new(config).await?;
    let user_context = UserContext::system_user();

    // Try to drop a non-existent table without IF EXISTS
    let drop_sql = "DROP TABLE nonexistent_table;";
    let result = database.execute_query(drop_sql, &user_context).await;

    // Should fail
    assert!(result.is_err());

    Ok(())
}
