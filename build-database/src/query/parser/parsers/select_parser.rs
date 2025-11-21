//! SELECT Query Parser
//!
//! Parses SELECT statements with support for:
//! - Standard SQL SELECT syntax
//! - Vector search extensions
//! - Analytics functions

use crate::query::parser::ast::*;

/// SELECT query parser
pub struct SelectParser;

impl SelectParser {
    /// Parse SELECT query from tokens
    pub fn parse(tokens: &[Token]) -> ParseResult<SelectQuery> {
        let mut position = 0;

        // Expect SELECT keyword
        Self::expect_keyword(tokens, &mut position, "SELECT")?;

        // Parse SELECT list
        let select_list = Self::parse_select_list(tokens, &mut position)?;

        // Parse FROM clause
        let from_clause = Self::parse_from_clause(tokens, &mut position)?;

        // Parse optional WHERE clause
        let where_clause = Self::parse_where_clause(tokens, &mut position)?;

        // Parse optional GROUP BY clause
        let group_by = Self::parse_group_by_clause(tokens, &mut position)?;

        // Parse optional HAVING clause
        let having = Self::parse_having_clause(tokens, &mut position)?;

        // Parse optional ORDER BY clause
        let order_by = Self::parse_order_by_clause(tokens, &mut position)?;

        // Parse optional LIMIT clause
        let limit = Self::parse_limit_clause(tokens, &mut position)?;

        Ok(SelectQuery {
            select_list,
            from_clause,
            where_clause,
            group_by,
            having,
            order_by,
            limit,
            vector_extensions: None,
        })
    }

    /// Parse SELECT list (column expressions)
    fn parse_select_list(tokens: &[Token], position: &mut usize) -> ParseResult<Vec<SelectItem>> {
        let mut select_list = Vec::new();

        loop {
            // Check for wildcard
            if Self::match_token(tokens, position, Token::Operator("*".to_string())) {
                select_list.push(SelectItem::Wildcard);
                *position += 1;
            } else if let Some(Token::Identifier(column)) = tokens.get(*position) {
                // Parse column name
                let column_name = column.clone();
                *position += 1;

                // Check for optional alias
                let expression = Expression::Column(column_name.clone());
                let alias = if Self::match_keyword(tokens, position, "AS") {
                    if let Some(Token::Identifier(alias_name)) = tokens.get(*position) {
                        let alias_str = alias_name.clone();
                        *position += 1;
                        Some(alias_str)
                    } else {
                        None
                    }
                } else {
                    None
                };

                if let Some(alias_name) = alias {
                    select_list.push(SelectItem::Aliased { expression, alias: alias_name });
                } else {
                    select_list.push(SelectItem::Expression(expression));
                }
            } else {
                return Err(ParseError::SyntaxError {
                    position: *position,
                    message: "Expected column name or wildcard in SELECT list".to_string(),
                });
            }

            // Check for comma (more columns) or break
            if !Self::match_token(tokens, position, Token::Comma) {
                break;
            }
        }

        Ok(select_list)
    }

    /// Parse FROM clause with joins
    fn parse_from_clause(tokens: &[Token], position: &mut usize) -> ParseResult<FromClause> {
        // Expect FROM keyword
        Self::expect_keyword(tokens, position, "FROM")?;

        // Parse table name
        let table_name = if let Some(Token::Identifier(table)) = tokens.get(*position) {
            let name = table.clone();
            *position += 1;
            name
        } else {
            return Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected table name after FROM".to_string(),
            });
        };

        // Optional table alias
        let alias = if let Some(Token::Identifier(alias)) = tokens.get(*position) {
            let alias_name = alias.clone();
            *position += 1;
            Some(alias_name)
        } else {
            None
        };

        // Parse JOIN clauses
        let joins = Self::parse_join_clauses(tokens, position)?;

        Ok(FromClause {
            table: table_name,
            alias,
            joins,
        })
    }

    /// Parse JOIN clauses
    fn parse_join_clauses(tokens: &[Token], position: &mut usize) -> ParseResult<Vec<crate::query::parser::ast::JoinClause>> {
        let mut joins = Vec::new();

        loop {
            // Check if next token is a JOIN keyword
            let join_type = if Self::match_keyword(tokens, position, "INNER") {
                Some(crate::query::parser::ast::JoinType::Inner)
            } else if Self::match_keyword(tokens, position, "LEFT") {
                if Self::match_keyword(tokens, position, "OUTER") {
                    Some(crate::query::parser::ast::JoinType::Left)
                } else {
                    Some(crate::query::parser::ast::JoinType::Left)
                }
            } else if Self::match_keyword(tokens, position, "RIGHT") {
                if Self::match_keyword(tokens, position, "OUTER") {
                    Some(crate::query::parser::ast::JoinType::Right)
                } else {
                    Some(crate::query::parser::ast::JoinType::Right)
                }
            } else if Self::match_keyword(tokens, position, "FULL") {
                if Self::match_keyword(tokens, position, "OUTER") {
                    Some(crate::query::parser::ast::JoinType::Full)
                } else {
                    Some(crate::query::parser::ast::JoinType::Full)
                }
            } else if Self::match_keyword(tokens, position, "JOIN") {
                Some(crate::query::parser::ast::JoinType::Inner) // Default to INNER JOIN
            } else {
                break; // No more joins
            };

            if let Some(join_type) = join_type {
                // Parse JOIN table
                Self::expect_keyword(tokens, position, "JOIN")?;

                // Parse table name
                let table_name = if let Some(Token::Identifier(table)) = tokens.get(*position) {
                    let name = table.clone();
                    *position += 1;
                    name
                } else {
                    return Err(ParseError::SyntaxError {
                        position: *position,
                        message: "Expected table name after JOIN".to_string(),
                    });
                };

                // Optional table alias
                let alias = if let Some(Token::Identifier(alias)) = tokens.get(*position) {
                    let alias_name = alias.clone();
                    *position += 1;
                    Some(alias_name)
                } else {
                    None
                };

                // Parse ON condition
                Self::expect_keyword(tokens, position, "ON")?;
                let condition = Self::parse_expression(tokens, position)?;

                joins.push(crate::query::parser::ast::JoinClause {
                    join_type,
                    table: table_name,
                    alias,
                    condition,
                });
            }
        }

        Ok(joins)
    }

    /// Parse WHERE clause
    fn parse_where_clause(tokens: &[Token], position: &mut usize) -> ParseResult<Option<Expression>> {
        if !Self::match_keyword(tokens, position, "WHERE") {
            return Ok(None);
        }

        // Parse expression (simplified for now)
        let expression = Self::parse_expression(tokens, position)?;

        Ok(Some(expression))
    }

    /// Parse GROUP BY clause
    fn parse_group_by_clause(tokens: &[Token], position: &mut usize) -> ParseResult<Option<GroupByClause>> {
        if !Self::match_keyword(tokens, position, "GROUP") {
            return Ok(None);
        }

        Self::expect_keyword(tokens, position, "BY")?;

        let mut expressions = Vec::new();

        loop {
            if let Some(Token::Identifier(column)) = tokens.get(*position) {
                expressions.push(Expression::Column(column.clone()));
                *position += 1;
            } else {
                return Err(ParseError::SyntaxError {
                    position: *position,
                    message: "Expected column name in GROUP BY".to_string(),
                });
            }

            if !Self::match_token(tokens, position, Token::Comma) {
                break;
            }
        }

        Ok(Some(GroupByClause {
            expressions,
        }))
    }

    /// Parse HAVING clause
    fn parse_having_clause(tokens: &[Token], position: &mut usize) -> ParseResult<Option<Expression>> {
        if !Self::match_keyword(tokens, position, "HAVING") {
            return Ok(None);
        }

        let expression = Self::parse_expression(tokens, position)?;

        Ok(Some(expression))
    }

    /// Parse ORDER BY clause
    fn parse_order_by_clause(tokens: &[Token], position: &mut usize) -> ParseResult<Option<OrderByClause>> {
        if !Self::match_keyword(tokens, position, "ORDER") {
            return Ok(None);
        }

        Self::expect_keyword(tokens, position, "BY")?;

        let mut items = Vec::new();

        loop {
            if let Some(Token::Identifier(column)) = tokens.get(*position) {
                let column_name = column.clone();
                *position += 1;

                // Check for ASC/DESC
                let direction = if Self::match_keyword(tokens, position, "DESC") {
                    SortDirection::Descending
                } else {
                    // Default to ASC, consume ASC if present
                    Self::match_keyword(tokens, position, "ASC");
                    SortDirection::Ascending
                };

                items.push(OrderByItem {
                    expression: Expression::Column(column_name),
                    direction,
                });
            } else {
                return Err(ParseError::SyntaxError {
                    position: *position,
                    message: "Expected column name in ORDER BY".to_string(),
                });
            }

            if !Self::match_token(tokens, position, Token::Comma) {
                break;
            }
        }

        Ok(Some(OrderByClause {
            items,
        }))
    }

    /// Parse LIMIT clause
    fn parse_limit_clause(tokens: &[Token], position: &mut usize) -> ParseResult<Option<LimitClause>> {
        if !Self::match_keyword(tokens, position, "LIMIT") {
            return Ok(None);
        }

        if let Some(Token::Number(limit_str)) = tokens.get(*position) {
            let limit = limit_str.parse().map_err(|_| ParseError::SyntaxError {
                position: *position,
                message: "Invalid LIMIT value".to_string(),
            })?;
            *position += 1;

            let offset = if Self::match_keyword(tokens, position, "OFFSET") {
                if let Some(Token::Number(offset_str)) = tokens.get(*position) {
                    let offset_val = offset_str.parse().map_err(|_| ParseError::SyntaxError {
                        position: *position,
                        message: "Invalid OFFSET value".to_string(),
                    })?;
                    *position += 1;
                    Some(offset_val)
                } else {
                    return Err(ParseError::SyntaxError {
                        position: *position,
                        message: "Expected OFFSET value".to_string(),
                    });
                }
            } else {
                None
            };

            Ok(Some(LimitClause {
                limit,
                offset,
            }))
        } else {
            Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected LIMIT value".to_string(),
            })
        }
    }

    /// Parse expression (simplified)
    fn parse_expression(tokens: &[Token], position: &mut usize) -> ParseResult<Expression> {
        // Check for function calls first
        if let Some(Token::Identifier(func_name)) = tokens.get(*position) {
            if let Some(Token::LParen) = tokens.get(*position + 1) {
                // This is a function call
                return Self::parse_function_call(tokens, position);
            }
        }

        // Very basic expression parsing for now
        if let Some(Token::Identifier(column)) = tokens.get(*position) {
            let column_name = column.clone();
            *position += 1;

            // Check for operator and value
            if let Some(Token::Operator(op)) = tokens.get(*position) {
                if op == "=" || op == ">" || op == "<" || op == ">=" || op == "<=" || op == "!=" {
                    let operator = op.clone();
                    *position += 1;

                    if let Some(Token::String(value)) = tokens.get(*position) {
                        let string_value = value.clone();
                        *position += 1;
                        return Ok(Expression::BinaryOp(BinaryOp {
                            left: Box::new(Expression::Column(column_name)),
                            operator: Self::string_to_binary_operator(&operator)?,
                            right: Box::new(Expression::Literal(Literal::String(string_value))),
                        }));
                    } else if let Some(Token::Number(value)) = tokens.get(*position) {
                        let num_value = value.clone();
                        *position += 1;
                        // Try to parse as integer first, then float
                        if let Ok(int_val) = num_value.parse::<i64>() {
                            return Ok(Expression::BinaryOp(BinaryOp {
                                left: Box::new(Expression::Column(column_name)),
                                operator: Self::string_to_binary_operator(&operator)?,
                                right: Box::new(Expression::Literal(Literal::Integer(int_val))),
                            }));
                        } else if let Ok(float_val) = num_value.parse::<f64>() {
                            return Ok(Expression::BinaryOp(BinaryOp {
                                left: Box::new(Expression::Column(column_name)),
                                operator: Self::string_to_binary_operator(&operator)?,
                                right: Box::new(Expression::Literal(Literal::Float(float_val))),
                            }));
                        }
                    }
                }
            }

            Ok(Expression::Column(column_name))
        } else {
            Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected expression".to_string(),
            })
        }
    }

    /// Parse function call (regular or window function)
    fn parse_function_call(tokens: &[Token], position: &mut usize) -> ParseResult<Expression> {
        // Get function name
        let func_name = if let Some(Token::Identifier(name)) = tokens.get(*position) {
            let name_clone = name.clone();
            *position += 1;
            name_clone
        } else {
            return Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected function name".to_string(),
            });
        };

        // Expect opening parenthesis
        Self::expect_token(tokens, position, Token::LParen)?;

        // Parse arguments
        let mut arguments = Vec::new();

        // Special handling for COUNT(*) - allow asterisk
        if func_name.to_uppercase() == "COUNT" && let Some(Token::Asterisk) = tokens.get(*position) {
            *position += 1;
            arguments.push(Expression::Asterisk);
        } else {
            // Parse regular arguments
            loop {
                if let Some(Token::RParen) = tokens.get(*position) {
                    break;
                }

                let arg = Self::parse_expression(tokens, position)?;
                arguments.push(arg);

                // Check for comma or closing paren
                if let Some(Token::Comma) = tokens.get(*position) {
                    *position += 1;
                } else if let Some(Token::RParen) = tokens.get(*position) {
                    break;
                } else {
                    return Err(ParseError::SyntaxError {
                        position: *position,
                        message: "Expected comma or closing parenthesis".to_string(),
                    });
                }
            }
        }

        // Expect closing parenthesis
        Self::expect_token(tokens, position, Token::RParen)?;

        // Check if this is a window function (followed by OVER)
        if Self::match_keyword(tokens, position, "OVER") {
            // This is a window function - parse the OVER clause
            let window_function = Self::parse_window_function(
                tokens,
                position,
                FunctionCall {
                    name: func_name,
                    arguments,
                }
            )?;
            Ok(Expression::WindowFunction(window_function))
        } else {
            // Regular function call
            Ok(Expression::Function(FunctionCall {
                name: func_name,
                arguments,
            }))
        }
    }

    /// Parse window function OVER clause
    fn parse_window_function(
        tokens: &[Token],
        position: &mut usize,
        function: FunctionCall,
    ) -> ParseResult<crate::query::parser::ast::WindowFunction> {
        // Expect opening parenthesis after OVER
        Self::expect_token(tokens, position, Token::LParen)?;

        let mut partition_by = Vec::new();
        let mut order_by = Vec::new();
        let mut frame_clause = None;

        // Parse window specification
        loop {
            if let Some(Token::RParen) = tokens.get(*position) {
                *position += 1;
                break;
            }

            if Self::match_keyword(tokens, position, "PARTITION") {
                Self::expect_keyword(tokens, position, "BY")?;
                // Parse partition columns
                loop {
                    let expr = Self::parse_expression(tokens, position)?;
                    partition_by.push(expr);

                    if Self::match_keyword(tokens, position, "ORDER") ||
                       Self::match_keyword(tokens, position, "ROWS") ||
                       Self::match_keyword(tokens, position, "RANGE") ||
                       let Some(Token::RParen) = tokens.get(*position) {
                        break;
                    }

                    if let Some(Token::Comma) = tokens.get(*position) {
                        *position += 1;
                    } else {
                        break;
                    }
                }
            } else if Self::match_keyword(tokens, position, "ORDER") {
                Self::expect_keyword(tokens, position, "BY")?;
                order_by = Self::parse_order_by_clause(tokens, position)?;
            } else if Self::match_keyword(tokens, position, "ROWS") ||
                      Self::match_keyword(tokens, position, "RANGE") {
                frame_clause = Some(Self::parse_frame_clause(tokens, position)?);
            } else {
                return Err(ParseError::SyntaxError {
                    position: *position,
                    message: "Expected PARTITION BY, ORDER BY, ROWS, or RANGE in window specification".to_string(),
                });
            }
        }

        Ok(crate::query::parser::ast::WindowFunction {
            function,
            partition_by,
            order_by,
            frame_clause,
        })
    }

    /// Parse frame clause (ROWS/RANGE)
    fn parse_frame_clause(tokens: &[Token], position: &mut usize) -> ParseResult<crate::query::parser::ast::FrameClause> {
        let frame_type = if Self::match_keyword(tokens, position, "ROWS") {
            crate::query::parser::ast::FrameType::Rows
        } else if Self::match_keyword(tokens, position, "RANGE") {
            crate::query::parser::ast::FrameType::Range
        } else {
            return Err(ParseError::SyntaxError {
                position: *position,
                message: "Expected ROWS or RANGE".to_string(),
            });
        };

        let start_bound = Self::parse_frame_bound(tokens, position)?;

        // Check for BETWEEN ... AND ...
        let end_bound = if Self::match_keyword(tokens, position, "AND") {
            Some(Self::parse_frame_bound(tokens, position)?)
        } else {
            None
        };

        Ok(crate::query::parser::ast::FrameClause {
            frame_type,
            start_bound,
            end_bound,
        })
    }

    /// Parse frame bound
    fn parse_frame_bound(tokens: &[Token], position: &mut usize) -> ParseResult<crate::query::parser::ast::FrameBound> {
        if Self::match_keyword(tokens, position, "UNBOUNDED") {
            if Self::match_keyword(tokens, position, "PRECEDING") {
                Ok(crate::query::parser::ast::FrameBound::UnboundedPreceding)
            } else if Self::match_keyword(tokens, position, "FOLLOWING") {
                Ok(crate::query::parser::ast::FrameBound::UnboundedFollowing)
            } else {
                return Err(ParseError::SyntaxError {
                    position: *position,
                    message: "Expected PRECEDING or FOLLOWING after UNBOUNDED".to_string(),
                });
            }
        } else if Self::match_keyword(tokens, position, "CURRENT") {
            Self::expect_keyword(tokens, position, "ROW")?;
            Ok(crate::query::parser::ast::FrameBound::CurrentRow)
        } else {
            // Parse number for PRECEDING/FOLLOWING
            if let Some(Token::Number(num_str)) = tokens.get(*position) {
                let num = num_str.parse::<u64>().map_err(|_| ParseError::SyntaxError {
                    position: *position,
                    message: "Invalid number in frame bound".to_string(),
                })?;
                *position += 1;

                if Self::match_keyword(tokens, position, "PRECEDING") {
                    Ok(crate::query::parser::ast::FrameBound::Preceding(num))
                } else if Self::match_keyword(tokens, position, "FOLLOWING") {
                    Ok(crate::query::parser::ast::FrameBound::Following(num))
                } else {
                    return Err(ParseError::SyntaxError {
                        position: *position,
                        message: "Expected PRECEDING or FOLLOWING after number".to_string(),
                    });
                }
            } else {
                return Err(ParseError::SyntaxError {
                    position: *position,
                    message: "Expected UNBOUNDED, CURRENT ROW, or number".to_string(),
                });
            }
        }
    }

    /// Helper: Expect specific keyword
    fn expect_keyword(tokens: &[Token], position: &mut usize, keyword: &str) -> ParseResult<()> {
        if let Some(Token::Keyword(kw)) = tokens.get(*position) {
            if kw == keyword {
                *position += 1;
                Ok(())
            } else {
                Err(ParseError::SyntaxError {
                    position: *position,
                    message: format!("Expected keyword '{}', found '{}'", keyword, kw),
                })
            }
        } else {
            Err(ParseError::SyntaxError {
                position: *position,
                message: format!("Expected keyword '{}'", keyword),
            })
        }
    }

    /// Helper: Match keyword without consuming
    fn match_keyword(tokens: &[Token], position: &mut usize, keyword: &str) -> bool {
        if let Some(Token::Keyword(kw)) = tokens.get(*position) {
            if kw == keyword {
                *position += 1;
                return true;
            }
        }
        false
    }

    /// Helper: Expect specific token (consumes token, returns error if not found)
    fn expect_token(tokens: &[Token], position: &mut usize, expected: Token) -> ParseResult<()> {
        if let Some(token) = tokens.get(*position) {
            if token == &expected {
                *position += 1;
                return Ok(());
            }
        }
        Err(ParseError::SyntaxError {
            position: *position,
            message: format!("Expected token {:?}, found {:?}", expected, tokens.get(*position)),
        })
    }

    /// Helper: Match token
    fn match_token(tokens: &[Token], position: &mut usize, expected: Token) -> bool {
        if let Some(token) = tokens.get(*position) {
            if token == &expected {
                *position += 1;
                return true;
            }
        }
        false
    }

    /// Helper: Convert string operator to BinaryOperator
    fn string_to_binary_operator(op: &str) -> ParseResult<BinaryOperator> {
        match op {
            "=" => Ok(BinaryOperator::Equal),
            "!=" => Ok(BinaryOperator::NotEqual),
            ">" => Ok(BinaryOperator::GreaterThan),
            "<" => Ok(BinaryOperator::LessThan),
            ">=" => Ok(BinaryOperator::GreaterEqual),
            "<=" => Ok(BinaryOperator::LessEqual),
            _ => Err(ParseError::SyntaxError {
                position: 0,
                message: format!("Unsupported operator: {}", op),
            }),
        }
    }

    /// Parse vector search extensions
    fn parse_vector_extensions(&self, _tokens: &[Token], _position: &mut usize) -> ParseResult<Option<VectorExtensions>> {
        // TODO: Implement vector extension parsing
        Ok(None)
    }
}
