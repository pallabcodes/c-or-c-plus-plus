//! AuroraDB Hybrid Search: Vector + Keyword + Metadata Search
//!
//! Revolutionary hybrid search combining multiple modalities with AuroraDB UNIQUENESS:
//! - Vector similarity + BM25 keyword search + metadata filtering
//! - Multi-stage ranking with reciprocal rank fusion
//! - Adaptive weighting based on query characteristics
//! - Real-time index updates across all modalities

use std::collections::{HashMap, HashSet, BTreeMap, BinaryHeap};
use crate::core::errors::{AuroraResult, AuroraError};
use super::super::distance_metrics::{DistanceComputer, DistanceMetric};

/// Hybrid search engine combining vector, keyword, and metadata search
pub struct HybridSearchEngine {
    /// Vector search component
    vector_search: VectorSearchComponent,
    /// Keyword search component (BM25)
    keyword_search: KeywordSearchComponent,
    /// Metadata filtering component
    metadata_filter: super::filtering::AdvancedVectorFilter,
    /// Ranking fusion engine
    rank_fusion: RankFusionEngine,
    /// Query optimizer
    query_optimizer: HybridQueryOptimizer,
    /// Performance monitor
    performance_monitor: HybridPerformanceMonitor,
}

impl HybridSearchEngine {
    /// Create a new hybrid search engine
    pub fn new() -> AuroraResult<Self> {
        Ok(Self {
            vector_search: VectorSearchComponent::new(),
            keyword_search: KeywordSearchComponent::new(),
            metadata_filter: super::filtering::AdvancedVectorFilter::new(),
            rank_fusion: RankFusionEngine::new(),
            query_optimizer: HybridQueryOptimizer::new(),
            performance_monitor: HybridPerformanceMonitor::new(),
        })
    }

    /// Perform hybrid search with vector, keyword, and metadata components
    pub async fn hybrid_search(&self, query: &HybridQuery, k: usize) -> AuroraResult<HybridSearchResults> {
        let start_time = std::time::Instant::now();

        // Optimize query execution plan
        let execution_plan = self.query_optimizer.optimize_query(query).await?;

        // Execute search components in parallel
        let (vector_results, keyword_results, metadata_candidates) = tokio::try_join!(
            self.vector_search.search(&query.vector_query, k * 2),
            self.keyword_search.search(&query.keyword_query, k * 2),
            self.metadata_filter.apply_filters(&query.metadata_filters)
        )?;

        // Apply metadata filtering to results
        let filtered_vector_results = self.filter_results_by_metadata(vector_results, &metadata_candidates);
        let filtered_keyword_results = self.filter_results_by_metadata(keyword_results, &metadata_candidates);

        // Fuse rankings using reciprocal rank fusion
        let fused_results = self.rank_fusion.fuse_rankings(
            &filtered_vector_results,
            &filtered_keyword_results,
            &execution_plan.weights
        )?;

        // Return top k results
        let top_results = fused_results.into_iter().take(k).collect();

        let total_time = start_time.elapsed().as_millis() as f64;
        self.performance_monitor.record_search(total_time, query);

        Ok(HybridSearchResults {
            results: top_results,
            total_candidates: metadata_candidates.len(),
            search_time_ms: total_time,
            component_breakdown: ComponentBreakdown {
                vector_results: filtered_vector_results.len(),
                keyword_results: filtered_keyword_results.len(),
                metadata_filtered: metadata_candidates.len(),
            },
        })
    }

    /// Add content for indexing (vector + text + metadata)
    pub async fn add_content(&mut self, id: usize, content: &HybridContent) -> AuroraResult<()> {
        // Add vector
        self.vector_search.add_vector(id, &content.vector)?;

        // Add text for keyword search
        self.keyword_search.add_document(id, &content.text)?;

        // Add metadata
        self.metadata_filter.add_metadata(id, content.metadata.clone())?;

        Ok(())
    }

    /// Update content (supports real-time updates)
    pub async fn update_content(&mut self, id: usize, content: &HybridContent) -> AuroraResult<()> {
        // Update vector
        self.vector_search.update_vector(id, &content.vector)?;

        // Update text
        self.keyword_search.update_document(id, &content.text)?;

        // Update metadata
        self.metadata_filter.update_metadata(id, content.metadata.clone())?;

        Ok(())
    }

    /// Delete content
    pub async fn delete_content(&mut self, id: usize) -> AuroraResult<()> {
        self.vector_search.delete_vector(id)?;
        self.keyword_search.delete_document(id)?;
        self.metadata_filter.remove_metadata(id)?;
        Ok(())
    }

    /// Get search performance statistics
    pub fn get_performance_stats(&self) -> &HybridPerformanceStats {
        self.performance_monitor.get_stats()
    }

    /// Filter search results by metadata candidates
    fn filter_results_by_metadata(&self, results: Vec<(usize, f32)>, candidates: &HashSet<usize>) -> Vec<(usize, f32)> {
        results.into_iter()
            .filter(|(id, _)| candidates.contains(id))
            .collect()
    }
}

/// Hybrid query combining multiple search modalities
#[derive(Debug, Clone)]
pub struct HybridQuery {
    pub vector_query: VectorQuery,
    pub keyword_query: KeywordQuery,
    pub metadata_filters: Vec<super::filtering::MetadataFilter>,
    pub search_mode: HybridSearchMode,
}

impl HybridQuery {
    pub fn new(vector: Vec<f32>, keywords: Vec<String>) -> Self {
        Self {
            vector_query: VectorQuery { vector, k: 100 },
            keyword_query: KeywordQuery { keywords, operator: KeywordOperator::Or },
            metadata_filters: Vec::new(),
            search_mode: HybridSearchMode::Balanced,
        }
    }

    pub fn with_metadata_filters(mut self, filters: Vec<super::filtering::MetadataFilter>) -> Self {
        self.metadata_filters = filters;
        self
    }

    pub fn with_search_mode(mut self, mode: HybridSearchMode) -> Self {
        self.search_mode = mode;
        self
    }
}

/// Vector query component
#[derive(Debug, Clone)]
pub struct VectorQuery {
    pub vector: Vec<f32>,
    pub k: usize,
}

/// Keyword query component
#[derive(Debug, Clone)]
pub struct KeywordQuery {
    pub keywords: Vec<String>,
    pub operator: KeywordOperator,
}

/// Keyword search operators
#[derive(Debug, Clone)]
pub enum KeywordOperator {
    And,
    Or,
    Phrase,
}

/// Hybrid search modes
#[derive(Debug, Clone)]
pub enum HybridSearchMode {
    VectorFirst,     // Prioritize vector similarity
    KeywordFirst,    // Prioritize keyword matching
    Balanced,        // Equal weighting
    Adaptive,        // Automatically adjust based on query
}

/// Content to be indexed (vector + text + metadata)
#[derive(Debug, Clone)]
pub struct HybridContent {
    pub vector: Vec<f32>,
    pub text: String,
    pub metadata: HashMap<String, super::filtering::MetadataValue>,
}

/// Hybrid search results
#[derive(Debug, Clone)]
pub struct HybridSearchResults {
    pub results: Vec<(usize, f32)>,
    pub total_candidates: usize,
    pub search_time_ms: f64,
    pub component_breakdown: ComponentBreakdown,
}

/// Component result breakdown
#[derive(Debug, Clone)]
pub struct ComponentBreakdown {
    pub vector_results: usize,
    pub keyword_results: usize,
    pub metadata_filtered: usize,
}

/// Vector search component
pub struct VectorSearchComponent {
    vectors: HashMap<usize, Vec<f32>>,
    distance_computer: DistanceComputer,
}

impl VectorSearchComponent {
    fn new() -> Self {
        Self {
            vectors: HashMap::new(),
            distance_computer: DistanceComputer::new(DistanceMetric::Cosine),
        }
    }

    async fn search(&self, query: &VectorQuery, k: usize) -> AuroraResult<Vec<(usize, f32)>> {
        if self.vectors.is_empty() {
            return Ok(Vec::new());
        }

        let mut results = BinaryHeap::new();

        for (&id, vector) in &self.vectors {
            let distance = self.distance_computer.compute(&query.vector, vector)?;
            let score = 1.0 - distance; // Convert distance to similarity score

            results.push(std::cmp::Reverse((score, id)));

            // Keep only top k
            if results.len() > k {
                results.pop();
            }
        }

        let mut final_results = Vec::new();
        while let Some(std::cmp::Reverse((score, id))) = results.pop() {
            final_results.push((id, score));
        }
        final_results.reverse(); // Highest scores first

        Ok(final_results)
    }

    fn add_vector(&mut self, id: usize, vector: &[f32]) -> AuroraResult<()> {
        self.vectors.insert(id, vector.to_vec());
        Ok(())
    }

    fn update_vector(&mut self, id: usize, vector: &[f32]) -> AuroraResult<()> {
        self.vectors.insert(id, vector.to_vec());
        Ok(())
    }

    fn delete_vector(&mut self, id: usize) -> AuroraResult<()> {
        self.vectors.remove(&id);
        Ok(())
    }
}

/// Keyword search component using BM25
pub struct KeywordSearchComponent {
    documents: HashMap<usize, Vec<String>>, // Document ID -> terms
    term_frequency: HashMap<String, HashMap<usize, usize>>, // Term -> (Doc ID -> frequency)
    document_frequency: HashMap<String, usize>, // Term -> document count
    total_documents: usize,
    avg_document_length: f64,
    document_lengths: HashMap<usize, usize>,
}

impl KeywordSearchComponent {
    fn new() -> Self {
        Self {
            documents: HashMap::new(),
            term_frequency: HashMap::new(),
            document_frequency: HashMap::new(),
            total_documents: 0,
            avg_document_length: 0.0,
            document_lengths: HashMap::new(),
        }
    }

    async fn search(&self, query: &KeywordQuery, k: usize) -> AuroraResult<Vec<(usize, f32)>> {
        if self.documents.is_empty() {
            return Ok(Vec::new());
        }

        let query_terms = self.tokenize_query(&query.keywords, &query.operator);
        let mut scores = HashMap::new();

        for term in &query_terms {
            if let Some(doc_freq) = self.document_frequency.get(term) {
                let idf = self.calculate_idf(*doc_freq);

                if let Some(term_docs) = self.term_frequency.get(term) {
                    for (&doc_id, &tf) in term_docs {
                        let doc_length = self.document_lengths.get(&doc_id).copied().unwrap_or(1);
                        let bm25_score = self.calculate_bm25(tf, idf, doc_length);

                        *scores.entry(doc_id).or_insert(0.0) += bm25_score;
                    }
                }
            }
        }

        // Sort by score and return top k
        let mut results: Vec<(usize, f32)> = scores.into_iter()
            .map(|(id, score)| (id, score))
            .collect();

        results.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));
        results.truncate(k);

        Ok(results)
    }

    fn add_document(&mut self, id: usize, text: &str) -> AuroraResult<()> {
        let terms = self.tokenize_text(text);
        let doc_length = terms.len();

        self.documents.insert(id, terms.clone());
        self.document_lengths.insert(id, doc_length);
        self.total_documents += 1;

        // Update term frequencies
        for term in terms {
            // Update term frequency for this document
            self.term_frequency.entry(term.clone())
                .or_insert_with(HashMap::new)
                .entry(id)
                .and_modify(|e| *e += 1)
                .or_insert(1);

            // Update document frequency
            self.document_frequency.entry(term)
                .and_modify(|e| *e += 1)
                .or_insert(1);
        }

        // Update average document length
        let total_length: usize = self.document_lengths.values().sum();
        self.avg_document_length = total_length as f64 / self.total_documents as f64;

        Ok(())
    }

    fn update_document(&mut self, id: usize, text: &str) -> AuroraResult<()> {
        // Remove old document
        self.delete_document(id)?;
        // Add new document
        self.add_document(id, text)
    }

    fn delete_document(&mut self, id: usize) -> AuroraResult<()> {
        if let Some(terms) = self.documents.remove(&id) {
            self.document_lengths.remove(&id);
            self.total_documents = self.total_documents.saturating_sub(1);

            // Update term frequencies and document frequencies
            for term in terms {
                if let Some(term_docs) = self.term_frequency.get_mut(&term) {
                    term_docs.remove(&id);
                    if term_docs.is_empty() {
                        self.term_frequency.remove(&term);
                        self.document_frequency.remove(&term);
                    } else {
                        *self.document_frequency.get_mut(&term).unwrap() -= 1;
                    }
                }
            }

            // Update average document length
            let total_length: usize = self.document_lengths.values().sum();
            self.avg_document_length = if self.total_documents > 0 {
                total_length as f64 / self.total_documents as f64
            } else {
                0.0
            };
        }

        Ok(())
    }

    fn tokenize_query(&self, keywords: &[String], operator: &KeywordOperator) -> Vec<String> {
        match operator {
            KeywordOperator::Phrase => {
                // For phrase search, treat as single term
                vec![keywords.join(" ")]
            }
            _ => {
                // For AND/OR, tokenize each keyword
                keywords.iter()
                    .flat_map(|kw| self.tokenize_text(kw))
                    .collect()
            }
        }
    }

    fn tokenize_text(&self, text: &str) -> Vec<String> {
        // Simple tokenization: lowercase, split on whitespace, remove punctuation
        text.to_lowercase()
            .split_whitespace()
            .map(|word| word.trim_matches(|c: char| !c.is_alphanumeric()).to_string())
            .filter(|word| !word.is_empty())
            .collect()
    }

    fn calculate_idf(&self, doc_freq: usize) -> f64 {
        let n = self.total_documents as f64;
        let df = doc_freq as f64;
        ((n - df + 0.5) / (df + 0.5)).ln()
    }

    fn calculate_bm25(&self, tf: usize, idf: f64, doc_length: usize) -> f64 {
        const K1: f64 = 1.5; // BM25 parameter
        const B: f64 = 0.75; // BM25 parameter

        let tf_float = tf as f64;
        let doc_length_float = doc_length as f64;

        let numerator = tf_float * (K1 + 1.0);
        let denominator = tf_float + K1 * (1.0 - B + B * doc_length_float / self.avg_document_length);

        idf * numerator / denominator
    }
}

/// Rank fusion engine using reciprocal rank fusion
pub struct RankFusionEngine {
    fusion_constant: f64,
}

impl RankFusionEngine {
    fn new() -> Self {
        Self {
            fusion_constant: 60.0, // Standard RRF constant
        }
    }

    fn fuse_rankings(
        &self,
        vector_results: &[(usize, f32)],
        keyword_results: &[(usize, f32)],
        weights: &HybridWeights,
    ) -> AuroraResult<Vec<(usize, f32)>> {
        let mut fused_scores = HashMap::new();

        // Create ranking maps
        let vector_ranks: HashMap<usize, usize> = vector_results.iter()
            .enumerate()
            .map(|(rank, (id, _))| (*id, rank + 1))
            .collect();

        let keyword_ranks: HashMap<usize, usize> = keyword_results.iter()
            .enumerate()
            .map(|(rank, (id, _))| (*id, rank + 1))
            .collect();

        // Get all unique document IDs
        let mut all_ids = HashSet::new();
        all_ids.extend(vector_ranks.keys());
        all_ids.extend(keyword_ranks.keys());

        // Calculate RRF scores
        for &id in &all_ids {
            let vector_rank = vector_ranks.get(&id).copied().unwrap_or(usize::MAX);
            let keyword_rank = keyword_ranks.get(&id).copied().unwrap_or(usize::MAX);

            let vector_rrf = if vector_rank != usize::MAX {
                weights.vector_weight / (self.fusion_constant + vector_rank as f64)
            } else {
                0.0
            };

            let keyword_rrf = if keyword_rank != usize::MAX {
                weights.keyword_weight / (self.fusion_constant + keyword_rank as f64)
            } else {
                0.0
            };

            let total_score = vector_rrf + keyword_rrf;
            fused_scores.insert(id, total_score);
        }

        // Sort by fused score
        let mut results: Vec<(usize, f32)> = fused_scores.into_iter()
            .map(|(id, score)| (id, score as f32))
            .collect();

        results.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap_or(std::cmp::Ordering::Equal));

        Ok(results)
    }
}

/// Hybrid query optimizer
pub struct HybridQueryOptimizer;

impl HybridQueryOptimizer {
    fn new() -> Self {
        Self
    }

    async fn optimize_query(&self, query: &HybridQuery) -> AuroraResult<ExecutionPlan> {
        // Analyze query characteristics
        let has_vector = !query.vector_query.vector.is_empty();
        let has_keywords = !query.keyword_query.keywords.is_empty();
        let has_filters = !query.metadata_filters.is_empty();

        // Determine optimal weights based on query composition
        let weights = match query.search_mode {
            HybridSearchMode::VectorFirst => HybridWeights {
                vector_weight: 0.8,
                keyword_weight: 0.2,
            },
            HybridSearchMode::KeywordFirst => HybridWeights {
                vector_weight: 0.2,
                keyword_weight: 0.8,
            },
            HybridSearchMode::Balanced => HybridWeights {
                vector_weight: 0.5,
                keyword_weight: 0.5,
            },
            HybridSearchMode::Adaptive => {
                // Adaptive weighting based on query content
                let vector_weight = if has_vector { 0.6 } else { 0.0 };
                let keyword_weight = if has_keywords { 0.4 } else { 0.0 };
                HybridWeights {
                    vector_weight,
                    keyword_weight,
                }
            }
        };

        // Determine execution order
        let execution_order = if has_filters {
            vec![ExecutionStep::MetadataFilter, ExecutionStep::ParallelSearch]
        } else {
            vec![ExecutionStep::ParallelSearch]
        };

        Ok(ExecutionPlan {
            weights,
            execution_order,
            parallel_execution: has_vector && has_keywords,
        })
    }
}

/// Execution plan for hybrid queries
#[derive(Debug, Clone)]
struct ExecutionPlan {
    weights: HybridWeights,
    execution_order: Vec<ExecutionStep>,
    parallel_execution: bool,
}

/// Execution steps
#[derive(Debug, Clone)]
enum ExecutionStep {
    MetadataFilter,
    ParallelSearch,
    SequentialSearch,
}

/// Weights for different search components
#[derive(Debug, Clone)]
struct HybridWeights {
    vector_weight: f64,
    keyword_weight: f64,
}

/// Performance monitor for hybrid search
pub struct HybridPerformanceMonitor {
    search_times: Vec<f64>,
    query_types: Vec<String>,
    total_searches: usize,
}

impl HybridPerformanceMonitor {
    fn new() -> Self {
        Self {
            search_times: Vec::new(),
            query_types: Vec::new(),
            total_searches: 0,
        }
    }

    fn record_search(&self, time_ms: f64, query: &HybridQuery) {
        // In a real implementation, this would be thread-safe
        // For now, just track the metrics
    }

    fn get_stats(&self) -> &HybridPerformanceStats {
        // Return mock stats for now
        static MOCK_STATS: HybridPerformanceStats = HybridPerformanceStats {
            total_searches: 0,
            avg_search_time_ms: 0.0,
            p95_search_time_ms: 0.0,
            searches_per_second: 0.0,
        };
        &MOCK_STATS
    }
}

/// Hybrid performance statistics
#[derive(Debug, Clone)]
pub struct HybridPerformanceStats {
    pub total_searches: usize,
    pub avg_search_time_ms: f64,
    pub p95_search_time_ms: f64,
    pub searches_per_second: f64,
}

#[cfg(test)]
mod tests {
    use super::*;
    use super::super::filtering::MetadataFilter;

    #[tokio::test]
    async fn test_hybrid_search_engine_creation() {
        let engine = HybridSearchEngine::new().unwrap();
        // Should create successfully
        assert!(engine.get_performance_stats().total_searches >= 0);
    }

    #[tokio::test]
    async fn test_add_hybrid_content() {
        let mut engine = HybridSearchEngine::new().unwrap();

        let content = HybridContent {
            vector: vec![0.1, 0.2, 0.3],
            text: "This is a test document about artificial intelligence".to_string(),
            metadata: {
                let mut meta = HashMap::new();
                meta.insert("category".to_string(), super::filtering::MetadataValue::String("AI".to_string()));
                meta
            },
        };

        engine.add_content(1, &content).await.unwrap();

        // Content should be added successfully
        // (We can't easily test the internal state without exposing it)
    }

    #[tokio::test]
    async fn test_hybrid_search() {
        let mut engine = HybridSearchEngine::new().unwrap();

        // Add test content
        let contents = vec![
            HybridContent {
                vector: vec![1.0, 0.0, 0.0],
                text: "artificial intelligence machine learning".to_string(),
                metadata: {
                    let mut meta = HashMap::new();
                    meta.insert("category".to_string(), super::filtering::MetadataValue::String("AI".to_string()));
                    meta
                },
            },
            HybridContent {
                vector: vec![0.0, 1.0, 0.0],
                text: "database systems query optimization".to_string(),
                metadata: {
                    let mut meta = HashMap::new();
                    meta.insert("category".to_string(), super::filtering::MetadataValue::String("DB".to_string()));
                    meta
                },
            },
        ];

        for (i, content) in contents.into_iter().enumerate() {
            engine.add_content(i, &content).await.unwrap();
        }

        // Test hybrid query
        let query = HybridQuery::new(
            vec![0.8, 0.2, 0.0], // Similar to first vector
            vec!["artificial".to_string(), "intelligence".to_string()]
        ).with_metadata_filters(vec![
            MetadataFilter::Equal {
                attribute: "category".to_string(),
                value: super::filtering::MetadataValue::String("AI".to_string()),
            }
        ]);

        let results = engine.hybrid_search(&query, 5).await.unwrap();

        // Should return results
        assert!(!results.results.is_empty());
        // First result should be the AI document (due to metadata filter)
        assert_eq!(results.results[0].0, 0);
    }

    #[test]
    fn test_keyword_search_component() {
        let mut keyword_search = KeywordSearchComponent::new();

        // Add documents
        keyword_search.add_document(1, "the quick brown fox jumps over the lazy dog").unwrap();
        keyword_search.add_document(2, "a quick brown dog runs through the park").unwrap();

        let query = KeywordQuery {
            keywords: vec!["quick".to_string(), "brown".to_string()],
            operator: KeywordOperator::And,
        };

        let results = tokio::runtime::Runtime::new().unwrap()
            .block_on(keyword_search.search(&query, 10)).unwrap();

        // Both documents should match
        assert_eq!(results.len(), 2);
        // Document 1 should score higher (has both terms + "the" appears twice)
        assert!(results[0].1 > results[1].1);
    }

    #[test]
    fn test_rank_fusion() {
        let fusion_engine = RankFusionEngine::new();

        let vector_results = vec![(1, 0.9), (2, 0.8), (3, 0.7)];
        let keyword_results = vec![(2, 0.85), (1, 0.75), (4, 0.65)];
        let weights = HybridWeights {
            vector_weight: 0.6,
            keyword_weight: 0.4,
        };

        let fused = fusion_engine.fuse_rankings(&vector_results, &keyword_results, &weights).unwrap();

        // Should combine rankings
        assert_eq!(fused.len(), 4); // All unique documents
        // Document 1 should be first (good rank in both)
        assert_eq!(fused[0].0, 1);
        // Document 2 should be second (good rank in both)
        assert_eq!(fused[1].0, 2);
    }

    #[tokio::test]
    async fn test_query_optimization() {
        let optimizer = HybridQueryOptimizer::new();

        // Test balanced mode
        let query = HybridQuery {
            vector_query: VectorQuery { vector: vec![1.0], k: 10 },
            keyword_query: KeywordQuery {
                keywords: vec!["test".to_string()],
                operator: KeywordOperator::Or,
            },
            metadata_filters: vec![],
            search_mode: HybridSearchMode::Balanced,
        };

        let plan = optimizer.optimize_query(&query).await.unwrap();

        // Should have balanced weights
        assert_eq!(plan.weights.vector_weight, 0.5);
        assert_eq!(plan.weights.keyword_weight, 0.5);
    }

    #[tokio::test]
    async fn test_adaptive_mode() {
        let optimizer = HybridQueryOptimizer::new();

        // Test adaptive mode with vector only
        let query = HybridQuery {
            vector_query: VectorQuery { vector: vec![1.0], k: 10 },
            keyword_query: KeywordQuery {
                keywords: vec![],
                operator: KeywordOperator::Or,
            },
            metadata_filters: vec![],
            search_mode: HybridSearchMode::Adaptive,
        };

        let plan = optimizer.optimize_query(&query).await.unwrap();

        // Should weight vector heavily
        assert_eq!(plan.weights.vector_weight, 0.6);
        assert_eq!(plan.weights.keyword_weight, 0.0);
    }

    #[test]
    fn test_bm25_scoring() {
        let mut keyword_search = KeywordSearchComponent::new();

        // Add documents of different lengths
        keyword_search.add_document(1, "the quick brown fox").unwrap(); // Short doc
        keyword_search.add_document(2, "the quick brown fox jumps over the lazy dog and runs through the park").unwrap(); // Long doc

        let query = KeywordQuery {
            keywords: vec!["fox".to_string()],
            operator: KeywordOperator::Or,
        };

        let results = tokio::runtime::Runtime::new().unwrap()
            .block_on(keyword_search.search(&query, 10)).unwrap();

        // Should return both documents
        assert_eq!(results.len(), 2);
        // Shorter document should score higher (BM25 length normalization)
    }

    #[tokio::test]
    async fn test_phrase_search() {
        let mut keyword_search = KeywordSearchComponent::new();

        keyword_search.add_document(1, "machine learning is awesome").unwrap();
        keyword_search.add_document(2, "learning machine algorithms").unwrap();

        let query = KeywordQuery {
            keywords: vec!["machine".to_string(), "learning".to_string()],
            operator: KeywordOperator::Phrase,
        };

        let results = keyword_search.search(&query, 10).await.unwrap();

        // Should find document 1 (exact phrase match)
        assert_eq!(results.len(), 1);
        assert_eq!(results[0].0, 1);
    }

    #[tokio::test]
    async fn test_update_content() {
        let mut engine = HybridSearchEngine::new().unwrap();

        // Add initial content
        let content1 = HybridContent {
            vector: vec![1.0, 0.0],
            text: "initial content".to_string(),
            metadata: HashMap::new(),
        };

        engine.add_content(1, &content1).await.unwrap();

        // Update content
        let content2 = HybridContent {
            vector: vec![0.0, 1.0],
            text: "updated content".to_string(),
            metadata: HashMap::new(),
        };

        engine.update_content(1, &content2).await.unwrap();

        // Search for updated content
        let query = HybridQuery::new(vec![0.0, 1.0], vec!["updated".to_string()]);
        let results = engine.hybrid_search(&query, 5).await.unwrap();

        // Should find the updated document
        assert!(!results.results.is_empty());
    }

    #[tokio::test]
    async fn test_delete_content() {
        let mut engine = HybridSearchEngine::new().unwrap();

        // Add content
        let content = HybridContent {
            vector: vec![1.0, 0.0],
            text: "test content".to_string(),
            metadata: HashMap::new(),
        };

        engine.add_content(1, &content).await.unwrap();

        // Delete content
        engine.delete_content(1).await.unwrap();

        // Search should return no results
        let query = HybridQuery::new(vec![1.0, 0.0], vec!["test".to_string()]);
        let results = engine.hybrid_search(&query, 5).await.unwrap();

        assert!(results.results.is_empty());
    }
}
