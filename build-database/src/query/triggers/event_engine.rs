//! Event Engine: Intelligent Database Event Processing
//!
//! Advanced event-driven architecture that captures, filters, and routes
//! database events to appropriate triggers with intelligent optimization.

use std::collections::{HashMap, HashSet};
use std::sync::Arc;
use parking_lot::RwLock;
use chrono::{DateTime, Utc};
use crate::core::errors::{AuroraResult, AuroraError};
use super::trigger_manager::TriggerDefinition;

/// Database event types
#[derive(Debug, Clone, PartialEq)]
pub enum EventType {
    BeforeInsert,
    AfterInsert,
    BeforeUpdate,
    AfterUpdate,
    BeforeDelete,
    AfterDelete,
    BeforeTruncate,
    AfterTruncate,
    BeforeSelect,
    AfterSelect,
    TransactionBegin,
    TransactionCommit,
    TransactionRollback,
    DDLChange,
}

/// Database event data
#[derive(Debug, Clone)]
pub struct DatabaseEvent {
    pub event_type: EventType,
    pub table_name: String,
    pub operation: String,
    pub timestamp: DateTime<Utc>,
    pub transaction_id: Option<String>,
    pub user_id: Option<String>,
    pub session_id: Option<String>,
    pub old_values: Option<HashMap<String, String>>, // For UPDATE/DELETE
    pub new_values: Option<HashMap<String, String>>, // For INSERT/UPDATE
    pub affected_rows: u64,
    pub query_text: Option<String>,
    pub client_info: Option<HashMap<String, String>>,
}

/// Event filter for intelligent trigger selection
#[derive(Debug, Clone)]
pub struct EventFilter {
    pub table_name: Option<String>,
    pub event_types: HashSet<EventType>,
    pub user_filter: Option<String>,
    pub time_window: Option<TimeWindow>,
    pub condition_filters: Vec<String>,
    pub priority_threshold: Option<i32>,
}

/// Time window for event filtering
#[derive(Debug, Clone)]
pub struct TimeWindow {
    pub start_time: DateTime<Utc>,
    pub end_time: Option<DateTime<Utc>>,
}

/// Event processing statistics
#[derive(Debug)]
pub struct EventStats {
    pub total_events: u64,
    pub processed_events: u64,
    pub filtered_events: u64,
    pub trigger_executions: u64,
    pub avg_processing_time_ms: f64,
    pub peak_events_per_second: u64,
}

/// Event batch for efficient processing
#[derive(Debug)]
pub struct EventBatch {
    pub events: Vec<DatabaseEvent>,
    pub batch_id: String,
    pub created_at: DateTime<Utc>,
    pub priority: EventPriority,
}

/// Event priority levels
#[derive(Debug, Clone, PartialEq, PartialOrd)]
pub enum EventPriority {
    Low,
    Normal,
    High,
    Critical,
}

/// Intelligent event engine
pub struct EventEngine {
    event_filters: RwLock<HashMap<String, EventFilter>>, // trigger_name -> filter
    event_queue: RwLock<Vec<EventBatch>>,
    event_stats: RwLock<EventStats>,
    trigger_mappings: RwLock<HashMap<String, Vec<String>>>, // table -> trigger names
    event_processors: RwLock<HashMap<EventType, Vec<String>>>, // event_type -> trigger names
    performance_monitor: Arc<EventPerformanceMonitor>,
}

impl EventEngine {
    pub fn new() -> Self {
        Self {
            event_filters: RwLock::new(HashMap::new()),
            event_queue: RwLock::new(Vec::new()),
            event_stats: RwLock::new(EventStats {
                total_events: 0,
                processed_events: 0,
                filtered_events: 0,
                trigger_executions: 0,
                avg_processing_time_ms: 0.0,
                peak_events_per_second: 0,
            }),
            trigger_mappings: RwLock::new(HashMap::new()),
            event_processors: RwLock::new(HashMap::new()),
            performance_monitor: Arc::new(EventPerformanceMonitor::new()),
        }
    }

    /// Register a trigger with the event engine
    pub async fn register_trigger(&self, trigger: &TriggerDefinition) -> AuroraResult<()> {
        println!("📡 Registering trigger '{}' for table '{}' events", trigger.name, trigger.table_name);

        // Create event filter for the trigger
        let event_filter = self.create_event_filter(trigger)?;

        // Store filter
        {
            let mut filters = self.event_filters.write();
            filters.insert(trigger.name.clone(), event_filter);
        }

        // Update table mappings
        {
            let mut mappings = self.trigger_mappings.write();
            mappings.entry(trigger.table_name.clone())
                .or_insert_with(Vec::new)
                .push(trigger.name.clone());
        }

        // Update event type mappings
        self.update_event_type_mappings(trigger)?;

        println!("✅ Registered trigger '{}' - monitoring {} event types",
                trigger.name, trigger.events.len());

        Ok(())
    }

    /// Unregister a trigger
    pub async fn unregister_trigger(&self, trigger_name: &str) -> AuroraResult<()> {
        // Remove filter
        {
            let mut filters = self.event_filters.write();
            filters.remove(trigger_name);
        }

        // Remove from table mappings
        {
            let mut mappings = self.trigger_mappings.write();
            for triggers in mappings.values_mut() {
                triggers.retain(|t| t != trigger_name);
            }
            // Remove empty mappings
            mappings.retain(|_, triggers| !triggers.is_empty());
        }

        // Remove from event type mappings
        {
            let mut processors = self.event_processors.write();
            for triggers in processors.values_mut() {
                triggers.retain(|t| t != trigger_name);
            }
        }

        Ok(())
    }

    /// Process a database event
    pub async fn process_event(&self, event: DatabaseEvent) -> AuroraResult<Vec<String>> {
        let start_time = std::time::Instant::now();

        // Update statistics
        {
            let mut stats = self.event_stats.write();
            stats.total_events += 1;
        }

        // Find relevant triggers
        let relevant_triggers = self.find_relevant_triggers(&event).await?;

        // Filter triggers based on their conditions
        let filtered_triggers = self.apply_event_filters(&event, &relevant_triggers).await?;

        // Update statistics
        {
            let mut stats = self.event_stats.write();
            stats.processed_events += 1;
            stats.filtered_events += relevant_triggers.len() as u64 - filtered_triggers.len() as u64;
        }

        // Record performance
        let processing_time = start_time.elapsed().as_millis() as f64;
        self.performance_monitor.record_event_processing(&event, processing_time).await?;

        Ok(filtered_triggers)
    }

    /// Process events in batch for efficiency
    pub async fn process_event_batch(&self, batch: EventBatch) -> AuroraResult<HashMap<String, Vec<String>>> {
        println!("📦 Processing event batch '{}' with {} events", batch.batch_id, batch.events.len());

        let mut trigger_events = HashMap::new();

        for event in &batch.events {
            let triggers = self.process_event(event.clone()).await?;
            for trigger in triggers {
                trigger_events.entry(trigger).or_insert_with(Vec::new).push(event.table_name.clone());
            }
        }

        // Group events by trigger for efficient execution
        let mut result = HashMap::new();
        for (trigger, tables) in trigger_events {
            let unique_tables: HashSet<String> = tables.into_iter().collect();
            result.insert(trigger, unique_tables.into_iter().collect());
        }

        Ok(result)
    }

    /// Queue event for deferred processing
    pub async fn queue_event(&self, event: DatabaseEvent, priority: EventPriority) -> AuroraResult<()> {
        let batch = EventBatch {
            events: vec![event],
            batch_id: format!("batch_{}", Utc::now().timestamp()),
            created_at: Utc::now(),
            priority,
        };

        let mut queue = self.event_queue.write();
        queue.push(batch);

        Ok(())
    }

    /// Process queued events
    pub async fn process_queued_events(&self) -> AuroraResult<usize> {
        let batches = {
            let mut queue = self.event_queue.write();
            let batches = queue.drain(..).collect::<Vec<_>>();
            batches
        };

        let mut processed = 0;

        for batch in batches {
            self.process_event_batch(batch).await?;
            processed += 1;
        }

        Ok(processed)
    }

    /// Get event statistics
    pub fn get_event_stats(&self) -> EventStats {
        self.event_stats.read().clone()
    }

    /// Get triggers for a table
    pub async fn get_table_triggers(&self, table_name: &str) -> Vec<String> {
        let mappings = self.trigger_mappings.read();
        mappings.get(table_name).cloned().unwrap_or_default()
    }

    /// Get triggers for an event type
    pub async fn get_event_triggers(&self, event_type: &EventType) -> Vec<String> {
        let processors = self.event_processors.read();
        processors.get(event_type).cloned().unwrap_or_default()
    }

    // Private methods

    fn create_event_filter(&self, trigger: &TriggerDefinition) -> AuroraResult<EventFilter> {
        let mut event_types = HashSet::new();

        // Map trigger events to event types
        for event in &trigger.events {
            match event {
                super::trigger_manager::TriggerEvent::Insert => {
                    match trigger.timing {
                        super::trigger_manager::TriggerTiming::Before => event_types.insert(EventType::BeforeInsert),
                        super::trigger_manager::TriggerTiming::After => event_types.insert(EventType::AfterInsert),
                        super::trigger_manager::TriggerTiming::Instead => event_types.insert(EventType::BeforeInsert),
                    };
                }
                super::trigger_manager::TriggerEvent::Update => {
                    match trigger.timing {
                        super::trigger_manager::TriggerTiming::Before => event_types.insert(EventType::BeforeUpdate),
                        super::trigger_manager::TriggerTiming::After => event_types.insert(EventType::AfterUpdate),
                        super::trigger_manager::TriggerTiming::Instead => event_types.insert(EventType::BeforeUpdate),
                    };
                }
                super::trigger_manager::TriggerEvent::Delete => {
                    match trigger.timing {
                        super::trigger_manager::TriggerTiming::Before => event_types.insert(EventType::BeforeDelete),
                        super::trigger_manager::TriggerTiming::After => event_types.insert(EventType::AfterDelete),
                        super::trigger_manager::TriggerTiming::Instead => event_types.insert(EventType::BeforeDelete),
                    };
                }
                super::trigger_manager::TriggerEvent::Truncate => {
                    match trigger.timing {
                        super::trigger_manager::TriggerTiming::Before => event_types.insert(EventType::BeforeTruncate),
                        super::trigger_manager::TriggerTiming::After => event_types.insert(EventType::AfterTruncate),
                        super::trigger_manager::TriggerTiming::Instead => event_types.insert(EventType::BeforeTruncate),
                    };
                }
                super::trigger_manager::TriggerEvent::Select => {
                    match trigger.timing {
                        super::trigger_manager::TriggerTiming::Before => event_types.insert(EventType::BeforeSelect),
                        super::trigger_manager::TriggerTiming::After => event_types.insert(EventType::AfterSelect),
                        super::trigger_manager::TriggerTiming::Instead => event_types.insert(EventType::BeforeSelect),
                    };
                }
            }
        }

        Ok(EventFilter {
            table_name: Some(trigger.table_name.clone()),
            event_types,
            user_filter: None,
            time_window: None,
            condition_filters: vec![], // Would be populated from trigger conditions
            priority_threshold: Some(trigger.priority),
        })
    }

    fn update_event_type_mappings(&self, trigger: &TriggerDefinition) -> AuroraResult<()> {
        let filters = self.event_filters.read();
        if let Some(filter) = filters.get(&trigger.name) {
            let mut processors = self.event_processors.write();

            for event_type in &filter.event_types {
                processors.entry(event_type.clone())
                    .or_insert_with(Vec::new)
                    .push(trigger.name.clone());
            }
        }

        Ok(())
    }

    async fn find_relevant_triggers(&self, event: &DatabaseEvent) -> AuroraResult<Vec<String>> {
        let mut relevant = Vec::new();

        // Check table-specific triggers
        let table_triggers = self.get_table_triggers(&event.table_name).await;
        relevant.extend(table_triggers);

        // Check event-type-specific triggers
        let event_triggers = self.get_event_triggers(&event.event_type).await;
        relevant.extend(event_triggers);

        // Remove duplicates
        let mut unique: HashSet<String> = relevant.into_iter().collect();
        unique.extend(relevant);

        Ok(unique.into_iter().collect())
    }

    async fn apply_event_filters(&self, event: &DatabaseEvent, triggers: &[String]) -> AuroraResult<Vec<String>> {
        let mut filtered = Vec::new();
        let filters = self.event_filters.read();

        for trigger_name in triggers {
            if let Some(filter) = filters.get(trigger_name) {
                if self.event_matches_filter(event, filter)? {
                    filtered.push(trigger_name.clone());
                }
            } else {
                // No filter means always match
                filtered.push(trigger_name.clone());
            }
        }

        Ok(filtered)
    }

    fn event_matches_filter(&self, event: &DatabaseEvent, filter: &EventFilter) -> AuroraResult<bool> {
        // Check table name
        if let Some(table_filter) = &filter.table_name {
            if *table_filter != event.table_name {
                return Ok(false);
            }
        }

        // Check event type
        if !filter.event_types.contains(&event.event_type) {
            return Ok(false);
        }

        // Check user filter
        if let Some(user_filter) = &filter.user_filter {
            if let Some(user_id) = &event.user_id {
                if user_id != user_filter {
                    return Ok(false);
                }
            } else {
                return Ok(false);
            }
        }

        // Check time window
        if let Some(time_window) = &filter.time_window {
            if event.timestamp < time_window.start_time {
                return Ok(false);
            }
            if let Some(end_time) = time_window.end_time {
                if event.timestamp > end_time {
                    return Ok(false);
                }
            }
        }

        // Check condition filters (simplified - would evaluate actual conditions)
        for condition in &filter.condition_filters {
            if !self.evaluate_condition_filter(condition, event)? {
                return Ok(false);
            }
        }

        Ok(true)
    }

    fn evaluate_condition_filter(&self, condition: &str, event: &DatabaseEvent) -> AuroraResult<bool> {
        // Simplified condition evaluation
        // In a real implementation, this would parse and evaluate complex conditions
        match condition {
            "active_users_only" => {
                if let Some(new_values) = &event.new_values {
                    if let Some(status) = new_values.get("status") {
                        return Ok(status == "active");
                    }
                }
                Ok(false)
            }
            "high_value_transactions" => {
                if let Some(new_values) = &event.new_values {
                    if let Some(amount_str) = new_values.get("amount") {
                        if let Ok(amount) = amount_str.parse::<f64>() {
                            return Ok(amount > 1000.0);
                        }
                    }
                }
                Ok(false)
            }
            _ => Ok(true), // Unknown conditions pass through
        }
    }
}

/// Event performance monitor
#[derive(Debug)]
pub struct EventPerformanceMonitor {
    processing_times: RwLock<Vec<f64>>,
    event_counts: RwLock<HashMap<String, u64>>,
}

impl EventPerformanceMonitor {
    pub fn new() -> Self {
        Self {
            processing_times: RwLock::new(Vec::new()),
            event_counts: RwLock::new(HashMap::new()),
        }
    }

    async fn record_event_processing(&self, event: &DatabaseEvent, processing_time_ms: f64) -> AuroraResult<()> {
        // Record processing time
        {
            let mut times = self.processing_times.write();
            times.push(processing_time_ms);

            // Keep only last 1000 measurements
            if times.len() > 1000 {
                times.remove(0);
            }
        }

        // Record event count
        {
            let mut counts = self.event_counts.write();
            let key = format!("{:?}_{}", event.event_type, event.table_name);
            *counts.entry(key).or_insert(0) += 1;
        }

        Ok(())
    }

    pub fn get_avg_processing_time(&self) -> f64 {
        let times = self.processing_times.read();
        if times.is_empty() {
            0.0
        } else {
            times.iter().sum::<f64>() / times.len() as f64
        }
    }

    pub fn get_event_counts(&self) -> HashMap<String, u64> {
        self.event_counts.read().clone()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_event_engine_creation() {
        let engine = EventEngine::new();
        assert!(true); // Passes if created successfully
    }

    #[test]
    fn test_event_types() {
        assert_eq!(EventType::BeforeInsert, EventType::BeforeInsert);
        assert_ne!(EventType::AfterUpdate, EventType::BeforeDelete);
    }

    #[test]
    fn test_database_event() {
        let event = DatabaseEvent {
            event_type: EventType::AfterInsert,
            table_name: "users".to_string(),
            operation: "INSERT".to_string(),
            timestamp: Utc::now(),
            transaction_id: Some("tx_123".to_string()),
            user_id: Some("user_456".to_string()),
            session_id: Some("session_789".to_string()),
            old_values: None,
            new_values: Some(HashMap::from([
                ("id".to_string(), "1".to_string()),
                ("name".to_string(), "John Doe".to_string()),
            ])),
            affected_rows: 1,
            query_text: Some("INSERT INTO users (name) VALUES ('John Doe')".to_string()),
            client_info: Some(HashMap::from([
                ("client_version".to_string(), "1.0.0".to_string()),
            ])),
        };

        assert_eq!(event.table_name, "users");
        assert_eq!(event.affected_rows, 1);
        assert!(event.new_values.is_some());
    }

    #[test]
    fn test_event_filter() {
        let filter = EventFilter {
            table_name: Some("users".to_string()),
            event_types: HashSet::from([EventType::AfterInsert, EventType::AfterUpdate]),
            user_filter: Some("admin".to_string()),
            time_window: None,
            condition_filters: vec!["active_users_only".to_string()],
            priority_threshold: Some(5),
        };

        assert_eq!(filter.table_name, Some("users".to_string()));
        assert_eq!(filter.event_types.len(), 2);
        assert_eq!(filter.priority_threshold, Some(5));
    }

    #[test]
    fn test_event_priority() {
        assert!(EventPriority::Low < EventPriority::High);
        assert!(EventPriority::Normal > EventPriority::Low);
    }

    #[test]
    fn test_event_batch() {
        let events = vec![
            DatabaseEvent {
                event_type: EventType::AfterInsert,
                table_name: "users".to_string(),
                operation: "INSERT".to_string(),
                timestamp: Utc::now(),
                transaction_id: None,
                user_id: None,
                session_id: None,
                old_values: None,
                new_values: None,
                affected_rows: 1,
                query_text: None,
                client_info: None,
            }
        ];

        let batch = EventBatch {
            events,
            batch_id: "batch_123".to_string(),
            created_at: Utc::now(),
            priority: EventPriority::Normal,
        };

        assert_eq!(batch.events.len(), 1);
        assert_eq!(batch.batch_id, "batch_123");
        assert_eq!(batch.priority, EventPriority::Normal);
    }

    #[tokio::test]
    async fn test_event_performance_monitor() {
        let monitor = EventPerformanceMonitor::new();

        let event = DatabaseEvent {
            event_type: EventType::AfterInsert,
            table_name: "users".to_string(),
            operation: "INSERT".to_string(),
            timestamp: Utc::now(),
            transaction_id: None,
            user_id: None,
            session_id: None,
            old_values: None,
            new_values: None,
            affected_rows: 1,
            query_text: None,
            client_info: None,
        };

        monitor.record_event_processing(&event, 150.0).await.unwrap();

        let avg_time = monitor.get_avg_processing_time();
        assert_eq!(avg_time, 150.0);

        let counts = monitor.get_event_counts();
        assert!(counts.contains_key("AfterInsert_users"));
    }

    #[tokio::test]
    async fn test_event_stats() {
        let engine = EventEngine::new();
        let stats = engine.get_event_stats();

        assert_eq!(stats.total_events, 0);
        assert_eq!(stats.processed_events, 0);
        assert_eq!(stats.avg_processing_time_ms, 0.0);
    }
}
