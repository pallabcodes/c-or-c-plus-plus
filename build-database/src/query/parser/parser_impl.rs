//! Parser Implementation
//!
//! Main parser implementation that delegates to specific query parsers.

use super::*;
use super::parsers::*;

/// Main SQL parser with AI-powered query understanding
pub struct SqlParser {
    /// Query position for error reporting
    position: usize,
    /// AI-powered query hints and optimizations
    query_hints: HashMap<String, String>,
    /// Parser statistics for performance monitoring
    stats: ParserStats,
}

/// Parser performance statistics
#[derive(Debug, Clone, Default)]
pub struct ParserStats {
    pub total_queries_parsed: u64,
    pub parse_errors: u64,
    pub average_parse_time_ms: f64,
    pub hints_applied: u64,
}

impl SqlParser {
    /// Create a new SQL parser
    pub fn new() -> Self {
        Self {
            position: 0,
            query_hints: HashMap::new(),
            stats: ParserStats::default(),
        }
    }

    /// Parse a SQL query string into a Query AST
    pub fn parse(&mut self, sql: &str) -> ParseResult<Query> {
        let start_time = std::time::Instant::now();

        // Tokenize the input
        let mut tokenizer = Tokenizer::new();
        let tokens = tokenizer.tokenize(sql)?;

        // Parse based on the first token
        let query = self.parse_query(&tokens)?;

        // Update statistics
        let parse_time = start_time.elapsed().as_millis() as f64;
        self.stats.total_queries_parsed += 1;
        self.stats.average_parse_time_ms =
            (self.stats.average_parse_time_ms * (self.stats.total_queries_parsed - 1) as f64 + parse_time)
                / self.stats.total_queries_parsed as f64;

        Ok(query)
    }

    /// Parse query based on first token
    fn parse_query(&mut self, tokens: &[Token]) -> ParseResult<Query> {
        match tokens.first() {
            Some(Token::Keyword(keyword)) => match keyword.as_str() {
                "SELECT" => Ok(Query::Select(SelectParser::parse(tokens)?)),
                "INSERT" | "UPDATE" | "DELETE" => Ok(DmlParser::parse(tokens)?),
                "CREATE" | "DROP" => Ok(DdlParser::parse(tokens)?),
                "NEAREST" | "VECTOR_SEARCH" => Ok(Query::VectorSearch(VectorParser::parse(tokens)?)),
                _ => Err(ParseError::SyntaxError {
                    position: self.position,
                    message: format!("Unsupported query type: {}", keyword),
                }),
            },
            _ => Err(ParseError::SyntaxError {
                position: self.position,
                message: "Expected keyword at start of query".to_string(),
            }),
        }
    }

    /// Get parser statistics
    pub fn stats(&self) -> &ParserStats {
        &self.stats
    }

    /// Apply AI-powered query hints
    pub fn apply_hints(&mut self, hints: HashMap<String, String>) {
        self.query_hints = hints;
        self.stats.hints_applied += 1;
    }
}