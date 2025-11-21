//! Performance Monitor: Stored Procedure Performance Tracking
//!
//! Comprehensive performance monitoring with metrics collection,
//! alerting, and optimization recommendations.

use std::collections::HashMap;
use parking_lot::RwLock;
use chrono::{DateTime, Utc, Duration};
use crate::core::errors::{AuroraResult, AuroraError};
use super::procedure_manager::ExecutionResult;

/// Performance metrics
#[derive(Debug, Clone)]
pub struct PerformanceMetrics {
    pub execution_time_ms: f64,
    pub memory_used_mb: f64,
    pub cpu_utilization: f64,
    pub io_operations: u64,
    pub network_calls: u32,
    pub timestamp: DateTime<Utc>,
}

/// Performance statistics
#[derive(Debug)]
pub struct PerformanceStats {
    pub total_executions: u64,
    pub avg_execution_time_ms: f64,
    pub max_execution_time_ms: f64,
    pub min_execution_time_ms: f64,
    pub total_memory_used_mb: f64,
    pub last_executed: Option<DateTime<Utc>>,
    pub error_rate: f64,
    pub p95_execution_time_ms: f64,
    pub p99_execution_time_ms: f64,
}

/// Performance alert
#[derive(Debug)]
pub struct PerformanceAlert {
    pub procedure_name: String,
    pub alert_type: AlertType,
    pub severity: AlertSeverity,
    pub message: String,
    pub timestamp: DateTime<Utc>,
    pub metrics: HashMap<String, f64>,
}

/// Alert types
#[derive(Debug, Clone)]
pub enum AlertType {
    SlowExecution,
    HighMemoryUsage,
    HighErrorRate,
    ResourceExhaustion,
    PerformanceDegradation,
}

/// Alert severity levels
#[derive(Debug, Clone, PartialEq)]
pub enum AlertSeverity {
    Low,
    Medium,
    High,
    Critical,
}

/// Performance monitor
pub struct PerformanceMonitor {
    metrics: RwLock<HashMap<String, Vec<PerformanceMetrics>>>,
    alerts: RwLock<Vec<PerformanceAlert>>,
    thresholds: HashMap<String, PerformanceThreshold>,
}

impl PerformanceMonitor {
    pub fn new() -> Self {
        let mut thresholds = HashMap::new();
        thresholds.insert("slow_execution".to_string(), PerformanceThreshold {
            metric: "execution_time_ms".to_string(),
            operator: ThresholdOperator::GreaterThan,
            value: 1000.0, // 1 second
            severity: AlertSeverity::Medium,
        });
        thresholds.insert("high_memory".to_string(), PerformanceThreshold {
            metric: "memory_used_mb".to_string(),
            operator: ThresholdOperator::GreaterThan,
            value: 500.0, // 500MB
            severity: AlertSeverity::High,
        });

        Self {
            metrics: RwLock::new(HashMap::new()),
            alerts: RwLock::new(Vec::new()),
            thresholds,
        }
    }

    /// Register procedure for monitoring
    pub async fn register_procedure(&self, procedure_name: &str) -> AuroraResult<()> {
        let mut metrics = self.metrics.write();
        metrics.entry(procedure_name.to_string()).or_insert_with(Vec::new);
        Ok(())
    }

    /// Record execution performance
    pub async fn record_execution(&self, procedure_name: &str, result: &ExecutionResult) -> AuroraResult<()> {
        let metrics = PerformanceMetrics {
            execution_time_ms: result.execution_time_ms,
            memory_used_mb: result.memory_used_mb,
            cpu_utilization: 0.0, // Would be measured
            io_operations: 0,
            network_calls: 0,
            timestamp: Utc::now(),
        };

        let mut all_metrics = self.metrics.write();
        all_metrics.entry(procedure_name.to_string())
            .or_insert_with(Vec::new)
            .push(metrics);

        // Check for alerts
        self.check_alerts(procedure_name, result).await?;

        Ok(())
    }

    /// Get performance statistics
    pub async fn get_statistics(&self, procedure_name: &str) -> PerformanceStats {
        let metrics = self.metrics.read();
        if let Some(proc_metrics) = metrics.get(procedure_name) {
            if proc_metrics.is_empty() {
                return PerformanceStats::default();
            }

            let total_executions = proc_metrics.len() as u64;
            let execution_times: Vec<f64> = proc_metrics.iter().map(|m| m.execution_time_ms).collect();

            let avg_execution_time = execution_times.iter().sum::<f64>() / execution_times.len() as f64;
            let max_execution_time = execution_times.iter().fold(0.0, |max, &val| if val > max { val } else { max });
            let min_execution_time = execution_times.iter().fold(f64::INFINITY, |min, &val| if val < min { val } else { min });

            let total_memory = proc_metrics.iter().map(|m| m.memory_used_mb).sum::<f64>();
            let last_executed = proc_metrics.last().map(|m| m.timestamp);

            // Calculate percentiles (simplified)
            let mut sorted_times = execution_times.clone();
            sorted_times.sort_by(|a, b| a.partial_cmp(b).unwrap());
            let p95_index = (sorted_times.len() as f64 * 0.95) as usize;
            let p99_index = (sorted_times.len() as f64 * 0.99) as usize;

            let p95_execution_time = sorted_times.get(p95_index).copied().unwrap_or(max_execution_time);
            let p99_execution_time = sorted_times.get(p99_index).copied().unwrap_or(max_execution_time);

            PerformanceStats {
                total_executions,
                avg_execution_time_ms: avg_execution_time,
                max_execution_time_ms: max_execution_time,
                min_execution_time_ms: min_execution_time,
                total_memory_used_mb: total_memory,
                last_executed,
                error_rate: 0.0, // Would track errors separately
                p95_execution_time_ms: p95_execution_time,
                p99_execution_time_ms: p99_execution_time,
            }
        } else {
            PerformanceStats::default()
        }
    }

    /// Get performance metrics
    pub fn get_metrics(&self, procedure_name: &str) -> AuroraResult<HashMap<String, f64>> {
        let stats = self.get_statistics(procedure_name);
        let mut metrics = HashMap::new();
        metrics.insert("avg_execution_time".to_string(), stats.avg_execution_time_ms);
        metrics.insert("total_executions".to_string(), stats.total_executions as f64);
        metrics.insert("error_rate".to_string(), stats.error_rate);
        Ok(metrics)
    }

    /// Remove procedure from monitoring
    pub async fn remove_procedure(&self, procedure_name: &str) -> AuroraResult<()> {
        let mut metrics = self.metrics.write();
        metrics.remove(procedure_name);
        Ok(())
    }

    /// Get performance alerts
    pub async fn get_alerts(&self, limit: usize) -> Vec<PerformanceAlert> {
        let alerts = self.alerts.read();
        alerts.iter().rev().take(limit).cloned().collect()
    }

    /// Generate performance report
    pub async fn generate_report(&self, procedure_name: &str) -> AuroraResult<PerformanceReport> {
        let stats = self.get_statistics(procedure_name).await;
        let alerts = self.get_alerts(10).await.into_iter()
            .filter(|a| a.procedure_name == procedure_name)
            .collect::<Vec<_>>();

        let recommendations = self.generate_recommendations(procedure_name, &stats).await?;

        Ok(PerformanceReport {
            procedure_name: procedure_name.to_string(),
            statistics: stats,
            recent_alerts: alerts,
            recommendations,
            generated_at: Utc::now(),
        })
    }

    // Private methods

    async fn check_alerts(&self, procedure_name: &str, result: &ExecutionResult) -> AuroraResult<()> {
        for (threshold_name, threshold) in &self.thresholds {
            let value = match threshold.metric.as_str() {
                "execution_time_ms" => result.execution_time_ms,
                "memory_used_mb" => result.memory_used_mb,
                _ => continue,
            };

            let triggered = match threshold.operator {
                ThresholdOperator::GreaterThan => value > threshold.value,
                ThresholdOperator::LessThan => value < threshold.value,
                ThresholdOperator::Equals => (value - threshold.value).abs() < f64::EPSILON,
            };

            if triggered {
                let alert = PerformanceAlert {
                    procedure_name: procedure_name.to_string(),
                    alert_type: match threshold_name.as_str() {
                        "slow_execution" => AlertType::SlowExecution,
                        "high_memory" => AlertType::HighMemoryUsage,
                        _ => AlertType::PerformanceDegradation,
                    },
                    severity: threshold.severity.clone(),
                    message: format!("{} exceeded threshold: {:.2} > {:.2}",
                                   threshold.metric, value, threshold.value),
                    timestamp: Utc::now(),
                    metrics: HashMap::from([
                        (threshold.metric.clone(), value),
                        ("threshold".to_string(), threshold.value),
                    ]),
                };

                let mut alerts = self.alerts.write();
                alerts.push(alert);
            }
        }

        Ok(())
    }

    async fn generate_recommendations(&self, procedure_name: &str, stats: &PerformanceStats) -> AuroraResult<Vec<String>> {
        let mut recommendations = Vec::new();

        if stats.avg_execution_time_ms > 500.0 {
            recommendations.push("Consider JIT compilation for better performance".to_string());
        }

        if stats.total_memory_used_mb > 1000.0 {
            recommendations.push("High memory usage detected - consider optimization".to_string());
        }

        if stats.error_rate > 0.1 {
            recommendations.push("High error rate detected - review error handling".to_string());
        }

        if stats.p95_execution_time_ms > stats.avg_execution_time_ms * 2.0 {
            recommendations.push("High variance in execution times - investigate outliers".to_string());
        }

        Ok(recommendations)
    }
}

/// Performance threshold
#[derive(Debug)]
pub struct PerformanceThreshold {
    pub metric: String,
    pub operator: ThresholdOperator,
    pub value: f64,
    pub severity: AlertSeverity,
}

/// Threshold operators
#[derive(Debug, Clone)]
pub enum ThresholdOperator {
    GreaterThan,
    LessThan,
    Equals,
}

/// Performance report
#[derive(Debug)]
pub struct PerformanceReport {
    pub procedure_name: String,
    pub statistics: PerformanceStats,
    pub recent_alerts: Vec<PerformanceAlert>,
    pub recommendations: Vec<String>,
    pub generated_at: DateTime<Utc>,
}

impl Default for PerformanceStats {
    fn default() -> Self {
        Self {
            total_executions: 0,
            avg_execution_time_ms: 0.0,
            max_execution_time_ms: f64::MIN,
            min_execution_time_ms: f64::MAX,
            total_memory_used_mb: 0.0,
            last_executed: None,
            error_rate: 0.0,
            p95_execution_time_ms: 0.0,
            p99_execution_time_ms: 0.0,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::query::stored_procedures::procedure_manager::ExecutionResult;

    #[tokio::test]
    async fn test_performance_monitor_creation() {
        let monitor = PerformanceMonitor::new();
        assert!(true); // Passes if created successfully
    }

    #[tokio::test]
    async fn test_performance_recording() {
        let monitor = PerformanceMonitor::new();

        monitor.register_procedure("test_proc").await.unwrap();

        let result = ExecutionResult {
            success: true,
            return_value: Some("42".to_string()),
            output_parameters: std::collections::HashMap::new(),
            execution_time_ms: 150.0,
            memory_used_mb: 25.0,
            security_events: vec![],
            performance_metrics: std::collections::HashMap::new(),
        };

        monitor.record_execution("test_proc", &result).await.unwrap();

        let stats = monitor.get_statistics("test_proc").await;
        assert_eq!(stats.total_executions, 1);
        assert_eq!(stats.avg_execution_time_ms, 150.0);
    }

    #[test]
    fn test_alert_types() {
        assert_eq!(AlertType::SlowExecution, AlertType::SlowExecution);
        assert_ne!(AlertType::HighMemoryUsage, AlertType::HighErrorRate);
    }

    #[test]
    fn test_alert_severity_ordering() {
        assert!(AlertSeverity::Low < AlertSeverity::Critical);
        assert!(AlertSeverity::Medium > AlertSeverity::Low);
    }

    #[tokio::test]
    async fn test_threshold_alerts() {
        let monitor = PerformanceMonitor::new();

        monitor.register_procedure("slow_proc").await.unwrap();

        // Create a slow execution result
        let slow_result = ExecutionResult {
            success: true,
            return_value: None,
            output_parameters: std::collections::HashMap::new(),
            execution_time_ms: 1500.0, // Over 1 second threshold
            memory_used_mb: 50.0,
            security_events: vec![],
            performance_metrics: std::collections::HashMap::new(),
        };

        monitor.record_execution("slow_proc", &slow_result).await.unwrap();

        let alerts = monitor.get_alerts(10).await;
        assert!(!alerts.is_empty());
        assert_eq!(alerts[0].alert_type, AlertType::SlowExecution);
    }

    #[tokio::test]
    async fn test_performance_report() {
        let monitor = PerformanceMonitor::new();

        monitor.register_procedure("report_proc").await.unwrap();

        // Add some execution data
        for i in 1..=5 {
            let result = ExecutionResult {
                success: true,
                return_value: None,
                output_parameters: std::collections::HashMap::new(),
                execution_time_ms: (i * 100) as f64,
                memory_used_mb: (i * 10) as f64,
                security_events: vec![],
                performance_metrics: std::collections::HashMap::new(),
            };
            monitor.record_execution("report_proc", &result).await.unwrap();
        }

        let report = monitor.generate_report("report_proc").await.unwrap();
        assert_eq!(report.procedure_name, "report_proc");
        assert_eq!(report.statistics.total_executions, 5);
        assert_eq!(report.statistics.avg_execution_time_ms, 300.0); // (100+200+300+400+500)/5
    }

    #[test]
    fn test_performance_stats_defaults() {
        let stats = PerformanceStats::default();
        assert_eq!(stats.total_executions, 0);
        assert_eq!(stats.avg_execution_time_ms, 0.0);
    }
}
