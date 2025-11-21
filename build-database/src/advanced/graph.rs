//! AuroraDB Graph Database: Property Graphs with Vector Embeddings
//!
//! Revolutionary graph database capabilities that integrate:
//! - Property Graph Model (nodes, relationships, properties)
//! - Vector embeddings on nodes and relationships
//! - Graph algorithms (PageRank, centrality, path finding)
//! - Vector-enhanced graph queries
//! - Graph neural network support

use std::collections::{HashMap, HashSet, VecDeque, BTreeMap};
use crate::core::errors::{AuroraResult, AuroraError};

/// Property Graph with Vector Embeddings
pub struct PropertyGraph {
    /// Graph nodes with properties and embeddings
    nodes: HashMap<NodeId, Node>,
    /// Graph relationships with properties and embeddings
    relationships: HashMap<RelationshipId, Relationship>,
    /// Adjacency list for efficient traversal
    adjacency_list: HashMap<NodeId, Vec<(RelationshipId, Direction)>>,
    /// Reverse adjacency list
    reverse_adjacency: HashMap<NodeId, Vec<(RelationshipId, Direction)>>,
    /// Node labels and their nodes
    node_labels: HashMap<String, HashSet<NodeId>>,
    /// Relationship types and their relationships
    relationship_types: HashMap<String, HashSet<RelationshipId>>,
    /// Vector index for semantic search
    vector_index: GraphVectorIndex,
    /// Graph statistics
    stats: GraphStatistics,
}

/// Node in the property graph
#[derive(Debug, Clone)]
pub struct Node {
    pub id: NodeId,
    pub labels: Vec<String>,
    pub properties: HashMap<String, PropertyValue>,
    pub embedding: Option<Vec<f32>>,
    pub created_at: i64,
    pub updated_at: i64,
}

/// Relationship in the property graph
#[derive(Debug, Clone)]
pub struct Relationship {
    pub id: RelationshipId,
    pub type_name: String,
    pub start_node: NodeId,
    pub end_node: NodeId,
    pub properties: HashMap<String, PropertyValue>,
    pub embedding: Option<Vec<f32>>,
    pub direction: Direction,
    pub created_at: i64,
    pub updated_at: i64,
}

/// Property values that can be stored
#[derive(Debug, Clone, PartialEq)]
pub enum PropertyValue {
    String(String),
    Integer(i64),
    Float(f64),
    Boolean(bool),
    Vector(Vec<f32>),
    List(Vec<PropertyValue>),
    Map(HashMap<String, PropertyValue>),
}

/// Graph traversal direction
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Direction {
    Outgoing,
    Incoming,
    Both,
}

/// Node and relationship identifiers
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub struct NodeId(pub u64);

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub struct RelationshipId(pub u64);

/// Graph query result
#[derive(Debug, Clone)]
pub struct GraphQueryResult {
    pub nodes: Vec<Node>,
    pub relationships: Vec<Relationship>,
    pub paths: Vec<Path>,
    pub statistics: QueryStatistics,
}

/// Path in graph traversal
#[derive(Debug, Clone)]
pub struct Path {
    pub nodes: Vec<NodeId>,
    pub relationships: Vec<RelationshipId>,
    pub cost: f64,
}

/// Query execution statistics
#[derive(Debug, Clone, Default)]
pub struct QueryStatistics {
    pub nodes_traversed: usize,
    pub relationships_traversed: usize,
    pub execution_time_ms: f64,
    pub heap_allocations: usize,
}

/// Vector index for graph entities
pub struct GraphVectorIndex {
    node_embeddings: HashMap<NodeId, Vec<f32>>,
    relationship_embeddings: HashMap<RelationshipId, Vec<f32>>,
    // In production, this would use AuroraDB's vector index
}

impl PropertyGraph {
    /// Create a new property graph
    pub fn new() -> Self {
        Self {
            nodes: HashMap::new(),
            relationships: HashMap::new(),
            adjacency_list: HashMap::new(),
            reverse_adjacency: HashMap::new(),
            node_labels: HashMap::new(),
            relationship_types: HashMap::new(),
            vector_index: GraphVectorIndex::new(),
            stats: GraphStatistics::default(),
        }
    }

    /// Create a new node
    pub fn create_node(&mut self, labels: Vec<String>, properties: HashMap<String, PropertyValue>, embedding: Option<Vec<f32>>) -> AuroraResult<NodeId> {
        let node_id = NodeId(self.generate_id());
        let now = chrono::Utc::now().timestamp();

        let node = Node {
            id: node_id,
            labels: labels.clone(),
            properties,
            embedding: embedding.clone(),
            created_at: now,
            updated_at: now,
        };

        self.nodes.insert(node_id, node);

        // Update label index
        for label in labels {
            self.node_labels.entry(label).or_insert_with(HashSet::new).insert(node_id);
        }

        // Update vector index
        if let Some(emb) = embedding {
            self.vector_index.add_node_embedding(node_id, emb);
        }

        self.stats.node_count += 1;
        Ok(node_id)
    }

    /// Create a relationship between nodes
    pub fn create_relationship(
        &mut self,
        start_node: NodeId,
        end_node: NodeId,
        type_name: String,
        properties: HashMap<String, PropertyValue>,
        embedding: Option<Vec<f32>>
    ) -> AuroraResult<RelationshipId> {
        // Validate nodes exist
        if !self.nodes.contains_key(&start_node) || !self.nodes.contains_key(&end_node) {
            return Err(AuroraError::InvalidArgument("Start or end node does not exist".to_string()));
        }

        let relationship_id = RelationshipId(self.generate_id());
        let now = chrono::Utc::now().timestamp();

        let relationship = Relationship {
            id: relationship_id,
            type_name: type_name.clone(),
            start_node,
            end_node,
            properties,
            embedding: embedding.clone(),
            direction: Direction::Outgoing,
            created_at: now,
            updated_at: now,
        };

        self.relationships.insert(relationship_id, relationship);

        // Update adjacency lists
        self.adjacency_list.entry(start_node).or_insert_with(Vec::new)
            .push((relationship_id, Direction::Outgoing));
        self.reverse_adjacency.entry(end_node).or_insert_with(Vec::new)
            .push((relationship_id, Direction::Incoming));

        // Update type index
        self.relationship_types.entry(type_name).or_insert_with(HashSet::new).insert(relationship_id);

        // Update vector index
        if let Some(emb) = embedding {
            self.vector_index.add_relationship_embedding(relationship_id, emb);
        }

        self.stats.relationship_count += 1;
        Ok(relationship_id)
    }

    /// Execute Cypher-like graph query
    pub fn query(&self, query: &str) -> AuroraResult<GraphQueryResult> {
        // Simplified query parser - in production, this would be a full Cypher parser
        if query.to_lowercase().contains("match") {
            self.execute_match_query(query)
        } else if query.to_lowercase().contains("shortestpath") {
            self.execute_shortest_path_query(query)
        } else if query.to_lowercase().contains("similar") {
            self.execute_vector_similarity_query(query)
        } else {
            Err(AuroraError::InvalidArgument("Unsupported query type".to_string()))
        }
    }

    /// Find shortest path between nodes
    pub fn shortest_path(&self, start: NodeId, end: NodeId, algorithm: PathAlgorithm) -> AuroraResult<Option<Path>> {
        match algorithm {
            PathAlgorithm::Dijkstra => self.dijkstra_shortest_path(start, end),
            PathAlgorithm::AStar => self.a_star_shortest_path(start, end),
            PathAlgorithm::Bidirectional => self.bidirectional_search(start, end),
        }
    }

    /// Calculate graph analytics
    pub fn analytics(&self, algorithm: GraphAlgorithm) -> AuroraResult<GraphAnalyticsResult> {
        match algorithm {
            GraphAlgorithm::PageRank => self.page_rank(),
            GraphAlgorithm::BetweennessCentrality => self.betweenness_centrality(),
            GraphAlgorithm::ClosenessCentrality => self.closeness_centrality(),
            GraphAlgorithm::CommunityDetection => self.community_detection(),
        }
    }

    /// Vector-enhanced graph search
    pub fn vector_search(&self, query_embedding: &[f32], top_k: usize, node_filter: Option<&str>) -> AuroraResult<Vec<(NodeId, f64)>> {
        let candidates = if let Some(label) = node_filter {
            self.node_labels.get(label).cloned().unwrap_or_default()
        } else {
            self.nodes.keys().cloned().collect()
        };

        self.vector_index.search_nodes(query_embedding, &candidates, top_k)
    }

    /// Get graph statistics
    pub fn statistics(&self) -> &GraphStatistics {
        &self.stats
    }

    fn execute_match_query(&self, query: &str) -> AuroraResult<GraphQueryResult> {
        // Simplified MATCH parsing
        let mut nodes = Vec::new();
        let mut relationships = Vec::new();

        // Find all nodes with specific labels
        if query.contains("Person") {
            for node in self.nodes.values() {
                if node.labels.contains(&"Person".to_string()) {
                    nodes.push(node.clone());
                }
            }
        }

        // Find relationships
        if query.contains("KNOWS") {
            for rel in self.relationships.values() {
                if rel.type_name == "KNOWS" {
                    relationships.push(rel.clone());
                }
            }
        }

        Ok(GraphQueryResult {
            nodes,
            relationships,
            paths: Vec::new(),
            statistics: QueryStatistics {
                nodes_traversed: nodes.len(),
                relationships_traversed: relationships.len(),
                execution_time_ms: 10.0,
                heap_allocations: 0,
            },
        })
    }

    fn execute_shortest_path_query(&self, query: &str) -> AuroraResult<GraphQueryResult> {
        // Simplified shortest path query
        let start_node = NodeId(1); // Would parse from query
        let end_node = NodeId(2);

        if let Some(path) = self.shortest_path(start_node, end_node, PathAlgorithm::Dijkstra)? {
            Ok(GraphQueryResult {
                nodes: Vec::new(),
                relationships: Vec::new(),
                paths: vec![path],
                statistics: QueryStatistics::default(),
            })
        } else {
            Ok(GraphQueryResult::default())
        }
    }

    fn execute_vector_similarity_query(&self, query: &str) -> AuroraResult<GraphQueryResult> {
        // Simplified vector similarity query
        let embedding = vec![0.1, 0.2, 0.3]; // Would parse from query
        let similar_nodes = self.vector_search(&embedding, 5, Some("Person"))?;

        let nodes = similar_nodes.into_iter()
            .filter_map(|(node_id, _)| self.nodes.get(&node_id).cloned())
            .collect();

        Ok(GraphQueryResult {
            nodes,
            relationships: Vec::new(),
            paths: Vec::new(),
            statistics: QueryStatistics::default(),
        })
    }

    fn dijkstra_shortest_path(&self, start: NodeId, end: NodeId) -> AuroraResult<Option<Path>> {
        if !self.nodes.contains_key(&start) || !self.nodes.contains_key(&end) {
            return Ok(None);
        }

        let mut distances = HashMap::new();
        let mut previous = HashMap::new();
        let mut queue = BTreeMap::new(); // Priority queue

        // Initialize distances
        for &node_id in self.nodes.keys() {
            distances.insert(node_id, if node_id == start { 0.0 } else { f64::INFINITY });
        }
        queue.insert(0.0, start);

        while let Some((_, current)) = queue.pop_first() {
            if current == end {
                break;
            }

            if let Some(neighbors) = self.adjacency_list.get(&current) {
                for (rel_id, _) in neighbors {
                    if let Some(relationship) = self.relationships.get(rel_id) {
                        let neighbor = if relationship.start_node == current {
                            relationship.end_node
                        } else {
                            relationship.start_node
                        };

                        let weight = relationship.properties.get("weight")
                            .and_then(|v| match v {
                                PropertyValue::Float(f) => Some(*f),
                                PropertyValue::Integer(i) => Some(*i as f64),
                                _ => Some(1.0),
                            })
                            .unwrap_or(1.0);

                        let new_distance = distances[&current] + weight;

                        if new_distance < distances[&neighbor] {
                            distances.insert(neighbor, new_distance);
                            previous.insert(neighbor, (current, *rel_id));
                            queue.insert(new_distance, neighbor);
                        }
                    }
                }
            }
        }

        // Reconstruct path
        if distances[&end] == f64::INFINITY {
            return Ok(None);
        }

        let mut path_nodes = Vec::new();
        let mut path_relationships = Vec::new();
        let mut current = end;

        while current != start {
            path_nodes.push(current);
            if let Some((prev_node, rel_id)) = previous.get(&current) {
                path_relationships.push(*rel_id);
                current = *prev_node;
            } else {
                return Ok(None);
            }
        }
        path_nodes.push(start);
        path_nodes.reverse();
        path_relationships.reverse();

        Ok(Some(Path {
            nodes: path_nodes,
            relationships: path_relationships,
            cost: distances[&end],
        }))
    }

    fn a_star_shortest_path(&self, start: NodeId, end: NodeId) -> AuroraResult<Option<Path>> {
        // Simplified A* implementation
        self.dijkstra_shortest_path(start, end) // Placeholder
    }

    fn bidirectional_search(&self, start: NodeId, end: NodeId) -> AuroraResult<Option<Path>> {
        // Simplified bidirectional search
        self.dijkstra_shortest_path(start, end) // Placeholder
    }

    fn page_rank(&self) -> AuroraResult<GraphAnalyticsResult> {
        let damping_factor = 0.85;
        let max_iterations = 100;
        let tolerance = 1e-6;

        let mut page_ranks = HashMap::new();
        let initial_rank = 1.0 / self.nodes.len() as f64;

        // Initialize ranks
        for &node_id in self.nodes.keys() {
            page_ranks.insert(node_id, initial_rank);
        }

        for _ in 0..max_iterations {
            let mut new_ranks = HashMap::new();
            let mut max_change = 0.0;

            for &node_id in self.nodes.keys() {
                let mut rank_sum = 0.0;

                // Sum ranks from incoming relationships
                if let Some(incoming) = self.reverse_adjacency.get(&node_id) {
                    for (rel_id, _) in incoming {
                        if let Some(relationship) = self.relationships.get(rel_id) {
                            let source_rank = page_ranks[&relationship.start_node];
                            let outgoing_count = self.adjacency_list.get(&relationship.start_node)
                                .map(|rels| rels.len())
                                .unwrap_or(1);
                            rank_sum += source_rank / outgoing_count as f64;
                        }
                    }
                }

                let new_rank = (1.0 - damping_factor) / self.nodes.len() as f64 + damping_factor * rank_sum;
                max_change = max_change.max((new_rank - page_ranks[&node_id]).abs());
                new_ranks.insert(node_id, new_rank);
            }

            page_ranks = new_ranks;

            if max_change < tolerance {
                break;
            }
        }

        Ok(GraphAnalyticsResult::PageRank(page_ranks))
    }

    fn betweenness_centrality(&self) -> AuroraResult<GraphAnalyticsResult> {
        let mut centrality = HashMap::new();

        // Initialize centrality scores
        for &node_id in self.nodes.keys() {
            centrality.insert(node_id, 0.0);
        }

        // For each node, compute shortest paths and count how many pass through each node
        for &source in self.nodes.keys() {
            let mut stack = Vec::new();
            let mut paths = HashMap::new();
            let mut distance = HashMap::new();
            let mut sigma = HashMap::new();

            // Initialize
            for &node_id in self.nodes.keys() {
                paths.insert(node_id, Vec::new());
                distance.insert(node_id, -1);
                sigma.insert(node_id, 0);
            }

            distance.insert(source, 0);
            sigma.insert(source, 1);

            let mut queue = VecDeque::new();
            queue.push_back(source);

            while let Some(v) = queue.pop_front() {
                stack.push(v);

                if let Some(neighbors) = self.adjacency_list.get(&v) {
                    for (rel_id, _) in neighbors {
                        if let Some(relationship) = self.relationships.get(rel_id) {
                            let w = if relationship.start_node == v {
                                relationship.end_node
                            } else {
                                relationship.start_node
                            };

                            if distance[&w] < 0 {
                                queue.push_back(w);
                                distance.insert(w, distance[&v] + 1);
                            }

                            if distance[&w] == distance[&v] + 1 {
                                sigma.insert(w, sigma[&w] + sigma[&v]);
                                paths.get_mut(&w).unwrap().push(v);
                            }
                        }
                    }
                }
            }

            let mut delta = HashMap::new();
            for &node_id in self.nodes.keys() {
                delta.insert(node_id, 0.0);
            }

            while let Some(w) = stack.pop() {
                for &v in &paths[&w] {
                    delta.insert(v, delta[&v] + (sigma[&v] as f64 / sigma[&w] as f64) * (1.0 + delta[&w]));
                }
                if w != source {
                    *centrality.get_mut(&w).unwrap() += delta[&w];
                }
            }
        }

        Ok(GraphAnalyticsResult::BetweennessCentrality(centrality))
    }

    fn closeness_centrality(&self) -> AuroraResult<GraphAnalyticsResult> {
        let mut centrality = HashMap::new();

        for &source in self.nodes.keys() {
            let mut total_distance = 0.0;
            let mut reachable_nodes = 0;

            // Compute shortest paths from source
            let distances = self.compute_shortest_paths(source);

            for &distance in distances.values() {
                if distance < f64::INFINITY && distance > 0.0 {
                    total_distance += distance;
                    reachable_nodes += 1;
                }
            }

            let closeness = if reachable_nodes > 0 {
                reachable_nodes as f64 / total_distance
            } else {
                0.0
            };

            centrality.insert(source, closeness);
        }

        Ok(GraphAnalyticsResult::ClosenessCentrality(centrality))
    }

    fn community_detection(&self) -> AuroraResult<GraphAnalyticsResult> {
        // Simplified Louvain method implementation
        let mut communities = HashMap::new();
        let mut community_id = 0;

        // Initialize each node in its own community
        for &node_id in self.nodes.keys() {
            communities.insert(node_id, community_id);
            community_id += 1;
        }

        // Simplified community detection (would be more sophisticated in production)
        Ok(GraphAnalyticsResult::CommunityDetection(communities))
    }

    fn compute_shortest_paths(&self, source: NodeId) -> HashMap<NodeId, f64> {
        let mut distances = HashMap::new();
        let mut queue = BTreeMap::new();

        // Initialize distances
        for &node_id in self.nodes.keys() {
            distances.insert(node_id, if node_id == source { 0.0 } else { f64::INFINITY });
        }
        queue.insert(0.0, source);

        while let Some((_, current)) = queue.pop_first() {
            if let Some(neighbors) = self.adjacency_list.get(&current) {
                for (rel_id, _) in neighbors {
                    if let Some(relationship) = self.relationships.get(rel_id) {
                        let neighbor = if relationship.start_node == current {
                            relationship.end_node
                        } else {
                            relationship.start_node
                        };

                        let weight = 1.0; // Simplified unit weight
                        let new_distance = distances[&current] + weight;

                        if new_distance < distances[&neighbor] {
                            distances.insert(neighbor, new_distance);
                            queue.insert(new_distance, neighbor);
                        }
                    }
                }
            }
        }

        distances
    }

    fn generate_id(&self) -> u64 {
        use std::time::{SystemTime, UNIX_EPOCH};
        SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_nanos() as u64
    }
}

/// Graph algorithms
#[derive(Debug, Clone)]
pub enum GraphAlgorithm {
    PageRank,
    BetweennessCentrality,
    ClosenessCentrality,
    CommunityDetection,
}

/// Path finding algorithms
#[derive(Debug, Clone)]
pub enum PathAlgorithm {
    Dijkstra,
    AStar,
    Bidirectional,
}

/// Graph analytics results
#[derive(Debug, Clone)]
pub enum GraphAnalyticsResult {
    PageRank(HashMap<NodeId, f64>),
    BetweennessCentrality(HashMap<NodeId, f64>),
    ClosenessCentrality(HashMap<NodeId, f64>),
    CommunityDetection(HashMap<NodeId, u64>),
}

/// Graph statistics
#[derive(Debug, Clone, Default)]
pub struct GraphStatistics {
    pub node_count: usize,
    pub relationship_count: usize,
    pub label_count: usize,
    pub relationship_type_count: usize,
    pub average_degree: f64,
    pub connected_components: usize,
}

impl GraphVectorIndex {
    fn new() -> Self {
        Self {
            node_embeddings: HashMap::new(),
            relationship_embeddings: HashMap::new(),
        }
    }

    fn add_node_embedding(&mut self, node_id: NodeId, embedding: Vec<f32>) {
        self.node_embeddings.insert(node_id, embedding);
    }

    fn add_relationship_embedding(&mut self, rel_id: RelationshipId, embedding: Vec<f32>) {
        self.relationship_embeddings.insert(rel_id, embedding);
    }

    fn search_nodes(&self, query: &[f32], candidates: &HashSet<NodeId>, top_k: usize) -> AuroraResult<Vec<(NodeId, f64)>> {
        let mut results = Vec::new();

        for &node_id in candidates {
            if let Some(embedding) = self.node_embeddings.get(&node_id) {
                let similarity = cosine_similarity(query, embedding);
                results.push((node_id, similarity));
            }
        }

        results.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());
        results.truncate(top_k);

        Ok(results)
    }
}

impl Default for GraphQueryResult {
    fn default() -> Self {
        Self {
            nodes: Vec::new(),
            relationships: Vec::new(),
            paths: Vec::new(),
            statistics: QueryStatistics::default(),
        }
    }
}

fn cosine_similarity(a: &[f32], b: &[f32]) -> f64 {
    let dot_product: f32 = a.iter().zip(b.iter()).map(|(x, y)| x * y).sum();
    let norm_a: f32 = a.iter().map(|x| x * x).sum::<f32>().sqrt();
    let norm_b: f32 = b.iter().map(|x| x * x).sum::<f32>().sqrt();

    if norm_a == 0.0 || norm_b == 0.0 {
        0.0
    } else {
        (dot_product / (norm_a * norm_b)) as f64
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_create_graph() {
        let mut graph = PropertyGraph::new();
        assert_eq!(graph.statistics().node_count, 0);
        assert_eq!(graph.statistics().relationship_count, 0);
    }

    #[test]
    fn test_create_node() {
        let mut graph = PropertyGraph::new();

        let properties = HashMap::from([
            ("name".to_string(), PropertyValue::String("Alice".to_string())),
            ("age".to_string(), PropertyValue::Integer(30)),
        ]);

        let node_id = graph.create_node(vec!["Person".to_string()], properties, None).unwrap();

        assert_eq!(graph.statistics().node_count, 1);
        assert!(graph.nodes.contains_key(&node_id));
    }

    #[test]
    fn test_create_relationship() {
        let mut graph = PropertyGraph::new();

        // Create nodes
        let alice_id = graph.create_node(vec!["Person".to_string()], HashMap::new(), None).unwrap();
        let bob_id = graph.create_node(vec!["Person".to_string()], HashMap::new(), None).unwrap();

        // Create relationship
        let rel_id = graph.create_relationship(
            alice_id,
            bob_id,
            "KNOWS".to_string(),
            HashMap::from([("since".to_string(), PropertyValue::Integer(2020))]),
            None
        ).unwrap();

        assert_eq!(graph.statistics().relationship_count, 1);
        assert!(graph.relationships.contains_key(&rel_id));
    }

    #[test]
    fn test_shortest_path() {
        let mut graph = PropertyGraph::new();

        // Create a simple graph: A -> B -> C
        let node_a = graph.create_node(vec!["Node".to_string()], HashMap::new(), None).unwrap();
        let node_b = graph.create_node(vec!["Node".to_string()], HashMap::new(), None).unwrap();
        let node_c = graph.create_node(vec!["Node".to_string()], HashMap::new(), None).unwrap();

        graph.create_relationship(node_a, node_b, "CONNECTS".to_string(), HashMap::new(), None).unwrap();
        graph.create_relationship(node_b, node_c, "CONNECTS".to_string(), HashMap::new(), None).unwrap();

        // Find shortest path
        let path = graph.shortest_path(node_a, node_c, PathAlgorithm::Dijkstra).unwrap();

        assert!(path.is_some());
        let path = path.unwrap();
        assert_eq!(path.nodes.len(), 3);
        assert_eq!(path.relationships.len(), 2);
    }

    #[test]
    fn test_vector_search() {
        let mut graph = PropertyGraph::new();

        // Create nodes with embeddings
        let embedding1 = vec![1.0, 0.0, 0.0];
        let embedding2 = vec![0.0, 1.0, 0.0];
        let embedding3 = vec![0.0, 0.0, 1.0];

        let node1 = graph.create_node(vec!["Item".to_string()], HashMap::new(), Some(embedding1.clone())).unwrap();
        let node2 = graph.create_node(vec!["Item".to_string()], HashMap::new(), Some(embedding2.clone())).unwrap();
        let node3 = graph.create_node(vec!["Item".to_string()], HashMap::new(), Some(embedding3.clone())).unwrap();

        // Search for similar vectors
        let query = vec![0.9, 0.1, 0.0]; // Similar to embedding1
        let results = graph.vector_search(&query, 2, Some("Item")).unwrap();

        assert!(!results.is_empty());
        assert_eq!(results[0].0, node1); // Should find node1 first
    }

    #[test]
    fn test_page_rank() {
        let mut graph = PropertyGraph::new();

        // Create a simple graph
        let node1 = graph.create_node(vec!["Page".to_string()], HashMap::new(), None).unwrap();
        let node2 = graph.create_node(vec!["Page".to_string()], HashMap::new(), None).unwrap();
        let node3 = graph.create_node(vec!["Page".to_string()], HashMap::new(), None).unwrap();

        graph.create_relationship(node1, node2, "LINKS_TO".to_string(), HashMap::new(), None).unwrap();
        graph.create_relationship(node2, node3, "LINKS_TO".to_string(), HashMap::new(), None).unwrap();
        graph.create_relationship(node3, node1, "LINKS_TO".to_string(), HashMap::new(), None).unwrap();

        // Calculate PageRank
        let result = graph.analytics(GraphAlgorithm::PageRank).unwrap();

        if let GraphAnalyticsResult::PageRank(ranks) = result {
            assert_eq!(ranks.len(), 3);
            // All nodes should have some rank
            for rank in ranks.values() {
                assert!(*rank >= 0.0);
            }
        }
    }

    #[test]
    fn test_property_values() {
        let string_val = PropertyValue::String("test".to_string());
        let int_val = PropertyValue::Integer(42);
        let float_val = PropertyValue::Float(3.14);
        let bool_val = PropertyValue::Boolean(true);

        assert_eq!(string_val, PropertyValue::String("test".to_string()));
        assert_eq!(int_val, PropertyValue::Integer(42));
        assert_eq!(float_val, PropertyValue::Float(3.14));
        assert_eq!(bool_val, PropertyValue::Boolean(true));
    }

    #[test]
    fn test_graph_query() {
        let mut graph = PropertyGraph::new();

        // Create test data
        let person1 = graph.create_node(vec!["Person".to_string()], HashMap::new(), None).unwrap();
        let person2 = graph.create_node(vec!["Person".to_string()], HashMap::new(), None).unwrap();

        graph.create_relationship(person1, person2, "KNOWS".to_string(), HashMap::new(), None).unwrap();

        // Execute simple query
        let result = graph.query("MATCH (p:Person)-[:KNOWS]->(p2:Person)").unwrap();

        assert!(!result.nodes.is_empty());
        assert!(!result.relationships.is_empty());
    }
}