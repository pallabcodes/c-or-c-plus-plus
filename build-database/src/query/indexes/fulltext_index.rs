//! Full-Text Index: Advanced Text Search and Ranking

use std::collections::HashMap;
use crate::core::errors::AuroraResult;

#[derive(Debug, Clone)]
pub struct FullTextIndexConfig {
    pub name: String,
    pub column: String,
    pub language: String,
    pub enable_stemming: bool,
    pub enable_stopwords: bool,
}

#[derive(Debug)]
pub struct FullTextIndex {
    config: FullTextIndexConfig,
    inverted_index: HashMap<String, Vec<(u64, f64)>>, // term -> [(doc_id, score)]
    document_count: u64,
}

impl FullTextIndex {
    pub fn new(config: FullTextIndexConfig) -> AuroraResult<Self> {
        Ok(Self {
            config,
            inverted_index: HashMap::new(),
            document_count: 0,
        })
    }

    pub fn insert(&mut self, doc_id: u64, text: &str) -> AuroraResult<()> {
        // Simplified full-text indexing
        let terms = self.tokenize(text);
        for term in terms {
            let score = self.calculate_tf_idf(term, doc_id, text);
            self.inverted_index.entry(term)
                .or_insert_with(Vec::new)
                .push((doc_id, score));
        }
        self.document_count += 1;
        Ok(())
    }

    pub fn search(&self, query: &str) -> AuroraResult<Vec<(u64, f64)>> {
        let query_terms = self.tokenize(query);
        let mut results = HashMap::new();

        for term in query_terms {
            if let Some(docs) = self.inverted_index.get(&term) {
                for (doc_id, score) in docs {
                    *results.entry(*doc_id).or_insert(0.0) += score;
                }
            }
        }

        let mut sorted_results: Vec<_> = results.into_iter().collect();
        sorted_results.sort_by(|a, b| b.1.partial_cmp(&a.1).unwrap());

        Ok(sorted_results)
    }

    fn tokenize(&self, text: &str) -> Vec<String> {
        text.to_lowercase()
            .split_whitespace()
            .map(|s| s.trim_matches(|c: char| !c.is_alphanumeric()))
            .filter(|s| !s.is_empty())
            .map(|s| s.to_string())
            .collect()
    }

    fn calculate_tf_idf(&self, term: String, doc_id: u64, text: &str) -> f64 {
        // Simplified TF-IDF calculation
        let term_count = text.to_lowercase().matches(&term).count() as f64;
        let doc_length = text.split_whitespace().count() as f64;
        let tf = term_count / doc_length;

        let df = self.inverted_index.get(&term).map(|docs| docs.len()).unwrap_or(1) as f64;
        let idf = (self.document_count as f64 / df).ln();

        tf * idf
    }
}
