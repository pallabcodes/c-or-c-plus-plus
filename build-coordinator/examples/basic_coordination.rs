//! Basic Aurora Coordinator Example
//!
//! Demonstrates fundamental coordinator operations:
//! - Starting the coordinator
//! - Registering AuroraDB nodes
//! - Basic cluster operations

use aurora_coordinator::{Coordinator, Config};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 Aurora Coordinator - Basic Coordination Example");
    println!("==================================================");

    // Create coordinator with default configuration
    let config = Config::default();
    let coordinator = Coordinator::new(config).await?;

    println!("✅ Coordinator initialized with node ID: {}", coordinator.node_id());

    // Register AuroraDB nodes
    println!("\n📝 Registering AuroraDB nodes...");

    let node1_id = coordinator.register_aurora_node(
        "aurora-node-1",
        "localhost:5432"
    ).await?;
    println!("✅ Registered AuroraDB node 1: {}", node1_id);

    let node2_id = coordinator.register_aurora_node(
        "aurora-node-2", 
        "localhost:5433"
    ).await?;
    println!("✅ Registered AuroraDB node 2: {}", node2_id);

    let node3_id = coordinator.register_aurora_node(
        "aurora-node-3",
        "localhost:5434"
    ).await?;
    println!("✅ Registered AuroraDB node 3: {}", node3_id);

    // Start coordination
    println!("\n🔄 Starting coordination...");
    coordinator.start().await?;
    println!("✅ Coordinator started successfully");

    // Get cluster status
    let status = coordinator.get_cluster_status().await?;
    println!("\n📊 Cluster Status:");
    println!("   Cluster Name: {}", status.name);
    println!("   Members: {}", status.members.len());
    println!("   Current Term: {}", status.term);
    println!("   Commit Index: {}", status.commit_index);

    // Simulate some coordination activity
    println!("\n⏳ Running coordination for 5 seconds...");
    tokio::time::sleep(std::time::Duration::from_secs(5)).await;

    // Stop coordination
    println!("\n🛑 Stopping coordinator...");
    coordinator.stop().await?;
    println!("✅ Coordinator stopped successfully");

    println!("\n🎉 Basic coordination example completed!");
    Ok(())
}
