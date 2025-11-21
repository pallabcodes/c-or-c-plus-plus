//! AuroraDB CLI Tool
//!
//! Command-line interface for AuroraDB administration and management.
//! Provides database operations, monitoring, and maintenance utilities.

use clap::{Arg, Command};
use std::io::{self, Write};
use tokio::runtime::Runtime;
use reqwest::Client;
use serde_json::Value;

mod commands;
mod client;
mod output;

use commands::*;
use client::AuroraClient;
use output::OutputFormat;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let app = Command::new("aurora-cli")
        .version("1.0.0")
        .author("AuroraDB Team")
        .about("AuroraDB command-line administration tool")
        .arg(Arg::new("host")
            .short('h')
            .long("host")
            .value_name("HOST")
            .help("Database host")
            .default_value("localhost"))
        .arg(Arg::new("port")
            .short('p')
            .long("port")
            .value_name("PORT")
            .help("Database port")
            .default_value("8080"))
        .arg(Arg::new("user")
            .short('U')
            .long("user")
            .value_name("USER")
            .help("Database user")
            .default_value("aurora"))
        .arg(Arg::new("password")
            .short('W')
            .long("password")
            .help("Prompt for password"))
        .arg(Arg::new("database")
            .short('d')
            .long("database")
            .value_name("DB")
            .help("Database name")
            .default_value("aurora"))
        .arg(Arg::new("format")
            .short('f')
            .long("format")
            .value_name("FORMAT")
            .help("Output format (table, json, csv)")
            .default_value("table"))
        .subcommand(
            Command::new("status")
                .about("Show database status and health")
                .alias("info")
        )
        .subcommand(
            Command::new("query")
                .about("Execute SQL query")
                .arg(Arg::new("sql")
                    .help("SQL query to execute")
                    .required(true)
                    .index(1))
                .alias("q")
        )
        .subcommand(
            Command::new("tables")
                .about("List database tables")
                .alias("ls")
        )
        .subcommand(
            Command::new("schema")
                .about("Show table schema")
                .arg(Arg::new("table")
                    .help("Table name")
                    .required(true)
                    .index(1))
        )
        .subcommand(
            Command::new("create-table")
                .about("Create a new table")
                .arg(Arg::new("name")
                    .help("Table name")
                    .required(true))
                .arg(Arg::new("columns")
                    .help("Column definitions (name:type,... )")
                    .required(true))
        )
        .subcommand(
            Command::new("metrics")
                .about("Show database metrics and performance stats")
                .alias("perf")
        )
        .subcommand(
            Command::new("backup")
                .about("Create database backup")
                .arg(Arg::new("output")
                    .short('o')
                    .long("output")
                    .value_name("FILE")
                    .help("Output file path")
                    .required(true))
        )
        .subcommand(
            Command::new("restore")
                .about("Restore database from backup")
                .arg(Arg::new("input")
                    .short('i')
                    .long("input")
                    .value_name("FILE")
                    .help("Input backup file")
                    .required(true))
        )
        .subcommand(
            Command::new("users")
                .about("Manage database users")
                .subcommand(
                    Command::new("list")
                        .about("List all users")
                )
                .subcommand(
                    Command::new("create")
                        .about("Create new user")
                        .arg(Arg::new("username")
                            .help("Username")
                            .required(true))
                        .arg(Arg::new("password")
                            .help("Password")
                            .required(true))
                )
                .subcommand(
                    Command::new("drop")
                        .about("Drop user")
                        .arg(Arg::new("username")
                            .help("Username")
                            .required(true))
                )
        )
        .subcommand(
            Command::new("cluster")
                .about("Cluster management commands")
                .subcommand(
                    Command::new("status")
                        .about("Show cluster status")
                )
                .subcommand(
                    Command::new("nodes")
                        .about("List cluster nodes")
                )
                .subcommand(
                    Command::new("join")
                        .about("Join node to cluster")
                        .arg(Arg::new("node")
                            .help("Node address")
                            .required(true))
                )
        )
        .subcommand(
            Command::new("jit")
                .about("JIT compilation management")
                .subcommand(
                    Command::new("status")
                        .about("Show JIT compilation status")
                )
                .subcommand(
                    Command::new("cache")
                        .about("Show JIT cache statistics")
                )
                .subcommand(
                    Command::new("clear")
                        .about("Clear JIT cache")
                )
        )
        .subcommand(
            Command::new("maintenance")
                .about("Database maintenance operations")
                .subcommand(
                    Command::new("vacuum")
                        .about("Run vacuum operation (AuroraDB optimized)")
                )
                .subcommand(
                    Command::new("analyze")
                        .about("Update table statistics")
                )
                .subcommand(
                    Command::new("reindex")
                        .about("Rebuild indexes")
                        .arg(Arg::new("table")
                            .help("Table name (optional)")
                            .required(false))
                )
        );

    let matches = app.get_matches();
    let host = matches.get_one::<String>("host").unwrap();
    let port = matches.get_one::<String>("port").unwrap();
    let user = matches.get_one::<String>("user").unwrap();
    let database = matches.get_one::<String>("database").unwrap();
    let format = matches.get_one::<String>("format").unwrap();

    let output_format = match format.as_str() {
        "json" => OutputFormat::Json,
        "csv" => OutputFormat::Csv,
        _ => OutputFormat::Table,
    };

    // Get password
    let password = if matches.get_flag("password") {
        rpassword::prompt_password("Password: ")?
    } else {
        std::env::var("AURORA_PASSWORD").unwrap_or_else(|_| "aurora".to_string())
    };

    // Create client
    let client = AuroraClient::new(host, port, user, &password, database)?;

    // Execute command
    match matches.subcommand() {
        Some(("status", _)) => {
            cmd_status(&client, output_format).await?;
        }
        Some(("query", sub_matches)) => {
            let sql = sub_matches.get_one::<String>("sql").unwrap();
            cmd_query(&client, sql, output_format).await?;
        }
        Some(("tables", _)) => {
            cmd_tables(&client, output_format).await?;
        }
        Some(("schema", sub_matches)) => {
            let table = sub_matches.get_one::<String>("table").unwrap();
            cmd_schema(&client, table, output_format).await?;
        }
        Some(("create-table", sub_matches)) => {
            let name = sub_matches.get_one::<String>("name").unwrap();
            let columns = sub_matches.get_one::<String>("columns").unwrap();
            cmd_create_table(&client, name, columns).await?;
        }
        Some(("metrics", _)) => {
            cmd_metrics(&client, output_format).await?;
        }
        Some(("backup", sub_matches)) => {
            let output = sub_matches.get_one::<String>("output").unwrap();
            cmd_backup(&client, output).await?;
        }
        Some(("restore", sub_matches)) => {
            let input = sub_matches.get_one::<String>("input").unwrap();
            cmd_restore(&client, input).await?;
        }
        Some(("users", sub_sub)) => {
            match sub_sub.subcommand() {
                Some(("list", _)) => cmd_users_list(&client, output_format).await?,
                Some(("create", sub_matches)) => {
                    let username = sub_matches.get_one::<String>("username").unwrap();
                    let password = sub_matches.get_one::<String>("password").unwrap();
                    cmd_users_create(&client, username, password).await?;
                }
                Some(("drop", sub_matches)) => {
                    let username = sub_matches.get_one::<String>("username").unwrap();
                    cmd_users_drop(&client, username).await?;
                }
                _ => print_help("users"),
            }
        }
        Some(("cluster", sub_sub)) => {
            match sub_sub.subcommand() {
                Some(("status", _)) => cmd_cluster_status(&client, output_format).await?,
                Some(("nodes", _)) => cmd_cluster_nodes(&client, output_format).await?,
                Some(("join", sub_matches)) => {
                    let node = sub_matches.get_one::<String>("node").unwrap();
                    cmd_cluster_join(&client, node).await?;
                }
                _ => print_help("cluster"),
            }
        }
        Some(("jit", sub_sub)) => {
            match sub_sub.subcommand() {
                Some(("status", _)) => cmd_jit_status(&client, output_format).await?,
                Some(("cache", _)) => cmd_jit_cache(&client, output_format).await?,
                Some(("clear", _)) => cmd_jit_clear(&client).await?,
                _ => print_help("jit"),
            }
        }
        Some(("maintenance", sub_sub)) => {
            match sub_sub.subcommand() {
                Some(("vacuum", _)) => cmd_maintenance_vacuum(&client).await?,
                Some(("analyze", _)) => cmd_maintenance_analyze(&client).await?,
                Some(("reindex", sub_matches)) => {
                    let table = sub_matches.get_one::<String>("table");
                    cmd_maintenance_reindex(&client, table.map(|s| s.as_str())).await?;
                }
                _ => print_help("maintenance"),
            }
        }
        _ => {
            println!("AuroraDB CLI Tool v1.0.0");
            println!("Use 'aurora-cli --help' for usage information");
        }
    }

    Ok(())
}

fn print_help(command: &str) {
    println!("Use 'aurora-cli {} --help' for usage information", command);
}
