//! DML Query Parser
//!
//! Parses Data Manipulation Language queries:
//! - INSERT statements
//! - UPDATE statements
//! - DELETE statements

use crate::query::parser::ast::*;

/// DML query parser
pub struct DmlParser;

impl DmlParser {
    /// Parse DML query from tokens
    pub fn parse(tokens: &[Token]) -> ParseResult<Query> {
        match tokens.first() {
            Some(Token::Keyword(keyword)) => match keyword.as_str() {
                "INSERT" => Ok(Query::Insert(Self::parse_insert(tokens)?)),
                "UPDATE" => Ok(Query::Update(Self::parse_update(tokens)?)),
                "DELETE" => Ok(Query::Delete(Self::parse_delete(tokens)?)),
                _ => Err(ParseError::SyntaxError {
                    position: 0,
                    message: format!("Not a DML query: {}", keyword),
                }),
            },
            _ => Err(ParseError::SyntaxError {
                position: 0,
                message: "Expected DML keyword".to_string(),
            }),
        }
    }

    /// Parse INSERT query
    fn parse_insert(tokens: &[Token]) -> ParseResult<InsertQuery> {
        let mut position = 0;

        // Expect INSERT INTO
        self.expect_keyword(tokens, &mut position, "INSERT")?;
        self.expect_keyword(tokens, &mut position, "INTO")?;

        // Parse table name
        let table_name = self.parse_identifier(tokens, &mut position)?;

        // Optional column list
        let columns = if matches!(tokens.get(position), Some(Token::LeftParen)) {
            position += 1; // skip '('
            let cols = self.parse_identifier_list(tokens, &mut position)?;
            self.expect_token(tokens, &mut position, Token::RightParen)?;
            cols
        } else {
            vec![] // No columns specified, will infer from values
        };

        // Expect VALUES
        self.expect_keyword(tokens, &mut position, "VALUES")?;

        // Parse value lists
        let values = self.parse_value_lists(tokens, &mut position)?;

        Ok(InsertQuery {
            table: table_name,
            columns,
            values,
        })
    }

    /// Parse UPDATE query
    fn parse_update(_tokens: &[Token]) -> ParseResult<UpdateQuery> {
        // TODO: Implement full UPDATE parsing
        Ok(UpdateQuery {
            table: "test_table".to_string(),
            assignments: vec![],
            where_clause: None,
        })
    }

    /// Parse UPDATE query
    fn parse_update(tokens: &[Token]) -> ParseResult<UpdateQuery> {
        let mut position = 0;

        // Expect UPDATE
        self.expect_keyword(tokens, &mut position, "UPDATE")?;

        // Parse table name
        let table_name = self.parse_identifier(tokens, &mut position)?;

        // Expect SET
        self.expect_keyword(tokens, &mut position, "SET")?;

        // Parse assignments
        let assignments = self.parse_assignments(tokens, &mut position)?;

        // Optional WHERE clause
        let where_clause = if matches!(tokens.get(position), Some(Token::Keyword(kw)) if kw == "WHERE") {
            position += 1; // skip WHERE
            Some(self.parse_expression(tokens, &mut position)?)
        } else {
            None
        };

        Ok(UpdateQuery {
            table: table_name,
            assignments,
            where_clause,
        })
    }

    /// Parse DELETE query
    fn parse_delete(tokens: &[Token]) -> ParseResult<DeleteQuery> {
        let mut position = 0;

        // Expect DELETE FROM
        self.expect_keyword(tokens, &mut position, "DELETE")?;
        self.expect_keyword(tokens, &mut position, "FROM")?;

        // Parse table name
        let table_name = self.parse_identifier(tokens, &mut position)?;

        // Optional WHERE clause
        let where_clause = if matches!(tokens.get(position), Some(Token::Keyword(kw)) if kw == "WHERE") {
            position += 1; // skip WHERE
            Some(self.parse_expression(tokens, &mut position)?)
        } else {
            None
        };

        Ok(DeleteQuery {
            table: table_name,
            where_clause,
        })
    }

    /// Parse identifier
    fn parse_identifier(&self, tokens: &[Token], position: &mut usize) -> ParseResult<String> {
        match tokens.get(*position) {
            Some(Token::Identifier(name)) => {
                *position += 1;
                Ok(name.clone())
            }
            _ => Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected identifier".to_string(),
            }),
        }
    }

    /// Parse list of identifiers
    fn parse_identifier_list(&self, tokens: &[Token], position: &mut usize) -> ParseResult<Vec<String>> {
        let mut identifiers = Vec::new();

        loop {
            identifiers.push(self.parse_identifier(tokens, position)?);

            match tokens.get(*position) {
                Some(Token::Comma) => {
                    *position += 1;
                }
                _ => break,
            }
        }

        Ok(identifiers)
    }

    /// Parse value lists for INSERT
    fn parse_value_lists(&self, tokens: &[Token], position: &mut usize) -> ParseResult<Vec<Vec<Expression>>> {
        let mut value_lists = Vec::new();

        loop {
            // Expect opening parenthesis
            self.expect_token(tokens, *position, Token::LeftParen)?;
            *position += 1;

            // Parse expressions in this value list
            let mut values = Vec::new();
            loop {
                values.push(self.parse_expression(tokens, position)?);

                match tokens.get(*position) {
                    Some(Token::Comma) => {
                        *position += 1;
                    }
                    Some(Token::RightParen) => {
                        *position += 1;
                        break;
                    }
                    _ => return Err(ParseError::SyntaxError {
                        position: *position,
                        message: "Expected comma or closing parenthesis in value list".to_string(),
                    }),
                }
            }

            value_lists.push(values);

            // Check for more value lists
            match tokens.get(*position) {
                Some(Token::Comma) => {
                    *position += 1;
                }
                _ => break,
            }
        }

        Ok(value_lists)
    }

    /// Parse assignments for UPDATE
    fn parse_assignments(&self, tokens: &[Token], position: &mut usize) -> ParseResult<Vec<Assignment>> {
        let mut assignments = Vec::new();

        loop {
            // Parse column = value
            let column = self.parse_identifier(tokens, position)?;
            self.expect_token(tokens, *position, Token::Equals)?;
            *position += 1;
            let value = self.parse_expression(tokens, position)?;

            assignments.push(Assignment { column, value });

            // Check for more assignments
            match tokens.get(*position) {
                Some(Token::Comma) => {
                    *position += 1;
                }
                _ => break,
            }
        }

        Ok(assignments)
    }

    /// Parse expression (simplified for literals)
    fn parse_expression(&self, tokens: &[Token], position: &mut usize) -> ParseResult<Expression> {
        match tokens.get(*position) {
            Some(Token::StringLiteral(s)) => {
                *position += 1;
                Ok(Expression::Literal(Literal::String(s.clone())))
            }
            Some(Token::NumberLiteral(n)) => {
                *position += 1;
                // Try to parse as integer first, then float
                if let Ok(int_val) = n.parse::<i64>() {
                    Ok(Expression::Literal(Literal::Integer(int_val)))
                } else if let Ok(float_val) = n.parse::<f64>() {
                    Ok(Expression::Literal(Literal::Float(float_val)))
                } else {
                    Err(ParseError::SyntaxError {
                        position: *position,
                        message: format!("Invalid number: {}", n),
                    })
                }
            }
            Some(Token::Keyword(kw)) if kw == "NULL" => {
                *position += 1;
                Ok(Expression::Literal(Literal::Null))
            }
            Some(Token::Keyword(kw)) if kw == "TRUE" => {
                *position += 1;
                Ok(Expression::Literal(Literal::Boolean(true)))
            }
            Some(Token::Keyword(kw)) if kw == "FALSE" => {
                *position += 1;
                Ok(Expression::Literal(Literal::Boolean(false)))
            }
            _ => Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected literal value".to_string(),
            }),
        }
    }

    /// Helper: Expect specific keyword
    fn expect_keyword(&self, tokens: &[Token], position: &mut usize, keyword: &str) -> ParseResult<()> {
        match tokens.get(*position) {
            Some(Token::Keyword(kw)) if kw == keyword => {
                *position += 1;
                Ok(())
            }
            _ => Err(ParseError::SyntaxError {
                position: *position,
                message: format!("Expected keyword '{}'", keyword),
            }),
        }
    }

    /// Helper: Expect specific token
    fn expect_token(&self, tokens: &[Token], position: usize, expected: Token) -> ParseResult<()> {
        match tokens.get(position) {
            Some(token) if token == &expected => Ok(()),
            _ => Err(ParseError::SyntaxError {
                position,
                message: format!("Expected {:?}", expected),
            }),
        }
    }

    /// Parse column list for INSERT/UPDATE
    fn parse_column_list(&self, _tokens: &[Token], _position: &mut usize) -> ParseResult<Vec<String>> {
        // TODO: Implement column list parsing
        Ok(vec![])
    }

    /// Parse VALUES clause for INSERT
    fn parse_values_clause(&self, _tokens: &[Token], _position: &mut usize) -> ParseResult<Vec<Vec<Expression>>> {
        // TODO: Implement VALUES parsing
        Ok(vec![])
    }

    /// Parse SET clause for UPDATE
    fn parse_set_clause(&self, _tokens: &[Token], _position: &mut usize) -> ParseResult<Vec<Assignment>> {
        // TODO: Implement SET clause parsing
        Ok(vec![])
    }
}
