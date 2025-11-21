//! B+ Tree Node Management
//!
//! Handles B+ tree node creation, splitting, and merging operations.

use crate::core::*;

/// B+ Tree node types
#[derive(Debug, Clone)]
pub enum NodeType {
    Internal,
    Leaf,
}

/// B+ Tree node structure
#[derive(Debug, Clone)]
pub struct BTreeNode {
    node_type: NodeType,
    keys: Vec<Vec<u8>>,
    children: Vec<NodeId>,
    values: Option<Vec<Vec<u8>>>, // Only for leaf nodes
    next_leaf: Option<NodeId>,   // For leaf node chaining
}

/// Unique identifier for B+ tree nodes
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct NodeId(pub u64);

/// B+ Tree configuration parameters
#[derive(Debug, Clone)]
pub struct BTreeConfig {
    /// Maximum keys per internal node
    pub internal_order: usize,
    /// Maximum keys per leaf node
    pub leaf_order: usize,
    /// Enable node compression
    pub enable_compression: bool,
    /// Cache size for hot nodes
    pub node_cache_size: usize,
}

impl Default for BTreeConfig {
    fn default() -> Self {
        Self {
            internal_order: 100,  // Typical B+ tree order
            leaf_order: 200,      // More entries in leaves
            enable_compression: true,
            node_cache_size: 1000,
        }
    }
}

impl BTreeNode {
    /// Create a new leaf node
    pub fn new_leaf(order: usize) -> Self {
        Self {
            node_type: NodeType::Leaf,
            keys: Vec::with_capacity(order),
            children: Vec::new(),
            values: Some(Vec::with_capacity(order)),
            next_leaf: None,
        }
    }

    /// Create a new internal node
    pub fn new_internal(order: usize) -> Self {
        Self {
            node_type: NodeType::Internal,
            keys: Vec::with_capacity(order),
            children: Vec::with_capacity(order + 1),
            values: None,
            next_leaf: None,
        }
    }

    /// Check if node is leaf
    pub fn is_leaf(&self) -> bool {
        matches!(self.node_type, NodeType::Leaf)
    }

    /// Check if node is full
    pub fn is_full(&self, order: usize) -> bool {
        self.keys.len() >= order
    }

    /// Get the number of keys in the node
    pub fn key_count(&self) -> usize {
        self.keys.len()
    }

    /// Insert a key-value pair into a leaf node
    pub fn insert_leaf(&mut self, key: Vec<u8>, value: Vec<u8>) -> Result<(), String> {
        if !self.is_leaf() {
            return Err("Cannot insert into internal node".to_string());
        }

        // Find insertion position
        let pos = self.keys.partition_point(|k| k < &key);

        self.keys.insert(pos, key);
        self.values.as_mut().unwrap().insert(pos, value);

        Ok(())
    }

    /// Search for a key in the node
    pub fn search(&self, key: &[u8]) -> Option<usize> {
        self.keys.iter().position(|k| k == key)
    }

    /// Get value at index (leaf nodes only)
    pub fn get_value(&self, index: usize) -> Option<&Vec<u8>> {
        self.values.as_ref()?.get(index)
    }

    /// Split a full node
    pub fn split(&mut self, order: usize) -> Result<(Vec<u8>, BTreeNode), String> {
        if !self.is_full(order) {
            return Err("Node is not full".to_string());
        }

        let mid = order / 2;
        let split_key = self.keys[mid].clone();

        // Create new node with right half
        let mut new_node = if self.is_leaf() {
            BTreeNode::new_leaf(order)
        } else {
            BTreeNode::new_internal(order)
        };

        // Move right half to new node
        new_node.keys = self.keys.split_off(mid + 1);
        if self.is_leaf() {
            new_node.values = Some(self.values.as_mut().unwrap().split_off(mid + 1));
            new_node.next_leaf = self.next_leaf.take();
            self.next_leaf = Some(NodeId(0)); // Will be set by caller
        } else {
            new_node.children = self.children.split_off(mid + 1);
        }

        // Remove split key from left node
        self.keys.pop();

        Ok((split_key, new_node))
    }
}
