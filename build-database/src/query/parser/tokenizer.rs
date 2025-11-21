//! Lexical Analysis (Tokenizer)
//!
//! Converts SQL input strings into tokens for parsing.
//! Includes support for vector search keywords and AI extensions.

use super::ast::*;

/// Token types for lexical analysis
#[derive(Debug, Clone, PartialEq)]
pub enum Token {
    Keyword(String),
    Identifier(String),
    Integer(i64),
    Float(f64),
    String(String),
    Equals,
    NotEquals,
    LessThan,
    GreaterThan,
    LessEqual,
    GreaterEqual,
    Plus,
    Minus,
    Asterisk,
    Slash,
    LeftParen,
    RightParen,
    Comma,
    Dot,
    Semicolon,
    Bang,
}

/// SQL Tokenizer with AI-powered query hints
pub struct Tokenizer {
    input: Vec<char>,
    position: usize,
    keywords: std::collections::HashSet<String>,
}

impl Tokenizer {
    /// Create a new tokenizer
    pub fn new() -> Self {
        let mut keywords = std::collections::HashSet::new();
        // SQL keywords
        for kw in &[
            "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES", "UPDATE", "SET",
            "DELETE", "CREATE", "TABLE", "DROP", "IF", "EXISTS", "PRIMARY", "KEY",
            "FOREIGN", "REFERENCES", "UNIQUE", "NULL", "NOT", "AND", "OR", "ORDER",
            "BY", "GROUP", "HAVING", "LIMIT", "OFFSET", "JOIN", "INNER", "LEFT",
            "RIGHT", "FULL", "ON", "AS", "ASC", "DESC"
        ] {
            keywords.insert(kw.to_string());
        }
        // Vector search keywords
        for kw in &["NEAREST", "SIMILARITY", "VECTOR", "SEARCH", "WITH", "DISTANCE"] {
            keywords.insert(kw.to_string());
        }

        Self {
            input: Vec::new(),
            position: 0,
            keywords,
        }
    }

    /// Tokenize input SQL string
    pub fn tokenize(&mut self, sql: &str) -> ParseResult<Vec<Token>> {
        self.input = sql.chars().collect();
        self.position = 0;
        let mut tokens = Vec::new();

        while self.position < self.input.len() {
            match self.peek() {
                Some(' ') | Some('\t') | Some('\n') | Some('\r') => {
                    self.advance();
                }
                Some('A'..='Z') | Some('a'..='z') | Some('_') => {
                    tokens.push(self.read_identifier_or_keyword()?);
                }
                Some('0'..='9') => {
                    tokens.push(self.read_number()?);
                }
                Some('"') | Some('\'') => {
                    tokens.push(self.read_string()?);
                }
                Some('=') => {
                    self.advance();
                    tokens.push(Token::Equals);
                }
                Some('!') => {
                    self.advance();
                    if self.peek() == Some('=') {
                        self.advance();
                        tokens.push(Token::NotEquals);
                    } else {
                        tokens.push(Token::Bang);
                    }
                }
                Some('<') => {
                    self.advance();
                    if self.peek() == Some('=') {
                        self.advance();
                        tokens.push(Token::LessEqual);
                    } else {
                        tokens.push(Token::LessThan);
                    }
                }
                Some('>') => {
                    self.advance();
                    if self.peek() == Some('=') {
                        self.advance();
                        tokens.push(Token::GreaterEqual);
                    } else {
                        tokens.push(Token::GreaterThan);
                    }
                }
                Some('+') => {
                    self.advance();
                    tokens.push(Token::Plus);
                }
                Some('-') => {
                    self.advance();
                    tokens.push(Token::Minus);
                }
                Some('*') => {
                    self.advance();
                    tokens.push(Token::Asterisk);
                }
                Some('/') => {
                    self.advance();
                    tokens.push(Token::Slash);
                }
                Some('(') => {
                    self.advance();
                    tokens.push(Token::LeftParen);
                }
                Some(')') => {
                    self.advance();
                    tokens.push(Token::RightParen);
                }
                Some(',') => {
                    self.advance();
                    tokens.push(Token::Comma);
                }
                Some('.') => {
                    self.advance();
                    tokens.push(Token::Dot);
                }
                Some(';') => {
                    self.advance();
                    tokens.push(Token::Semicolon);
                }
                Some(ch) => {
                    return Err(ParseError::SyntaxError {
                        position: self.position,
                        message: format!("Unexpected character: {}", ch),
                    });
                }
                None => break,
            }
        }

        Ok(tokens)
    }

    /// Read identifier or keyword
    fn read_identifier_or_keyword(&mut self) -> ParseResult<Token> {
        let start = self.position;
        while self.position < self.input.len() &&
              (self.input[self.position].is_alphanumeric() || self.input[self.position] == '_') {
            self.position += 1;
        }

        let text: String = self.input[start..self.position].iter().collect();
        let upper = text.to_uppercase();

        if self.keywords.contains(&upper) {
            Ok(Token::Keyword(upper))
        } else {
            Ok(Token::Identifier(text))
        }
    }

    /// Read numeric literal
    fn read_number(&mut self) -> ParseResult<Token> {
        let start = self.position;
        let mut has_dot = false;

        while self.position < self.input.len() {
            match self.input[self.position] {
                '0'..='9' => self.position += 1,
                '.' if !has_dot => {
                    has_dot = true;
                    self.position += 1;
                }
                _ => break,
            }
        }

        let text: String = self.input[start..self.position].iter().collect();

        if has_dot {
            match text.parse::<f64>() {
                Ok(value) => Ok(Token::Float(value)),
                Err(_) => Err(ParseError::SyntaxError {
                    position: start,
                    message: "Invalid float literal".to_string(),
                }),
            }
        } else {
            match text.parse::<i64>() {
                Ok(value) => Ok(Token::Integer(value)),
                Err(_) => Err(ParseError::SyntaxError {
                    position: start,
                    message: "Invalid integer literal".to_string(),
                }),
            }
        }
    }

    /// Read string literal
    fn read_string(&mut self) -> ParseResult<Token> {
        let quote = self.advance().unwrap();
        let start = self.position;
        let mut string = String::new();

        while let Some(ch) = self.peek() {
            if ch == quote {
                self.advance();
                return Ok(Token::String(string));
            } else if ch == '\\' {
                self.advance();
                if let Some(escaped) = self.peek() {
                    match escaped {
                        'n' => string.push('\n'),
                        't' => string.push('\t'),
                        'r' => string.push('\r'),
                        '\\' => string.push('\\'),
                        '"' => string.push('"'),
                        '\'' => string.push('\''),
                        _ => string.push(escaped),
                    }
                    self.advance();
                }
            } else {
                string.push(ch);
                self.advance();
            }
        }

        Err(ParseError::SyntaxError {
            position: start,
            message: "Unterminated string literal".to_string(),
        })
    }

    /// Peek at current character
    fn peek(&self) -> Option<char> {
        self.input.get(self.position).copied()
    }

    /// Advance to next character and return current
    fn advance(&mut self) -> Option<char> {
        if self.position < self.input.len() {
            let ch = self.input[self.position];
            self.position += 1;
            Some(ch)
        } else {
            None
        }
    }

    /// Get current position
    pub fn position(&self) -> usize {
        self.position
    }
}
