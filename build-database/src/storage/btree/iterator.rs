//! B+ Tree Iterator Implementation
//!
//! Provides efficient range query iterators for B+ tree operations.

use crate::core::*;
use super::node::{BTreeNode, NodeId};
use std::collections::VecDeque;

/// B+ Tree range iterator
pub struct BTreeIterator<'a> {
    /// Current position stack (node_id, key_index)
    position_stack: VecDeque<(NodeId, usize)>,
    /// Current leaf node data
    current_leaf: Option<&'a BTreeNode>,
    /// Current position in leaf
    current_index: usize,
    /// End key (exclusive)
    end_key: Vec<u8>,
    /// Node cache for traversal
    _node_cache: &'a std::collections::HashMap<NodeId, BTreeNode>,
}

impl<'a> BTreeIterator<'a> {
    /// Create a new iterator for range queries
    pub fn new(
        start_key: &[u8],
        end_key: &[u8],
        root_node: NodeId,
        node_cache: &'a std::collections::HashMap<NodeId, BTreeNode>,
    ) -> Self {
        let mut iterator = Self {
            position_stack: VecDeque::new(),
            current_leaf: None,
            current_index: 0,
            end_key: end_key.to_vec(),
            _node_cache: node_cache,
        };

        // Position iterator at start key
        iterator.seek_to_start(start_key, root_node);

        iterator
    }

    /// Position iterator at the first key >= start_key
    fn seek_to_start(&mut self, start_key: &[u8], root_node: NodeId) {
        // For now, this is a simplified implementation
        // In production, this would traverse the tree to find the starting position
        let _ = (start_key, root_node);
    }

    /// Get current key-value pair
    fn current(&self) -> Option<(Vec<u8>, Vec<u8>)> {
        if let Some(leaf) = self.current_leaf {
            if self.current_index < leaf.keys.len() {
                let key = leaf.keys[self.current_index].clone();
                let value = leaf.get_value(self.current_index)?.clone();
                return Some((key, value));
            }
        }
        None
    }
}

impl<'a> Iterator for BTreeIterator<'a> {
    type Item = (Vec<u8>, Vec<u8>);

    fn next(&mut self) -> Option<Self::Item> {
        // Get current item
        let current = self.current()?;

        // Check if we've reached the end
        if current.0 >= self.end_key {
            return None;
        }

        // Move to next item
        self.current_index += 1;

        // If we've exhausted current leaf, move to next leaf
        if self.current_index >= self.current_leaf?.keys.len() {
            // In production, this would follow the leaf chain
            self.current_leaf = None;
        }

        Some(current)
    }
}

/// B+ Tree reverse iterator for descending range queries
pub struct BTreeReverseIterator<'a> {
    /// Similar structure to forward iterator but in reverse
    position_stack: VecDeque<(NodeId, usize)>,
    current_leaf: Option<&'a BTreeNode>,
    current_index: usize,
    end_key: Vec<u8>,
    _node_cache: &'a std::collections::HashMap<NodeId, BTreeNode>,
}

impl<'a> BTreeReverseIterator<'a> {
    /// Create a new reverse iterator
    pub fn new(
        start_key: &[u8],
        end_key: &[u8],
        root_node: NodeId,
        node_cache: &'a std::collections::HashMap<NodeId, BTreeNode>,
    ) -> Self {
        let mut iterator = Self {
            position_stack: VecDeque::new(),
            current_leaf: None,
            current_index: 0,
            end_key: end_key.to_vec(),
            _node_cache: node_cache,
        };

        // Position at end and work backwards
        iterator.seek_to_end(start_key, root_node);

        iterator
    }

    /// Position iterator at the last key <= start_key
    fn seek_to_end(&mut self, start_key: &[u8], root_node: NodeId) {
        // Simplified implementation
        let _ = (start_key, root_node);
    }
}

impl<'a> Iterator for BTreeReverseIterator<'a> {
    type Item = (Vec<u8>, Vec<u8>);

    fn next(&mut self) -> Option<Self::Item> {
        // Similar to forward iterator but in reverse
        if let Some(leaf) = self.current_leaf {
            if self.current_index < leaf.keys.len() {
                let key = leaf.keys[self.current_index].clone();
                let value = leaf.get_value(self.current_index)?.clone();

                if key <= self.end_key {
                    return None;
                }

                // Move to previous item
                if self.current_index == 0 {
                    // Move to previous leaf
                    self.current_leaf = None;
                } else {
                    self.current_index -= 1;
                }

                return Some((key, value));
            }
        }
        None
    }
}
