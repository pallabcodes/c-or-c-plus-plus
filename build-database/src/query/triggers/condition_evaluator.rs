//! Condition Evaluator: Intelligent Trigger Condition Processing
//!
//! Advanced condition evaluation system for triggers with complex logic,
//! performance optimization, and intelligent filtering.

use std::collections::HashMap;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use super::event_engine::DatabaseEvent;

/// Trigger condition definition
#[derive(Debug, Clone)]
pub struct TriggerCondition {
    pub condition_type: String,
    pub parameters: HashMap<String, String>,
    pub operator: ConditionOperator,
    pub negate: bool,
}

/// Condition operators
#[derive(Debug, Clone, PartialEq)]
pub enum ConditionOperator {
    Equals,
    NotEquals,
    GreaterThan,
    LessThan,
    GreaterThanOrEqual,
    LessThanOrEqual,
    Contains,
    NotContains,
    Regex,
    In,
    NotIn,
    Between,
    IsNull,
    IsNotNull,
}

/// Complex condition with logical operators
#[derive(Debug, Clone)]
pub struct ComplexCondition {
    pub conditions: Vec<TriggerCondition>,
    pub logical_operator: LogicalOperator,
}

/// Logical operators for combining conditions
#[derive(Debug, Clone, PartialEq)]
pub enum LogicalOperator {
    And,
    Or,
}

/// Condition evaluation result
#[derive(Debug)]
pub struct ConditionResult {
    pub condition_met: bool,
    pub evaluation_time_ms: f64,
    pub evaluated_conditions: usize,
}

/// Condition evaluation statistics
#[derive(Debug)]
pub struct ConditionStats {
    pub total_evaluations: u64,
    pub cache_hits: u64,
    pub cache_misses: u64,
    pub avg_evaluation_time_ms: f64,
    pub most_frequent_conditions: HashMap<String, u64>,
}

/// Intelligent condition evaluator
pub struct ConditionEvaluator {
    evaluation_cache: std::sync::Mutex<HashMap<String, (ConditionResult, DateTime<Utc>)>>,
    stats: std::sync::Mutex<ConditionStats>,
    condition_parsers: HashMap<String, Box<dyn ConditionParser>>,
}

impl ConditionEvaluator {
    pub fn new() -> Self {
        let mut condition_parsers = HashMap::new();

        // Register built-in condition parsers
        condition_parsers.insert("row_count".to_string(), Box::new(RowCountParser));
        condition_parsers.insert("value_comparison".to_string(), Box::new(ValueComparisonParser));
        condition_parsers.insert("user_check".to_string(), Box::new(UserCheckParser));
        condition_parsers.insert("time_window".to_string(), Box::new(TimeWindowParser));
        condition_parsers.insert("regex_match".to_string(), Box::new(RegexMatchParser));
        condition_parsers.insert("json_path".to_string(), Box::new(JsonPathParser));

        Self {
            evaluation_cache: std::sync::Mutex::new(HashMap::new()),
            stats: std::sync::Mutex::new(ConditionStats {
                total_evaluations: 0,
                cache_hits: 0,
                cache_misses: 0,
                avg_evaluation_time_ms: 0.0,
                most_frequent_conditions: HashMap::new(),
            }),
            condition_parsers,
        }
    }

    /// Evaluate a single trigger condition
    pub async fn evaluate_condition(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let start_time = std::time::Instant::now();

        // Check cache first
        let cache_key = self.generate_cache_key(condition, event);
        if let Some((cached_result, cache_time)) = self.evaluation_cache.lock().unwrap().get(&cache_key) {
            // Cache for 5 minutes
            if Utc::now().signed_duration_since(cache_time).num_minutes() < 5 {
                let mut stats = self.stats.lock().unwrap();
                stats.cache_hits += 1;
                return Ok(cached_result.condition_met);
            }
        }

        // Update stats
        {
            let mut stats = self.stats.lock().unwrap();
            stats.total_evaluations += 1;
            stats.cache_misses += 1;
            *stats.most_frequent_conditions.entry(condition.condition_type.clone()).or_insert(0) += 1;
        }

        // Evaluate condition
        let result = self.evaluate_condition_internal(condition, event).await?;

        // Cache result
        let evaluation_time = start_time.elapsed().as_millis() as f64;
        let condition_result = ConditionResult {
            condition_met: result,
            evaluation_time_ms: evaluation_time,
            evaluated_conditions: 1,
        };

        {
            let mut cache = self.evaluation_cache.lock().unwrap();
            cache.insert(cache_key, (condition_result, Utc::now()));

            // Limit cache size
            if cache.len() > 1000 {
                // Remove oldest entries (simplified)
                let keys_to_remove: Vec<String> = cache.keys().take(100).cloned().collect();
                for key in keys_to_remove {
                    cache.remove(&key);
                }
            }
        }

        Ok(result)
    }

    /// Evaluate multiple conditions with logical operators
    pub async fn evaluate_conditions(&self, conditions: &[TriggerCondition], event: &DatabaseEvent) -> AuroraResult<bool> {
        if conditions.is_empty() {
            return Ok(true);
        }

        let start_time = std::time::Instant::now();

        // Evaluate all conditions
        let mut results = Vec::new();
        for condition in conditions {
            let result = self.evaluate_condition(condition, event).await?;
            results.push(result);
        }

        // Combine with AND logic (all conditions must be true)
        let final_result = results.iter().all(|&r| r);

        // Update stats
        let evaluation_time = start_time.elapsed().as_millis() as f64;
        {
            let mut stats = self.stats.lock().unwrap();
            stats.avg_evaluation_time_ms = (stats.avg_evaluation_time_ms * (stats.total_evaluations - 1) as f64 + evaluation_time) / stats.total_evaluations as f64;
        }

        Ok(final_result)
    }

    /// Evaluate complex condition with logical operators
    pub async fn evaluate_complex_condition(&self, complex_condition: &ComplexCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let mut results = Vec::new();

        for condition in &complex_condition.conditions {
            let result = self.evaluate_condition(condition, event).await?;
            results.push(result);
        }

        match complex_condition.logical_operator {
            LogicalOperator::And => Ok(results.iter().all(|&r| r)),
            LogicalOperator::Or => Ok(results.iter().any(|&r| r)),
        }
    }

    /// Pre-compile conditions for better performance
    pub async fn precompile_conditions(&self, conditions: &[TriggerCondition]) -> AuroraResult<Vec<CompiledCondition>> {
        let mut compiled = Vec::new();

        for condition in conditions {
            if let Some(parser) = self.condition_parsers.get(&condition.condition_type) {
                let compiled_condition = parser.precompile(condition).await?;
                compiled.push(compiled_condition);
            } else {
                return Err(AuroraError::InvalidArgument(format!("Unknown condition type: {}", condition.condition_type)));
            }
        }

        Ok(compiled)
    }

    /// Evaluate pre-compiled conditions
    pub async fn evaluate_compiled_conditions(&self, compiled_conditions: &[CompiledCondition], event: &DatabaseEvent) -> AuroraResult<bool> {
        for compiled in compiled_conditions {
            let result = (compiled.evaluator)(event)?;
            if !result {
                return Ok(false);
            }
        }
        Ok(true)
    }

    /// Get condition evaluation statistics
    pub fn get_condition_stats(&self) -> ConditionStats {
        self.stats.lock().unwrap().clone()
    }

    /// Clear evaluation cache
    pub fn clear_cache(&self) {
        let mut cache = self.evaluation_cache.lock().unwrap();
        cache.clear();
    }

    /// Add custom condition parser
    pub fn add_condition_parser(&mut self, condition_type: String, parser: Box<dyn ConditionParser>) {
        self.condition_parsers.insert(condition_type, parser);
    }

    // Private methods

    async fn evaluate_condition_internal(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let result = if let Some(parser) = self.condition_parsers.get(&condition.condition_type) {
            parser.evaluate(condition, event).await?
        } else {
            return Err(AuroraError::InvalidArgument(format!("Unknown condition type: {}", condition.condition_type)));
        };

        // Apply negation if specified
        Ok(if condition.negate { !result } else { result })
    }

    fn generate_cache_key(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> String {
        format!("{}_{}_{}_{:?}_{}",
                condition.condition_type,
                event.table_name,
                event.operation,
                condition.operator,
                condition.parameters.len())
    }
}

/// Pre-compiled condition for performance
#[derive(Debug)]
pub struct CompiledCondition {
    pub condition_type: String,
    pub evaluator: Box<dyn Fn(&DatabaseEvent) -> AuroraResult<bool> + Send + Sync>,
}

/// Condition parser trait
#[async_trait::async_trait]
pub trait ConditionParser: Send + Sync {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool>;
    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition>;
}

/// Row count condition parser
pub struct RowCountParser;

#[async_trait::async_trait]
impl ConditionParser for RowCountParser {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let threshold_str = condition.parameters.get("threshold")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing threshold parameter".to_string()))?;

        let threshold: u64 = threshold_str.parse()
            .map_err(|_| AuroraError::InvalidArgument("Invalid threshold value".to_string()))?;

        let result = match condition.operator {
            ConditionOperator::GreaterThan => event.affected_rows > threshold,
            ConditionOperator::LessThan => event.affected_rows < threshold,
            ConditionOperator::GreaterThanOrEqual => event.affected_rows >= threshold,
            ConditionOperator::LessThanOrEqual => event.affected_rows <= threshold,
            ConditionOperator::Equals => event.affected_rows == threshold,
            ConditionOperator::NotEquals => event.affected_rows != threshold,
            _ => return Err(AuroraError::InvalidArgument("Unsupported operator for row count".to_string())),
        };

        Ok(result)
    }

    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition> {
        let threshold_str = condition.parameters.get("threshold")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing threshold parameter".to_string()))?
            .clone();

        let threshold: u64 = threshold_str.parse()
            .map_err(|_| AuroraError::InvalidArgument("Invalid threshold value".to_string()))?;

        let operator = condition.operator.clone();

        let evaluator = move |event: &DatabaseEvent| {
            let result = match operator {
                ConditionOperator::GreaterThan => event.affected_rows > threshold,
                ConditionOperator::LessThan => event.affected_rows < threshold,
                ConditionOperator::GreaterThanOrEqual => event.affected_rows >= threshold,
                ConditionOperator::LessThanOrEqual => event.affected_rows <= threshold,
                ConditionOperator::Equals => event.affected_rows == threshold,
                ConditionOperator::NotEquals => event.affected_rows != threshold,
                _ => return Err(AuroraError::InvalidArgument("Unsupported operator for row count".to_string())),
            };

            Ok(if condition.negate { !result } else { result })
        };

        Ok(CompiledCondition {
            condition_type: "row_count".to_string(),
            evaluator: Box::new(evaluator),
        })
    }
}

/// Value comparison condition parser
pub struct ValueComparisonParser;

#[async_trait::async_trait]
impl ConditionParser for ValueComparisonParser {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let field = condition.parameters.get("field")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing field parameter".to_string()))?;

        let expected_value = condition.parameters.get("value")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing value parameter".to_string()))?;

        let actual_value = if let Some(new_values) = &event.new_values {
            new_values.get(field)
        } else if let Some(old_values) = &event.old_values {
            old_values.get(field)
        } else {
            None
        };

        let result = match condition.operator {
            ConditionOperator::Equals => actual_value == Some(expected_value),
            ConditionOperator::NotEquals => actual_value != Some(expected_value),
            ConditionOperator::Contains => actual_value.map_or(false, |v| v.contains(expected_value)),
            ConditionOperator::NotContains => actual_value.map_or(true, |v| !v.contains(expected_value)),
            _ => return Err(AuroraError::InvalidArgument("Unsupported operator for value comparison".to_string())),
        };

        Ok(result)
    }

    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition> {
        let field = condition.parameters.get("field")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing field parameter".to_string()))?
            .clone();

        let expected_value = condition.parameters.get("value")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing value parameter".to_string()))?
            .clone();

        let operator = condition.operator.clone();

        let evaluator = move |event: &DatabaseEvent| {
            let actual_value = if let Some(new_values) = &event.new_values {
                new_values.get(&field)
            } else if let Some(old_values) = &event.old_values {
                old_values.get(&field)
            } else {
                None
            };

            let result = match operator {
                ConditionOperator::Equals => actual_value == Some(&expected_value),
                ConditionOperator::NotEquals => actual_value != Some(&expected_value),
                ConditionOperator::Contains => actual_value.map_or(false, |v| v.contains(&expected_value)),
                ConditionOperator::NotContains => actual_value.map_or(true, |v| !v.contains(&expected_value)),
                _ => return Err(AuroraError::InvalidArgument("Unsupported operator for value comparison".to_string())),
            };

            Ok(if condition.negate { !result } else { result })
        };

        Ok(CompiledCondition {
            condition_type: "value_comparison".to_string(),
            evaluator: Box::new(evaluator),
        })
    }
}

/// User check condition parser
pub struct UserCheckParser;

#[async_trait::async_trait]
impl ConditionParser for UserCheckParser {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let required_role = condition.parameters.get("role")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing role parameter".to_string()))?;

        if let Some(user_id) = &event.user_id {
            let result = match condition.operator {
                ConditionOperator::Equals => self.check_user_role(user_id, required_role),
                ConditionOperator::NotEquals => !self.check_user_role(user_id, required_role),
                _ => return Err(AuroraError::InvalidArgument("Unsupported operator for user check".to_string())),
            };
            Ok(result)
        } else {
            Ok(false) // No user context
        }
    }

    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition> {
        let required_role = condition.parameters.get("role")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing role parameter".to_string()))?
            .clone();

        let operator = condition.operator.clone();

        let evaluator = move |event: &DatabaseEvent| {
            if let Some(user_id) = &event.user_id {
                let result = match operator {
                    ConditionOperator::Equals => self.check_user_role(user_id, &required_role),
                    ConditionOperator::NotEquals => !self.check_user_role(user_id, &required_role),
                    _ => return Err(AuroraError::InvalidArgument("Unsupported operator for user check".to_string())),
                };
                Ok(if condition.negate { !result } else { result })
            } else {
                Ok(condition.negate) // Negate false = true if no user context and negate is true
            }
        };

        Ok(CompiledCondition {
            condition_type: "user_check".to_string(),
            evaluator: Box::new(evaluator),
        })
    }
}

impl UserCheckParser {
    fn check_user_role(&self, user_id: &str, required_role: &str) -> bool {
        // Simplified role checking - in real implementation would check against user database
        match required_role {
            "admin" => user_id.starts_with("admin_") || user_id.contains("_admin"),
            "manager" => user_id.contains("manager") || user_id.starts_with("mgr_"),
            "user" => !user_id.starts_with("admin_"),
            _ => false,
        }
    }
}

/// Time window condition parser
pub struct TimeWindowParser;

#[async_trait::async_trait]
impl ConditionParser for TimeWindowParser {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let start_time_str = condition.parameters.get("start_time")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing start_time parameter".to_string()))?;

        let end_time_str = condition.parameters.get("end_time")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing end_time parameter".to_string()))?;

        let start_time = DateTime::parse_from_rfc3339(start_time_str)
            .map_err(|_| AuroraError::InvalidArgument("Invalid start_time format".to_string()))?
            .with_timezone(&Utc);

        let end_time = DateTime::parse_from_rfc3339(end_time_str)
            .map_err(|_| AuroraError::InvalidArgument("Invalid end_time format".to_string()))?
            .with_timezone(&Utc);

        let event_time = event.timestamp;

        let result = match condition.operator {
            ConditionOperator::Between => event_time >= start_time && event_time <= end_time,
            _ => return Err(AuroraError::InvalidArgument("Unsupported operator for time window".to_string())),
        };

        Ok(result)
    }

    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition> {
        let start_time_str = condition.parameters.get("start_time")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing start_time parameter".to_string()))?
            .clone();

        let end_time_str = condition.parameters.get("end_time")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing end_time parameter".to_string()))?
            .clone();

        let start_time = DateTime::parse_from_rfc3339(&start_time_str)
            .map_err(|_| AuroraError::InvalidArgument("Invalid start_time format".to_string()))?
            .with_timezone(&Utc);

        let end_time = DateTime::parse_from_rfc3339(&end_time_str)
            .map_err(|_| AuroraError::InvalidArgument("Invalid end_time format".to_string()))?
            .with_timezone(&Utc);

        let operator = condition.operator.clone();

        let evaluator = move |event: &DatabaseEvent| {
            let event_time = event.timestamp;

            let result = match operator {
                ConditionOperator::Between => event_time >= start_time && event_time <= end_time,
                _ => return Err(AuroraError::InvalidArgument("Unsupported operator for time window".to_string())),
            };

            Ok(if condition.negate { !result } else { result })
        };

        Ok(CompiledCondition {
            condition_type: "time_window".to_string(),
            evaluator: Box::new(evaluator),
        })
    }
}

/// Regex match condition parser
pub struct RegexMatchParser;

#[async_trait::async_trait]
impl ConditionParser for RegexMatchParser {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        let pattern = condition.parameters.get("pattern")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing pattern parameter".to_string()))?;

        let field = condition.parameters.get("field")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing field parameter".to_string()))?;

        let regex = regex::Regex::new(pattern)
            .map_err(|_| AuroraError::InvalidArgument("Invalid regex pattern".to_string()))?;

        let value_to_check = if let Some(new_values) = &event.new_values {
            new_values.get(field)
        } else if let Some(old_values) = &event.old_values {
            old_values.get(field)
        } else {
            None
        };

        let result = if let Some(value) = value_to_check {
            regex.is_match(value)
        } else {
            false
        };

        Ok(result)
    }

    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition> {
        let pattern = condition.parameters.get("pattern")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing pattern parameter".to_string()))?
            .clone();

        let field = condition.parameters.get("field")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing field parameter".to_string()))?
            .clone();

        let regex = regex::Regex::new(&pattern)
            .map_err(|_| AuroraError::InvalidArgument("Invalid regex pattern".to_string()))?;

        let evaluator = move |event: &DatabaseEvent| {
            let value_to_check = if let Some(new_values) = &event.new_values {
                new_values.get(&field)
            } else if let Some(old_values) = &event.old_values {
                old_values.get(&field)
            } else {
                None
            };

            let result = if let Some(value) = value_to_check {
                regex.is_match(value)
            } else {
                false
            };

            Ok(if condition.negate { !result } else { result })
        };

        Ok(CompiledCondition {
            condition_type: "regex_match".to_string(),
            evaluator: Box::new(evaluator),
        })
    }
}

/// JSON path condition parser
pub struct JsonPathParser;

#[async_trait::async_trait]
impl ConditionParser for JsonPathParser {
    async fn evaluate(&self, condition: &TriggerCondition, event: &DatabaseEvent) -> AuroraResult<bool> {
        // Simplified JSON path evaluation - in real implementation would use a proper JSON path library
        let path = condition.parameters.get("path")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing path parameter".to_string()))?;

        let expected_value = condition.parameters.get("value");

        // For demonstration, just check if the path exists in new_values
        let exists = if let Some(new_values) = &event.new_values {
            new_values.contains_key(path)
        } else {
            false
        };

        let result = if let Some(expected) = expected_value {
            exists && event.new_values.as_ref()
                .and_then(|vals| vals.get(path))
                .map_or(false, |v| v == expected)
        } else {
            exists
        };

        Ok(result)
    }

    async fn precompile(&self, condition: &TriggerCondition) -> AuroraResult<CompiledCondition> {
        let path = condition.parameters.get("path")
            .ok_or_else(|| AuroraError::InvalidArgument("Missing path parameter".to_string()))?
            .clone();

        let expected_value = condition.parameters.get("value").cloned();

        let evaluator = move |event: &DatabaseEvent| {
            let exists = if let Some(new_values) = &event.new_values {
                new_values.contains_key(&path)
            } else {
                false
            };

            let result = if let Some(expected) = &expected_value {
                exists && event.new_values.as_ref()
                    .and_then(|vals| vals.get(&path))
                    .map_or(false, |v| v == expected)
            } else {
                exists
            };

            Ok(if condition.negate { !result } else { result })
        };

        Ok(CompiledCondition {
            condition_type: "json_path".to_string(),
            evaluator: Box::new(evaluator),
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_test_event() -> DatabaseEvent {
        super::super::event_engine::DatabaseEvent {
            event_type: super::super::event_engine::EventType::AfterInsert,
            table_name: "users".to_string(),
            operation: "INSERT".to_string(),
            timestamp: Utc::now(),
            transaction_id: Some("tx_123".to_string()),
            user_id: Some("admin_user_456".to_string()),
            session_id: Some("session_789".to_string()),
            old_values: None,
            new_values: Some(HashMap::from([
                ("id".to_string(), "1".to_string()),
                ("name".to_string(), "John Doe".to_string()),
                ("email".to_string(), "john@example.com".to_string()),
                ("status".to_string(), "active".to_string()),
            ])),
            affected_rows: 1,
            query_text: Some("INSERT INTO users (name, email) VALUES ('John Doe', 'john@example.com')".to_string()),
            client_info: None,
        }
    }

    #[tokio::test]
    async fn test_condition_evaluator_creation() {
        let evaluator = ConditionEvaluator::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_condition_operators() {
        assert_eq!(ConditionOperator::Equals, ConditionOperator::Equals);
        assert_ne!(ConditionOperator::GreaterThan, ConditionOperator::LessThan);
    }

    #[test]
    fn test_logical_operators() {
        assert_eq!(LogicalOperator::And, LogicalOperator::And);
        assert_ne!(LogicalOperator::And, LogicalOperator::Or);
    }

    #[tokio::test]
    async fn test_row_count_condition() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let condition = TriggerCondition {
            condition_type: "row_count".to_string(),
            parameters: HashMap::from([
                ("threshold".to_string(), "0".to_string()),
            ]),
            operator: ConditionOperator::GreaterThan,
            negate: false,
        };

        let result = evaluator.evaluate_condition(&condition, &event).await.unwrap();
        assert!(result); // 1 > 0
    }

    #[tokio::test]
    async fn test_value_comparison_condition() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let condition = TriggerCondition {
            condition_type: "value_comparison".to_string(),
            parameters: HashMap::from([
                ("field".to_string(), "status".to_string()),
                ("value".to_string(), "active".to_string()),
            ]),
            operator: ConditionOperator::Equals,
            negate: false,
        };

        let result = evaluator.evaluate_condition(&condition, &event).await.unwrap();
        assert!(result);
    }

    #[tokio::test]
    async fn test_user_check_condition() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let condition = TriggerCondition {
            condition_type: "user_check".to_string(),
            parameters: HashMap::from([
                ("role".to_string(), "admin".to_string()),
            ]),
            operator: ConditionOperator::Equals,
            negate: false,
        };

        let result = evaluator.evaluate_condition(&condition, &event).await.unwrap();
        assert!(result); // admin_user_456 contains admin
    }

    #[tokio::test]
    async fn test_negated_condition() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let condition = TriggerCondition {
            condition_type: "row_count".to_string(),
            parameters: HashMap::from([
                ("threshold".to_string(), "10".to_string()),
            ]),
            operator: ConditionOperator::GreaterThan,
            negate: true, // Negate the result
        };

        let result = evaluator.evaluate_condition(&condition, &event).await.unwrap();
        assert!(result); // !(1 > 10) = true
    }

    #[tokio::test]
    async fn test_multiple_conditions() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let conditions = vec![
            TriggerCondition {
                condition_type: "row_count".to_string(),
                parameters: HashMap::from([("threshold".to_string(), "0".to_string())]),
                operator: ConditionOperator::GreaterThan,
                negate: false,
            },
            TriggerCondition {
                condition_type: "value_comparison".to_string(),
                parameters: HashMap::from([
                    ("field".to_string(), "status".to_string()),
                    ("value".to_string(), "active".to_string()),
                ]),
                operator: ConditionOperator::Equals,
                negate: false,
            },
        ];

        let result = evaluator.evaluate_conditions(&conditions, &event).await.unwrap();
        assert!(result); // Both conditions should be true
    }

    #[tokio::test]
    async fn test_complex_condition() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let complex_condition = ComplexCondition {
            conditions: vec![
                TriggerCondition {
                    condition_type: "row_count".to_string(),
                    parameters: HashMap::from([("threshold".to_string(), "0".to_string())]),
                    operator: ConditionOperator::GreaterThan,
                    negate: false,
                },
            ],
            logical_operator: LogicalOperator::And,
        };

        let result = evaluator.evaluate_complex_condition(&complex_condition, &event).await.unwrap();
        assert!(result);
    }

    #[tokio::test]
    async fn test_regex_condition() {
        let evaluator = ConditionEvaluator::new();
        let event = create_test_event();

        let condition = TriggerCondition {
            condition_type: "regex_match".to_string(),
            parameters: HashMap::from([
                ("field".to_string(), "email".to_string()),
                ("pattern".to_string(), r"@example\.com$".to_string()),
            ]),
            operator: ConditionOperator::Equals, // Not used for regex
            negate: false,
        };

        let result = evaluator.evaluate_condition(&condition, &event).await.unwrap();
        assert!(result);
    }

    #[test]
    fn test_condition_stats() {
        let evaluator = ConditionEvaluator::new();
        let stats = evaluator.get_condition_stats();

        assert_eq!(stats.total_evaluations, 0);
        assert_eq!(stats.cache_hits, 0);
        assert_eq!(stats.cache_misses, 0);
    }

    #[tokio::test]
    async fn test_precompile_conditions() {
        let evaluator = ConditionEvaluator::new();

        let conditions = vec![
            TriggerCondition {
                condition_type: "row_count".to_string(),
                parameters: HashMap::from([("threshold".to_string(), "1".to_string())]),
                operator: ConditionOperator::GreaterThan,
                negate: false,
            },
        ];

        let compiled = evaluator.precompile_conditions(&conditions).await.unwrap();
        assert_eq!(compiled.len(), 1);
        assert_eq!(compiled[0].condition_type, "row_count");
    }
}
