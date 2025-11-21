//! AuroraDB Real Comparative Benchmark Demo
//!
//! This demo runs actual performance comparisons against real PostgreSQL
//! and MySQL database servers (if available), providing genuine competitive analysis.
//!
//! Prerequisites:
//! - PostgreSQL server running (optional)
//! - MySQL server running (optional)
//! - AuroraDB (always tested)

use auroradb::config::DatabaseConfig;
use benchmarks::real_comparative_benchmarks::{run_real_comparative_benchmarks, DatabaseConnection, DatabaseType};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 AuroraDB Real Comparative Benchmark Demo");
    println!("===========================================");
    println!();

    println!("⚠️  Prerequisites Check:");
    println!("   • PostgreSQL server (optional): Configure if available");
    println!("   • MySQL server (optional): Configure if available");
    println!("   • AuroraDB: Always tested");
    println!();

    // AuroraDB configuration
    let temp_dir = tempfile::tempdir()?;
    let aurora_config = DatabaseConfig {
        data_directory: temp_dir.path().to_string(),
        ..DatabaseConfig::default()
    };

    // PostgreSQL configuration (modify these for your environment)
    let postgres_conn = Some(DatabaseConnection {
        db_type: DatabaseType::PostgreSQL,
        host: "localhost".to_string(),
        port: 5432,
        database: "benchmark_db".to_string(),
        username: "postgres".to_string(),
        password: "password".to_string(), // Change this!
    });

    // MySQL configuration (modify these for your environment)
    let mysql_conn = Some(DatabaseConnection {
        db_type: DatabaseType::MySQL,
        host: "localhost".to_string(),
        port: 3306,
        database: "benchmark_db".to_string(),
        username: "root".to_string(),
        password: "password".to_string(), // Change this!
    });

    // Note: Comment out postgres_conn or mysql_conn if servers are not available
    // let postgres_conn = None;
    // let mysql_conn = None;

    println!("📋 Benchmark Configuration:");
    println!("   AuroraDB: ✅ Configured");
    println!("   PostgreSQL: {}", if postgres_conn.is_some() { "✅ Configured" } else { "❌ Not configured" });
    println!("   MySQL: {}", if mysql_conn.is_some() { "✅ Configured" } else { "❌ Not configured" });
    println!();

    println!("🏃 Running comprehensive benchmarks...");
    println!("   This may take several minutes...");
    println!();

    // Run the comparative benchmarks
    let results = run_real_comparative_benchmarks(
        aurora_config,
        postgres_conn,
        mysql_conn,
    ).await?;

    // Additional analysis
    println!("\n📊 Detailed Analysis");
    println!("==================");

    // Performance ratios
    if let Some(pg_ratio) = results.comparison_summary.aurora_vs_postgres_oltp_ratio {
        println!("AuroraDB vs PostgreSQL OLTP: {:.2}x {}", pg_ratio, if pg_ratio > 1.0 { "🚀" } else { "📉" });
    }

    if let Some(mysql_ratio) = results.comparison_summary.aurora_vs_mysql_oltp_ratio {
        println!("AuroraDB vs MySQL OLTP: {:.2}x {}", mysql_ratio, if mysql_ratio > 1.0 { "🚀" } else { "📉" });
    }

    // Latency analysis
    println!("\n⚡ Latency Analysis:");
    println!("AuroraDB OLTP latency: {:.1}ms", results.aurora_results.oltp_avg_latency_ms);

    if let Some(ref pg) = results.postgres_results {
        println!("PostgreSQL OLTP latency: {:.1}ms", pg.oltp_avg_latency_ms);
    }

    if let Some(ref my) = results.mysql_results {
        println!("MySQL OLTP latency: {:.1}ms", my.oltp_avg_latency_ms);
    }

    // Connection time analysis
    println!("\n🔌 Connection Performance:");
    println!("AuroraDB connection time: {:.1}ms", results.aurora_results.connection_time_ms);

    if let Some(ref pg) = results.postgres_results {
        println!("PostgreSQL connection time: {:.1}ms", pg.connection_time_ms);
    }

    if let Some(ref my) = results.mysql_results {
        println!("MySQL connection time: {:.1}ms", my.connection_time_ms);
    }

    // Export results
    let json_results = serde_json::to_string_pretty(&results)?;
    std::fs::write("real_benchmark_results.json", json_results)?;
    println!("\n📄 Detailed results exported to real_benchmark_results.json");

    // Final assessment
    println!("\n🎯 Final Assessment");
    println!("==================");

    let aurora_score = calculate_performance_score(&results);
    println!("AuroraDB Performance Score: {:.1}/10", aurora_score);

    if aurora_score >= 7.0 {
        println!("✅ AuroraDB shows competitive performance!");
        println!("   Ready for production workloads with proper tuning.");
    } else if aurora_score >= 5.0 {
        println!("⚠️  AuroraDB shows promising performance.");
        println!("   Further optimization needed for production use.");
    } else {
        println!("📉 AuroraDB needs significant optimization.");
        println!("   Not yet ready for production workloads.");
    }

    println!("\n🏆 Benchmark Summary:");
    println!("   OLTP Winner: {}", results.comparison_summary.winner_oltp);
    println!("   Analytical Winner: {}", results.comparison_summary.winner_analytical);

    println!("\n💡 Key Insights:");
    for rec in &results.comparison_summary.recommendations {
        println!("   • {}", rec);
    }

    println!("\n🎉 Real comparative benchmarking completed!");
    println!("   AuroraDB performance validated against industry standards.");

    Ok(())
}

fn calculate_performance_score(results: &benchmarks::real_comparative_benchmarks::ComparativeBenchmarkResults) -> f64 {
    let mut score = 0.0;

    // Base score for functionality
    score += 3.0; // AuroraDB can run benchmarks

    // Performance relative to competitors
    if let Some(pg_ratio) = results.comparison_summary.aurora_vs_postgres_oltp_ratio {
        if pg_ratio > 0.8 {
            score += 2.0; // Competitive with PostgreSQL
        } else if pg_ratio > 0.5 {
            score += 1.0; // Reasonable performance
        }
    }

    if let Some(mysql_ratio) = results.comparison_summary.aurora_vs_mysql_oltp_ratio {
        if mysql_ratio > 0.8 {
            score += 2.0; // Competitive with MySQL
        } else if mysql_ratio > 0.5 {
            score += 1.0; // Reasonable performance
        }
    }

    // Latency performance
    if results.aurora_results.oltp_avg_latency_ms < 50.0 {
        score += 1.5; // Good latency
    } else if results.aurora_results.oltp_avg_latency_ms < 100.0 {
        score += 1.0; // Acceptable latency
    }

    // Connection performance
    if results.aurora_results.connection_time_ms < 100.0 {
        score += 0.5; // Fast connections
    }

    score.min(10.0)
}

/*
To run this demo:

1. Start PostgreSQL server (if testing against PostgreSQL):
   ```bash
   # On macOS with Homebrew
   brew services start postgresql
   createdb benchmark_db
   ```

2. Start MySQL server (if testing against MySQL):
   ```bash
   # On macOS with Homebrew
   brew services start mysql
   mysql -u root -e "CREATE DATABASE benchmark_db;"
   ```

3. Update connection configurations in this file with your actual credentials

4. Run the demo:
   ```bash
   cargo run --example auroradb_real_comparison_demo
   ```

Note: If PostgreSQL/MySQL servers are not available, comment out the connection
configurations and the demo will run AuroraDB-only benchmarks.
*/
