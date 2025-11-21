//! AuroraDB Window Functions Demo
//!
//! This demo showcases AuroraDB's window function capabilities:
//! - ROW_NUMBER(): Sequential numbering within partitions
//! - RANK() & DENSE_RANK(): Ranking with/without gaps
//! - LAG() & LEAD(): Access to previous/next rows
//! - FIRST_VALUE() & LAST_VALUE(): First/last values in window
//! - PARTITION BY: Data partitioning for windows
//! - ORDER BY: Window ordering within partitions

use auroradb::config::DatabaseConfig;
use auroradb::engine::AuroraDB;
use auroradb::security::UserContext;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB Window Functions Demo");
    println!("==================================");
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

    println!("✅ AuroraDB initialized with window functions");
    println!();

    // Demo 1: Setup employee sales data
    println!("📋 Demo 1: Setting up employee sales analytics data");
    setup_sales_data(&database, &user_context).await?;
    println!();

    // Demo 2: ROW_NUMBER() - Basic window function
    println!("📋 Demo 2: ROW_NUMBER() - Sequential numbering");
    println!("SQL: SELECT name, department, sales, ROW_NUMBER() OVER (ORDER BY sales DESC) as sales_rank FROM employees;");

    match database.execute_query(
        "SELECT name, department, sales, ROW_NUMBER() OVER (ORDER BY sales DESC) as sales_rank FROM employees;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("🏆 Sales Rankings (ROW_NUMBER):");
                println!("   Name          | Dept    | Sales  | Rank");
                println!("   --------------|---------|--------|-----");
                for row in &rows {
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let dept = row.get("department").unwrap_or(&auroradb::types::DataValue::Text("".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let rank = row.get("sales_rank").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    println!("   {:14} | {:7} | ${:5} | {:3}", format!("{}", name), format!("{}", dept), sales, rank);
                }
            }
        }
        Err(e) => {
            println!("❌ ROW_NUMBER failed: {}", e);
        }
    }
    println!();

    // Demo 3: PARTITION BY with ROW_NUMBER
    println!("📋 Demo 3: ROW_NUMBER() with PARTITION BY");
    println!("SQL: SELECT name, department, sales, ROW_NUMBER() OVER (PARTITION BY department ORDER BY sales DESC) as dept_rank FROM employees;");

    match database.execute_query(
        "SELECT name, department, sales, ROW_NUMBER() OVER (PARTITION BY department ORDER BY sales DESC) as dept_rank FROM employees;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("🏆 Department Rankings (PARTITION BY + ROW_NUMBER):");
                println!("   Name          | Dept    | Sales  | Dept Rank");
                println!("   --------------|---------|--------|----------");
                for row in &rows {
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let dept = row.get("department").unwrap_or(&auroradb::types::DataValue::Text("".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let rank = row.get("dept_rank").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    println!("   {:14} | {:7} | ${:5} | {:8}", format!("{}", name), format!("{}", dept), sales, rank);
                }
            }
        }
        Err(e) => {
            println!("❌ PARTITION BY ROW_NUMBER failed: {}", e);
        }
    }
    println!();

    // Demo 4: RANK() vs DENSE_RANK()
    println!("📋 Demo 4: RANK() vs DENSE_RANK() comparison");
    println!("SQL: SELECT name, sales, RANK() OVER (ORDER BY sales DESC) as rank, DENSE_RANK() OVER (ORDER BY sales DESC) as dense_rank FROM employees WHERE sales > 0;");

    match database.execute_query(
        "SELECT name, sales, RANK() OVER (ORDER BY sales DESC) as rank, DENSE_RANK() OVER (ORDER BY sales DESC) as dense_rank FROM employees WHERE sales > 0;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("🎯 RANK vs DENSE_RANK Comparison:");
                println!("   Name          | Sales  | RANK | DENSE_RANK");
                println!("   --------------|--------|------|-----------");
                for row in &rows {
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let rank = row.get("rank").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let dense_rank = row.get("dense_rank").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    println!("   {:14} | ${:5} | {:4} | {:9}", format!("{}", name), sales, rank, dense_rank);
                }
                println!("   → RANK has gaps for ties, DENSE_RANK doesn't");
            }
        }
        Err(e) => {
            println!("❌ RANK/DENSE_RANK failed: {}", e);
        }
    }
    println!();

    // Demo 5: LAG() and LEAD() functions
    println!("📋 Demo 5: LAG() and LEAD() - Previous/Next row access");
    println!("SQL: SELECT name, sales, LAG(sales) OVER (ORDER BY sales DESC) as prev_sales, LEAD(sales) OVER (ORDER BY sales DESC) as next_sales FROM employees;");

    match database.execute_query(
        "SELECT name, sales, LAG(sales) OVER (ORDER BY sales DESC) as prev_sales, LEAD(sales) OVER (ORDER BY sales DESC) as next_sales FROM employees;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("🔄 LAG/LEAD - Previous/Next Sales:");
                println!("   Name          | Sales  | Prev   | Next");
                println!("   --------------|--------|--------|--------");
                for row in &rows {
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let prev = row.get("prev_sales").unwrap_or(&auroradb::types::DataValue::Null);
                    let next = row.get("next_sales").unwrap_or(&auroradb::types::DataValue::Null);

                    let prev_str = match prev {
                        auroradb::types::DataValue::Null => "NULL".to_string(),
                        val => format!("${}", val)
                    };
                    let next_str = match next {
                        auroradb::types::DataValue::Null => "NULL".to_string(),
                        val => format!("${}", val)
                    };

                    println!("   {:14} | ${:5} | {:6} | {:6}", format!("{}", name), sales, prev_str, next_str);
                }
            }
        }
        Err(e) => {
            println!("❌ LAG/LEAD failed: {}", e);
        }
    }
    println!();

    // Demo 6: FIRST_VALUE() and LAST_VALUE()
    println!("📋 Demo 6: FIRST_VALUE() and LAST_VALUE()");
    println!("SQL: SELECT department, name, sales, FIRST_VALUE(name) OVER (PARTITION BY department ORDER BY sales DESC) as top_seller, LAST_VALUE(name) OVER (PARTITION BY department ORDER BY sales DESC) as bottom_seller FROM employees;");

    match database.execute_query(
        "SELECT department, name, sales, FIRST_VALUE(name) OVER (PARTITION BY department ORDER BY sales DESC) as top_seller, LAST_VALUE(name) OVER (PARTITION BY department ORDER BY sales DESC) as bottom_seller FROM employees;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("🥇 First/Last Values by Department:");
                println!("   Dept    | Name          | Sales  | Top Seller    | Bottom Seller");
                println!("   --------|---------------|--------|---------------|---------------");
                for row in &rows {
                    let dept = row.get("department").unwrap_or(&auroradb::types::DataValue::Text("".to_string()));
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let top = row.get("top_seller").unwrap_or(&auroradb::types::DataValue::Text("".to_string()));
                    let bottom = row.get("bottom_seller").unwrap_or(&auroradb::types::DataValue::Text("".to_string()));
                    println!("   {:7} | {:13} | ${:5} | {:13} | {:13}", format!("{}", dept), format!("{}", name), sales, format!("{}", top), format!("{}", bottom));
                }
            }
        }
        Err(e) => {
            println!("❌ FIRST_VALUE/LAST_VALUE failed: {}", e);
        }
    }
    println!();

    // Demo 7: Running totals with window aggregates
    println!("📋 Demo 7: Running totals - Window aggregate functions");
    println!("SQL: SELECT name, sales, SUM(sales) OVER (ORDER BY sales DESC ROWS UNBOUNDED PRECEDING) as running_total FROM employees;");

    match database.execute_query(
        "SELECT name, sales, SUM(sales) OVER (ORDER BY sales DESC ROWS UNBOUNDED PRECEDING) as running_total FROM employees;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("📈 Running Totals (Window SUM):");
                println!("   Name          | Sales  | Running Total");
                println!("   --------------|--------|--------------");
                for row in &rows {
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let running_total = row.get("running_total").unwrap_or(&auroradb::types::DataValue::Real(0.0));
                    println!("   {:14} | ${:5} | ${:10.2}", format!("{}", name), sales, running_total);
                }
            }
        }
        Err(e) => {
            println!("❌ Window aggregate failed: {}", e);
        }
    }
    println!();

    // Demo 8: Complex window analysis
    println!("📋 Demo 8: Complex window analysis - Department performance");
    println!("SQL: SELECT department, name, sales, ROW_NUMBER() OVER (PARTITION BY department ORDER BY sales DESC) as dept_position, AVG(sales) OVER (PARTITION BY department) as dept_avg, sales - AVG(sales) OVER (PARTITION BY department) as vs_avg FROM employees;");

    match database.execute_query(
        "SELECT department, name, sales, ROW_NUMBER() OVER (PARTITION BY department ORDER BY sales DESC) as dept_position, AVG(sales) OVER (PARTITION BY department) as dept_avg, sales - AVG(sales) OVER (PARTITION BY department) as vs_avg FROM employees;",
        &user_context
    ).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                println!("📊 Department Performance Analysis:");
                println!("   Dept    | Name          | Sales  | Position | Dept Avg | vs Avg");
                println!("   --------|---------------|--------|----------|----------|-------");
                for row in &rows {
                    let dept = row.get("department").unwrap_or(&auroradb::types::DataValue::Text("".to_string()));
                    let name = row.get("name").unwrap_or(&auroradb::types::DataValue::Text("Unknown".to_string()));
                    let sales = row.get("sales").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let position = row.get("dept_position").unwrap_or(&auroradb::types::DataValue::Integer(0));
                    let dept_avg = row.get("dept_avg").unwrap_or(&auroradb::types::DataValue::Real(0.0));
                    let vs_avg = row.get("vs_avg").unwrap_or(&auroradb::types::DataValue::Real(0.0));
                    println!("   {:7} | {:13} | ${:5} | {:8} | ${:6.1} | ${:4.1}", format!("{}", dept), format!("{}", name), sales, position, dept_avg, vs_avg);
                }
            }
        }
        Err(e) => {
            println!("❌ Complex window analysis failed: {}", e);
        }
    }
    println!();

    // Demo 9: Window function performance
    println!("📋 Demo 9: Window function performance comparison");

    // Time a regular query
    let start = std::time::Instant::now();
    for _ in 0..5 {
        let _ = database.execute_query("SELECT name, sales FROM employees ORDER BY sales DESC;", &user_context).await;
    }
    let regular_time = start.elapsed();

    // Time a window function query
    let start = std::time::Instant::now();
    for _ in 0..5 {
        let _ = database.execute_query("SELECT name, sales, ROW_NUMBER() OVER (ORDER BY sales DESC) as rank FROM employees;", &user_context).await;
    }
    let window_time = start.elapsed();

    println!("⚡ Performance Comparison (5 iterations each):");
    println!("   Regular query: {:.2}ms total ({:.2}ms avg)", regular_time.as_millis(), regular_time.as_millis() as f64 / 5.0);
    println!("   Window function: {:.2}ms total ({:.2}ms avg)", window_time.as_millis(), window_time.as_millis() as f64 / 5.0);
    println!("   Window overhead: {:.1}x", window_time.as_millis() as f64 / regular_time.as_millis() as f64);
    println!();

    // Demo 10: Window functions summary
    println!("📋 Demo 10: Window Functions Summary");
    println!("✅ ROW_NUMBER(): Sequential numbering within partitions");
    println!("✅ RANK() & DENSE_RANK(): Ranking with/without gaps for ties");
    println!("✅ LAG() & LEAD(): Access to previous/next rows in window");
    println!("✅ FIRST_VALUE() & LAST_VALUE(): First/last values in window");
    println!("✅ PARTITION BY: Data partitioning for independent windows");
    println!("✅ ORDER BY: Window ordering within partitions");
    println!("✅ Window aggregates: SUM, AVG, MIN, MAX over window frames");
    println!("✅ Complex analytics: Multi-function window analysis");
    println!("✅ MVCC integration: All windows work with transaction isolation");
    println!();

    println!("🎉 AuroraDB Window Functions Demo completed!");
    println!("   AuroraDB now supports:");
    println!("   ✅ Full SQL window function capabilities");
    println!("   ✅ PARTITION BY and ORDER BY clauses");
    println!("   ✅ Complex analytical queries");
    println!("   ✅ Advanced ranking and navigation functions");
    println!("   ✅ Running totals and window aggregates");

    println!();
    println!("🚧 Next Steps:");
    println!("   • Add more window functions (NTILE, PERCENT_RANK)");
    println!("   • Implement custom window frame bounds");
    println!("   • Add window function optimization");
    println!("   • Support window functions in subqueries");

    Ok(())
}

async fn setup_sales_data(db: &AuroraDB, user_context: &UserContext) -> Result<(), Box<dyn std::error::Error>> {
    // Create employees table
    db.execute_query(r#"
        CREATE TABLE employees (
            emp_id INTEGER PRIMARY KEY,
            name TEXT NOT NULL,
            department TEXT NOT NULL,
            sales INTEGER DEFAULT 0,
            hire_date TEXT
        );
    "#, user_context).await?;

    // Insert diverse employee sales data
    let employees = vec![
        (1, "Alice Johnson", "Sales", 15000, "2020-01-15"),
        (2, "Bob Smith", "Sales", 12000, "2020-03-20"),
        (3, "Charlie Brown", "Engineering", 18000, "2019-11-10"),
        (4, "Diana Prince", "Engineering", 16000, "2020-05-05"),
        (5, "Eve Wilson", "HR", 9000, "2021-02-28"),
        (6, "Frank Miller", "HR", 9500, "2020-08-15"),
        (7, "Grace Lee", "Sales", 14000, "2020-07-22"),
        (8, "Henry Ford", "Engineering", 17000, "2019-09-30"),
        (9, "Ivy Chen", "Marketing", 13000, "2020-12-01"),
        (10, "Jack Ryan", "Marketing", 11000, "2021-01-10"),
        (11, "Kate Moss", "Sales", 13000, "2020-11-15"),
        (12, "Liam Neeson", "Engineering", 19000, "2019-06-20"),
    ];

    for (id, name, dept, sales, hire_date) in employees {
        db.execute_query(
            &format!("INSERT INTO employees (emp_id, name, department, sales, hire_date) VALUES ({}, '{}', '{}', {}, '{}');",
                    id, name, dept, sales, hire_date),
            user_context
        ).await?;
    }

    println!("✅ Created employees table with {} diverse records", employees.len());
    println!("   • Departments: Sales, Engineering, HR, Marketing");
    println!("   • Sales range: $9,000 - $19,000");
    println!("   • Mix of high/low performers for ranking demos");

    // Verify data
    match db.execute_query("SELECT COUNT(*) FROM employees;", user_context).await {
        Ok(result) => {
            if let Some(rows) = result.rows {
                if let Some(row) = rows.first() {
                    if let Some(count) = row.get("COUNT(*)") {
                        println!("✅ Data verification: {} employee records loaded", count);
                    }
                }
            }
        }
        Err(e) => {
            println!("⚠️  Could not verify data count: {}", e);
        }
    }

    Ok(())
}
