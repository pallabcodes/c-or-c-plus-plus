//! B-Tree Index: High-Performance Balanced Tree Index
//!
//! Advanced B-tree implementation with optimization for range queries,
//! concurrent access, and intelligent page management.

use std::collections::{BTreeMap, HashMap};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};

/// B-tree index configuration
#[derive(Debug, Clone)]
pub struct BTreeIndexConfig {
    pub name: String,
    pub columns: Vec<String>,
    pub unique: bool,
    pub fill_factor: u8, // Percentage (0-100)
}

/// B-tree node types
#[derive(Debug, Clone)]
enum NodeType {
    Internal,
    Leaf,
}

/// B-tree node
#[derive(Debug, Clone)]
struct BTreeNode {
    node_type: NodeType,
    keys: Vec<String>, // Composite key as string
    values: Vec<Vec<u64>>, // Row IDs (multiple for non-unique)
    children: Vec<usize>, // Child node indices
    parent: Option<usize>,
    is_dirty: bool,
}

/// B-tree index statistics
#[derive(Debug, Clone)]
pub struct BTreeStats {
    pub total_nodes: usize,
    pub leaf_nodes: usize,
    pub height: usize,
    pub avg_fill_factor: f64,
    pub total_entries: u64,
    pub cache_hit_rate: f64,
}

/// High-performance B-tree index
pub struct BTreeIndex {
    config: BTreeIndexConfig,
    nodes: RwLock<Vec<BTreeNode>>,
    root_node: RwLock<Option<usize>>,
    node_cache: RwLock<HashMap<usize, BTreeNode>>,
    stats: RwLock<BTreeStats>,
    max_keys_per_node: usize,
    min_keys_per_node: usize,
}

impl BTreeIndex {
    /// Create a new B-tree index
    pub fn new(config: BTreeIndexConfig) -> AuroraResult<Self> {
        if config.fill_factor == 0 || config.fill_factor > 100 {
            return Err(AuroraError::InvalidArgument("Fill factor must be between 1-100".to_string()));
        }

        // Order of B-tree (maximum keys per node)
        let order = 100; // Configurable in production
        let max_keys = order - 1;
        let min_keys = max_keys / 2;

        Ok(Self {
            config,
            nodes: RwLock::new(Vec::new()),
            root_node: RwLock::new(None),
            node_cache: RwLock::new(HashMap::new()),
            stats: RwLock::new(BTreeStats {
                total_nodes: 0,
                leaf_nodes: 0,
                height: 0,
                avg_fill_factor: 0.0,
                total_entries: 0,
                cache_hit_rate: 0.0,
            }),
            max_keys_per_node: max_keys,
            min_keys_per_node: min_keys,
        })
    }

    /// Insert a key-value pair into the index
    pub async fn insert(&self, composite_key: String, row_id: u64) -> AuroraResult<()> {
        let mut root = self.root_node.write();

        if root.is_none() {
            // Create root node
            let root_idx = self.create_leaf_node();
            *root = Some(root_idx);
        }

        let root_idx = root.unwrap();
        drop(root);

        // Insert recursively
        if let Some(split_key) = self.insert_recursive(root_idx, composite_key.clone(), row_id).await? {
            // Root split required
            let mut root_write = self.root_node.write();
            let old_root_idx = root_write.unwrap();

            let new_root_idx = self.create_internal_node();
            let new_leaf_idx = self.create_leaf_node();

            // Move split key to new root
            {
                let mut nodes = self.nodes.write();
                let new_root = &mut nodes[new_root_idx];
                new_root.keys.push(split_key.clone());
                new_root.children.push(old_root_idx);
                new_root.children.push(new_leaf_idx);

                // Update parent pointers
                nodes[old_root_idx].parent = Some(new_root_idx);
                nodes[new_leaf_idx].parent = Some(new_root_idx);
            }

            *root_write = Some(new_root_idx);
        }

        // Update statistics
        {
            let mut stats = self.stats.write();
            stats.total_entries += 1;
        }

        Ok(())
    }

    /// Search for values by key
    pub async fn search(&self, composite_key: &str) -> AuroraResult<Vec<u64>> {
        let root = self.root_node.read();

        if root.is_none() {
            return Ok(vec![]);
        }

        self.search_recursive(root.unwrap(), composite_key).await
    }

    /// Range search for keys within bounds
    pub async fn range_search(&self, start_key: &str, end_key: Option<&str>) -> AuroraResult<Vec<u64>> {
        let root = self.root_node.read();

        if root.is_none() {
            return Ok(vec![]);
        }

        let mut results = Vec::new();
        self.range_search_recursive(root.unwrap(), start_key, end_key, &mut results).await?;

        Ok(results)
    }

    /// Delete a key-value pair
    pub async fn delete(&self, composite_key: &str, row_id: u64) -> AuroraResult<bool> {
        let root = self.root_node.read();

        if root.is_none() {
            return Ok(false);
        }

        let deleted = self.delete_recursive(root.unwrap(), composite_key, row_id).await?;

        if deleted {
            let mut stats = self.stats.write();
            stats.total_entries = stats.total_entries.saturating_sub(1);
        }

        Ok(deleted)
    }

    /// Update statistics and perform maintenance
    pub async fn update_stats(&self) -> AuroraResult<()> {
        let nodes = self.nodes.read();
        let mut leaf_nodes = 0;
        let mut total_fill_factor = 0.0;

        for node in nodes.iter() {
            if matches!(node.node_type, NodeType::Leaf) {
                leaf_nodes += 1;
                let fill_factor = node.keys.len() as f64 / self.max_keys_per_node as f64;
                total_fill_factor += fill_factor;
            }
        }

        let avg_fill_factor = if leaf_nodes > 0 {
            total_fill_factor / leaf_nodes as f64
        } else {
            0.0
        };

        // Calculate height
        let height = if let Some(root_idx) = *self.root_node.read() {
            self.calculate_height(root_idx)
        } else {
            0
        };

        let mut stats = self.stats.write();
        stats.total_nodes = nodes.len();
        stats.leaf_nodes = leaf_nodes;
        stats.height = height;
        stats.avg_fill_factor = avg_fill_factor;

        Ok(())
    }

    /// Get index statistics
    pub fn get_stats(&self) -> BTreeStats {
        self.stats.read().clone()
    }

    /// Perform index maintenance (rebalancing, defragmentation)
    pub async fn maintain(&self) -> AuroraResult<()> {
        // Simplified maintenance - rebalance tree and update stats
        self.update_stats().await?;

        let stats = self.stats.read();
        if stats.avg_fill_factor < 0.5 {
            println!("🔧 B-tree index '{}' has low fill factor ({:.1}%), considering rebuild",
                    self.config.name, stats.avg_fill_factor);
        }

        Ok(())
    }

    // Private methods

    fn create_leaf_node(&self) -> usize {
        let mut nodes = self.nodes.write();
        let node_idx = nodes.len();

        let node = BTreeNode {
            node_type: NodeType::Leaf,
            keys: Vec::new(),
            values: Vec::new(),
            children: Vec::new(),
            parent: None,
            is_dirty: true,
        };

        nodes.push(node);
        node_idx
    }

    fn create_internal_node(&self) -> usize {
        let mut nodes = self.nodes.write();
        let node_idx = nodes.len();

        let node = BTreeNode {
            node_type: NodeType::Internal,
            keys: Vec::new(),
            values: Vec::new(),
            children: Vec::new(),
            parent: None,
            is_dirty: true,
        };

        nodes.push(node);
        node_idx
    }

    async fn insert_recursive(&self, node_idx: usize, key: String, value: u64) -> AuroraResult<Option<String>> {
        let node = self.get_node(node_idx).await?;

        match node.node_type {
            NodeType::Leaf => {
                self.insert_into_leaf(node_idx, key, value).await
            }
            NodeType::Internal => {
                let child_idx = self.find_child_index(&node, &key);
                let split_key = self.insert_recursive(child_idx, key, value).await?;

                if let Some(split_key) = split_key {
                    self.insert_into_internal(node_idx, split_key).await
                } else {
                    Ok(None)
                }
            }
        }
    }

    async fn insert_into_leaf(&self, node_idx: usize, key: String, value: u64) -> AuroraResult<Option<String>> {
        let mut nodes = self.nodes.write();
        let node = &mut nodes[node_idx];

        // Find insertion position
        let pos = node.keys.partition_point(|k| k < &key);

        // Check for unique constraint
        if self.config.unique && pos < node.keys.len() && node.keys[pos] == key {
            return Err(AuroraError::InvalidArgument(format!("Duplicate key '{}' violates unique constraint", key)));
        }

        // Insert key and value
        node.keys.insert(pos, key.clone());
        node.values.insert(pos, vec![value]);
        node.is_dirty = true;

        // Check if node needs to split
        if node.keys.len() > self.max_keys_per_node {
            Ok(Some(self.split_leaf_node(node_idx).await?))
        } else {
            Ok(None)
        }
    }

    async fn insert_into_internal(&self, node_idx: usize, key: String) -> AuroraResult<Option<String>> {
        let mut nodes = self.nodes.write();
        let node = &mut nodes[node_idx];

        let pos = node.keys.partition_point(|k| k < &key);
        node.keys.insert(pos, key);

        // Check if node needs to split
        if node.keys.len() > self.max_keys_per_node {
            Ok(Some(self.split_internal_node(node_idx).await?))
        } else {
            Ok(None)
        }
    }

    async fn split_leaf_node(&self, node_idx: usize) -> AuroraResult<String> {
        let mut nodes = self.nodes.write();

        // Create new sibling node
        let sibling_idx = nodes.len();
        let mut sibling = BTreeNode {
            node_type: NodeType::Leaf,
            keys: Vec::new(),
            values: Vec::new(),
            children: Vec::new(),
            parent: nodes[node_idx].parent,
            is_dirty: true,
        };

        // Split keys and values
        let mid = nodes[node_idx].keys.len() / 2;
        let split_key = nodes[node_idx].keys[mid].clone();

        sibling.keys = nodes[node_idx].keys[mid..].to_vec();
        sibling.values = nodes[node_idx].values[mid..].to_vec();

        nodes[node_idx].keys.truncate(mid);
        nodes[node_idx].values.truncate(mid);

        nodes.push(sibling);

        Ok(split_key)
    }

    async fn split_internal_node(&self, node_idx: usize) -> AuroraResult<String> {
        let mut nodes = self.nodes.write();

        // Create new sibling node
        let sibling_idx = nodes.len();
        let mut sibling = BTreeNode {
            node_type: NodeType::Internal,
            keys: Vec::new(),
            values: Vec::new(),
            children: Vec::new(),
            parent: nodes[node_idx].parent,
            is_dirty: true,
        };

        // Split keys and children
        let mid = nodes[node_idx].keys.len() / 2;
        let split_key = nodes[node_idx].keys[mid].clone();

        sibling.keys = nodes[node_idx].keys[mid + 1..].to_vec();
        sibling.children = nodes[node_idx].children[mid + 1..].to_vec();

        nodes[node_idx].keys.truncate(mid);
        nodes[node_idx].children.truncate(mid + 1);

        // Update parent pointers for moved children
        for &child_idx in &sibling.children {
            if let Some(child) = nodes.get_mut(child_idx) {
                child.parent = Some(sibling_idx);
            }
        }

        nodes.push(sibling);

        Ok(split_key)
    }

    async fn search_recursive(&self, node_idx: usize, key: &str) -> AuroraResult<Vec<u64>> {
        let node = self.get_node(node_idx).await?;

        match node.node_type {
            NodeType::Leaf => {
                // Find key in leaf node
                for (i, node_key) in node.keys.iter().enumerate() {
                    if node_key == key {
                        return Ok(node.values[i].clone());
                    }
                }
                Ok(vec![])
            }
            NodeType::Internal => {
                let child_idx = self.find_child_index(&node, key);
                self.search_recursive(child_idx, key).await
            }
        }
    }

    async fn range_search_recursive(
        &self,
        node_idx: usize,
        start_key: &str,
        end_key: Option<&str>,
        results: &mut Vec<u64>,
    ) -> AuroraResult<()> {
        let node = self.get_node(node_idx).await?;

        match node.node_type {
            NodeType::Leaf => {
                // Collect all values in range
                for (i, key) in node.keys.iter().enumerate() {
                    if key >= start_key {
                        if let Some(end) = end_key {
                            if key > end {
                                break;
                            }
                        }
                        results.extend_from_slice(&node.values[i]);
                    }
                }
            }
            NodeType::Internal => {
                // Find starting child
                let start_child = self.find_child_index(&node, start_key);

                // Search all relevant children
                for &child_idx in &node.children[start_child..] {
                    self.range_search_recursive(child_idx, start_key, end_key, results).await?;

                    // Check if we've gone past the end key
                    if let Some(end) = end_key {
                        let node = self.get_node(child_idx).await?;
                        if matches!(node.node_type, NodeType::Leaf) {
                            if let Some(last_key) = node.keys.last() {
                                if last_key > end {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        Ok(())
    }

    async fn delete_recursive(&self, node_idx: usize, key: &str, row_id: u64) -> AuroraResult<bool> {
        let node = self.get_node(node_idx).await?;

        match node.node_type {
            NodeType::Leaf => {
                self.delete_from_leaf(node_idx, key, row_id).await
            }
            NodeType::Internal => {
                let child_idx = self.find_child_index(&node, key);
                self.delete_recursive(child_idx, key, row_id).await
            }
        }
    }

    async fn delete_from_leaf(&self, node_idx: usize, key: &str, row_id: u64) -> AuroraResult<bool> {
        let mut nodes = self.nodes.write();
        let node = &mut nodes[node_idx];

        // Find and remove the key-value pair
        for (i, node_key) in node.keys.iter().enumerate() {
            if node_key == key {
                let values = &mut node.values[i];
                if let Some(pos) = values.iter().position(|&v| v == row_id) {
                    values.remove(pos);
                    node.is_dirty = true;

                    // Remove key if no values left
                    if values.is_empty() {
                        node.keys.remove(i);
                        node.values.remove(i);
                    }

                    return Ok(true);
                }
            }
        }

        Ok(false)
    }

    async fn get_node(&self, node_idx: usize) -> AuroraResult<BTreeNode> {
        // Check cache first
        {
            let cache = self.node_cache.read();
            if let Some(node) = cache.get(&node_idx) {
                return Ok(node.clone());
            }
        }

        // Load from storage
        let nodes = self.nodes.read();
        if let Some(node) = nodes.get(node_idx) {
            let node_clone = node.clone();

            // Cache the node
            let mut cache = self.node_cache.write();
            cache.insert(node_idx, node_clone.clone());

            Ok(node_clone)
        } else {
            Err(AuroraError::NotFound(format!("Node {} not found", node_idx)))
        }
    }

    fn find_child_index(&self, node: &BTreeNode, key: &str) -> usize {
        // Find the child index where the key should go
        for (i, node_key) in node.keys.iter().enumerate() {
            if key < node_key {
                return i;
            }
        }
        node.children.len() - 1
    }

    fn calculate_height(&self, node_idx: usize) -> usize {
        let nodes = self.nodes.read();
        let mut height = 1;
        let mut current_idx = node_idx;

        while let Some(node) = nodes.get(current_idx) {
            if matches!(node.node_type, NodeType::Internal) && !node.children.is_empty() {
                height += 1;
                current_idx = node.children[0];
            } else {
                break;
            }
        }

        height
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_test_config(name: &str) -> BTreeIndexConfig {
        BTreeIndexConfig {
            name: name.to_string(),
            columns: vec!["id".to_string()],
            unique: false,
            fill_factor: 90,
        }
    }

    #[tokio::test]
    async fn test_btree_index_creation() {
        let config = create_test_config("test_btree");
        let index = BTreeIndex::new(config).unwrap();
        assert!(true); // Passes if created successfully
    }

    #[tokio::test]
    async fn test_btree_insert_and_search() {
        let config = create_test_config("test_btree");
        let index = BTreeIndex::new(config).unwrap();

        // Insert some values
        index.insert("key1".to_string(), 1).await.unwrap();
        index.insert("key2".to_string(), 2).await.unwrap();
        index.insert("key3".to_string(), 3).await.unwrap();

        // Search for values
        let results1 = index.search("key1").await.unwrap();
        assert_eq!(results1, vec![1]);

        let results2 = index.search("key2").await.unwrap();
        assert_eq!(results2, vec![2]);

        let results_missing = index.search("key_missing").await.unwrap();
        assert_eq!(results_missing, Vec::<u64>::new());
    }

    #[tokio::test]
    async fn test_btree_range_search() {
        let config = create_test_config("test_btree");
        let index = BTreeIndex::new(config).unwrap();

        // Insert sorted values
        for i in 1..=10 {
            index.insert(format!("key{:02}", i), i as u64).await.unwrap();
        }

        // Range search
        let results = index.range_search("key03", Some("key07")).await.unwrap();
        assert_eq!(results, vec![3, 4, 5, 6, 7]);
    }

    #[tokio::test]
    async fn test_btree_unique_constraint() {
        let mut config = create_test_config("test_btree");
        config.unique = true;
        let index = BTreeIndex::new(config).unwrap();

        // Insert unique value
        index.insert("key1".to_string(), 1).await.unwrap();

        // Try to insert duplicate (should fail)
        let result = index.insert("key1".to_string(), 2).await;
        assert!(result.is_err());
    }

    #[tokio::test]
    async fn test_btree_delete() {
        let config = create_test_config("test_btree");
        let index = BTreeIndex::new(config).unwrap();

        // Insert and then delete
        index.insert("key1".to_string(), 1).await.unwrap();
        let deleted = index.delete("key1", 1).await.unwrap();
        assert!(deleted);

        // Search should return empty
        let results = index.search("key1").await.unwrap();
        assert_eq!(results, Vec::<u64>::new());
    }

    #[tokio::test]
    async fn test_btree_stats() {
        let config = create_test_config("test_btree");
        let index = BTreeIndex::new(config).unwrap();

        // Insert some data
        for i in 1..=5 {
            index.insert(format!("key{}", i), i as u64).await.unwrap();
        }

        index.update_stats().await.unwrap();
        let stats = index.get_stats();

        assert_eq!(stats.total_entries, 5);
        assert!(stats.height >= 1);
    }

    #[test]
    fn test_btree_config_validation() {
        // Invalid fill factor
        let config = BTreeIndexConfig {
            name: "test".to_string(),
            columns: vec!["col".to_string()],
            unique: false,
            fill_factor: 0, // Invalid
        };

        let result = BTreeIndex::new(config);
        assert!(result.is_err());
    }

    #[test]
    fn test_btree_stats_structure() {
        let stats = BTreeStats {
            total_nodes: 10,
            leaf_nodes: 8,
            height: 3,
            avg_fill_factor: 0.75,
            total_entries: 100,
            cache_hit_rate: 0.95,
        };

        assert_eq!(stats.total_nodes, 10);
        assert_eq!(stats.height, 3);
        assert_eq!(stats.avg_fill_factor, 0.75);
        assert_eq!(stats.cache_hit_rate, 0.95);
    }
}
