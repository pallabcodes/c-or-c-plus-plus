//! DDL Query Parser
//!
//! Parses Data Definition Language queries:
//! - CREATE TABLE statements
//! - DROP TABLE statements
//! - ALTER TABLE statements (future)

use crate::query::parser::ast::*;

/// DDL query parser
pub struct DdlParser;

impl DdlParser {
    /// Parse DDL query from tokens
    pub fn parse(tokens: &[Token]) -> ParseResult<Query> {
        match tokens.get(1) {
            Some(Token::Keyword(keyword)) => match keyword.as_str() {
                "TABLE" => self.parse_table_statement(tokens),
                _ => Err(ParseError::SyntaxError {
                    position: 1,
                    message: format!("Unsupported DDL object: {}", keyword),
                }),
            },
            _ => Err(ParseError::SyntaxError {
                position: 0,
                message: "Expected object type after DDL keyword".to_string(),
            }),
        }
    }

    /// Parse table-related DDL statements
    fn parse_table_statement(&self, tokens: &[Token]) -> ParseResult<Query> {
        match tokens.first() {
            Some(Token::Keyword(keyword)) => match keyword.as_str() {
                "CREATE" => Ok(Query::CreateTable(self.parse_create_table(tokens)?)),
                "DROP" => Ok(Query::DropTable(self.parse_drop_table(tokens)?)),
                _ => Err(ParseError::SyntaxError {
                    position: 0,
                    message: format!("Unsupported table DDL: {}", keyword),
                }),
            },
            _ => Err(ParseError::SyntaxError {
                position: 0,
                message: "Expected DDL keyword".to_string(),
            }),
        }
    }

    /// Parse CREATE TABLE statement
    fn parse_create_table(&self, _tokens: &[Token]) -> ParseResult<CreateTableQuery> {
        // TODO: Implement full CREATE TABLE parsing
        Ok(CreateTableQuery {
            name: "test_table".to_string(),
            columns: vec![],
            constraints: vec![],
        })
    }

    /// Parse DROP TABLE statement
    fn parse_drop_table(&self, _tokens: &[Token]) -> ParseResult<DropTableQuery> {
        // TODO: Implement full DROP TABLE parsing
        Ok(DropTableQuery {
            name: "test_table".to_string(),
            if_exists: false,
        })
    }

    /// Parse column definitions
    fn parse_column_definitions(&self, _tokens: &[Token], _position: &mut usize) -> ParseResult<Vec<ColumnDefinition>> {
        // TODO: Implement column definition parsing
        Ok(vec![])
    }

    /// Parse table constraints
    fn parse_table_constraints(&self, _tokens: &[Token], _position: &mut usize) -> ParseResult<Vec<TableConstraint>> {
        // TODO: Implement table constraint parsing
        Ok(vec![])
    }
}
