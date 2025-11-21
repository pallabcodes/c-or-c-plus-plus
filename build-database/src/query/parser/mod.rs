//! SQL Parser Module
//!
//! Modular parser implementation with separate components for:
//! - AST definitions (abstract syntax tree)
//! - Tokenizer (lexical analysis)
//! - Parser implementation (syntax analysis)
//!
//! UNIQUENESS: Combines Pratt parser + recursive descent + error recovery
//! for superior SQL parsing with AI-powered query understanding

pub mod ast;
pub mod tokenizer;
pub mod parser_impl;

// Re-export main parser components
pub use ast::*;
pub use tokenizer::*;
pub use parser_impl::*;
