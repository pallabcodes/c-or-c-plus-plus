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
    fn parse_create_table(&self, tokens: &[Token]) -> ParseResult<CreateTableQuery> {
        let mut position = 0;

        // Expect CREATE TABLE
        self.expect_keyword(tokens, &mut position, "CREATE")?;
        self.expect_keyword(tokens, &mut position, "TABLE")?;

        // Parse table name
        let table_name = self.parse_table_name(tokens, &mut position)?;

        // Expect opening parenthesis
        self.expect_token(tokens, &mut position, Token::LeftParen)?;

        // Parse column definitions and constraints
        let (columns, constraints) = self.parse_columns_and_constraints(tokens, &mut position)?;

        // Expect closing parenthesis
        self.expect_token(tokens, &mut position, Token::RightParen)?;

        Ok(CreateTableQuery {
            name: table_name,
            columns,
            constraints,
        })
    }

    /// Parse DROP TABLE statement
    fn parse_drop_table(&self, tokens: &[Token]) -> ParseResult<DropTableQuery> {
        let mut position = 0;

        // Expect DROP TABLE
        self.expect_keyword(tokens, &mut position, "DROP")?;
        self.expect_keyword(tokens, &mut position, "TABLE")?;

        // Check for IF EXISTS
        let if_exists = if matches!(tokens.get(position), Some(Token::Keyword(kw)) if kw == "IF") {
            self.expect_keyword(tokens, &mut position, "IF")?;
            self.expect_keyword(tokens, &mut position, "EXISTS")?;
            true
        } else {
            false
        };

        // Parse table name
        let table_name = self.parse_table_name(tokens, &mut position)?;

        Ok(DropTableQuery {
            name: table_name,
            if_exists,
        })
    }

    /// Parse table name
    fn parse_table_name(&self, tokens: &[Token], position: &mut usize) -> ParseResult<String> {
        match tokens.get(*position) {
            Some(Token::Identifier(name)) => {
                *position += 1;
                Ok(name.clone())
            }
            _ => Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected table name".to_string(),
            }),
        }
    }

    /// Parse columns and constraints
    fn parse_columns_and_constraints(&self, tokens: &[Token], position: &mut usize) -> ParseResult<(Vec<ColumnDefinition>, Vec<TableConstraint>)> {
        let mut columns = Vec::new();
        let mut constraints = Vec::new();

        loop {
            // Check if it's a column definition or constraint
            match tokens.get(*position) {
                Some(Token::Identifier(_)) => {
                    // Try to parse as column definition first
                    match self.try_parse_column_definition(tokens, *position) {
                        Ok(column) => {
                            columns.push(column);
                            // Skip the tokens we consumed
                            self.skip_column_definition(tokens, position)?;
                            continue;
                        }
                        Err(_) => {
                            // Try to parse as constraint
                            match self.try_parse_table_constraint(tokens, *position) {
                                Ok(constraint) => {
                                    constraints.push(constraint);
                                    // Skip the tokens we consumed
                                    self.skip_table_constraint(tokens, position)?;
                                    continue;
                                }
                                Err(_) => {
                                    return Err(ParseError::SyntaxError {
                                        position: *position,
                                        message: "Expected column definition or table constraint".to_string(),
                                    });
                                }
                            }
                        }
                    }
                }
                Some(Token::Keyword(kw)) if kw == "PRIMARY" || kw == "UNIQUE" || kw == "FOREIGN" => {
                    // Parse table constraint
                    let constraint = self.parse_table_constraint(tokens, position)?;
                    constraints.push(constraint);
                }
                _ => break,
            }

            // Check for comma or end
            match tokens.get(*position) {
                Some(Token::Comma) => {
                    *position += 1;
                }
                Some(Token::RightParen) => break,
                _ => {
                    return Err(ParseError::SyntaxError {
                        position: *position,
                        message: "Expected comma or closing parenthesis".to_string(),
                    });
                }
            }
        }

        Ok((columns, constraints))
    }

    /// Try to parse a column definition
    fn try_parse_column_definition(&self, tokens: &[Token], start_pos: usize) -> ParseResult<ColumnDefinition> {
        let mut position = start_pos;

        // Column name
        let column_name = match tokens.get(position) {
            Some(Token::Identifier(name)) => {
                position += 1;
                name.clone()
            }
            _ => return Err(ParseError::SyntaxError {
                position,
                message: "Expected column name".to_string(),
            }),
        };

        // Column type
        let data_type = self.parse_data_type(tokens, &mut position)?;

        // Column constraints
        let mut nullable = true; // Default to nullable
        let mut default = None;

        // Parse optional constraints
        while let Some(token) = tokens.get(position) {
            match token {
                Token::Keyword(kw) if kw == "NOT" => {
                    // Check for NOT NULL
                    if matches!(tokens.get(position + 1), Some(Token::Keyword(kw)) if kw == "NULL") {
                        nullable = false;
                        position += 2;
                    } else {
                        break;
                    }
                }
                Token::Keyword(kw) if kw == "NULL" => {
                    nullable = true;
                    position += 1;
                }
                Token::Keyword(kw) if kw == "DEFAULT" => {
                    position += 1;
                    // Parse default value (simplified)
                    default = Some(Expression::Literal(Literal::String("default".to_string())));
                }
                Token::Keyword(kw) if kw == "PRIMARY" => {
                    // This might be PRIMARY KEY constraint
                    break;
                }
                _ => break,
            }
        }

        Ok(ColumnDefinition {
            name: column_name,
            data_type,
            nullable,
            default,
        })
    }

    /// Parse data type
    fn parse_data_type(&self, tokens: &[Token], position: &mut usize) -> ParseResult<crate::data::DataType> {
        match tokens.get(*position) {
            Some(Token::Keyword(kw)) => {
                let data_type = match kw.as_str() {
                    "INTEGER" | "INT" => crate::data::DataType::Integer,
                    "BIGINT" => crate::data::DataType::BigInt,
                    "FLOAT" | "REAL" => crate::data::DataType::Float,
                    "DOUBLE" => crate::data::DataType::Double,
                    "TEXT" | "VARCHAR" => crate::data::DataType::Text,
                    "BOOLEAN" | "BOOL" => crate::data::DataType::Boolean,
                    "BLOB" => crate::data::DataType::Blob,
                    _ => return Err(ParseError::SyntaxError {
                        position: *position,
                        message: format!("Unknown data type: {}", kw),
                    }),
                };
                *position += 1;
                Ok(data_type)
            }
            _ => Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected data type".to_string(),
            }),
        }
    }

    /// Try to parse table constraint
    fn try_parse_table_constraint(&self, tokens: &[Token], start_pos: usize) -> ParseResult<TableConstraint> {
        let mut position = start_pos;

        match tokens.get(position) {
            Some(Token::Keyword(kw)) if kw == "PRIMARY" => {
                // PRIMARY KEY constraint
                self.expect_keyword(tokens, &mut position, "PRIMARY")?;
                self.expect_keyword(tokens, &mut position, "KEY")?;
                self.expect_token(tokens, &mut position, Token::LeftParen)?;
                let columns = self.parse_column_list(tokens, &mut position)?;
                self.expect_token(tokens, &mut position, Token::RightParen)?;
                Ok(TableConstraint::PrimaryKey(columns))
            }
            Some(Token::Keyword(kw)) if kw == "UNIQUE" => {
                // UNIQUE constraint
                self.expect_keyword(tokens, &mut position, "UNIQUE")?;
                self.expect_token(tokens, &mut position, Token::LeftParen)?;
                let columns = self.parse_column_list(tokens, &mut position)?;
                self.expect_token(tokens, &mut position, Token::RightParen)?;
                Ok(TableConstraint::Unique(columns))
            }
            _ => Err(ParseError::SyntaxError {
                position,
                message: "Not a table constraint".to_string(),
            }),
        }
    }

    /// Parse column list for constraints
    fn parse_column_list(&self, tokens: &[Token], position: &mut usize) -> ParseResult<Vec<String>> {
        let mut columns = Vec::new();

        loop {
            match tokens.get(*position) {
                Some(Token::Identifier(name)) => {
                    columns.push(name.clone());
                    *position += 1;
                }
                _ => break,
            }

            match tokens.get(*position) {
                Some(Token::Comma) => {
                    *position += 1;
                }
                _ => break,
            }
        }

        Ok(columns)
    }

    /// Skip column definition tokens (for parsing logic)
    fn skip_column_definition(&self, tokens: &[Token], position: &mut usize) -> ParseResult<()> {
        // Skip until comma or closing paren
        while *position < tokens.len() {
            match tokens[*position] {
                Token::Comma | Token::RightParen => break,
                _ => *position += 1,
            }
        }
        Ok(())
    }

    /// Skip table constraint tokens
    fn skip_table_constraint(&self, tokens: &[Token], position: &mut usize) -> ParseResult<()> {
        // Skip until comma or closing paren
        while *position < tokens.len() {
            match tokens[*position] {
                Token::Comma | Token::RightParen => break,
                _ => *position += 1,
            }
        }
        Ok(())
    }

    /// Parse table constraint
    fn parse_table_constraint(&self, tokens: &[Token], position: &mut usize) -> ParseResult<TableConstraint> {
        self.try_parse_table_constraint(tokens, *position)
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
    fn expect_token(&self, tokens: &[Token], position: &mut usize, expected: Token) -> ParseResult<()> {
        match tokens.get(*position) {
            Some(token) if token == &expected => {
                *position += 1;
                Ok(())
            }
            _ => Err(ParseError::SyntaxError {
                position: *position,
                message: format!("Expected {:?}", expected),
            }),
        }
    }
}
