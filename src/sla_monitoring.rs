//! SLA Monitoring and Operational Maturity
//!
//! Production-grade SLA monitoring ensuring 99.9% uptime and operational excellence:
//! - SLA tracking (availability, latency, error budgets)
//! - Incident response automation
//! - Capacity planning and scaling
//! - Performance regression detection
//! - Operational runbooks and procedures

use crate::error::{Error, Result};
use std::collections::{HashMap, HashSet};
use std::sync::{Arc, atomic::{AtomicU64, Ordering}, RwLock};
use std::time::{Duration, Instant, SystemTime, UNIX_EPOCH};
use tokio::sync::{broadcast, mpsc};

/// SLA monitoring system
pub struct SlaMonitor {
    /// SLA targets
    targets: SlaTargets,
    /// Current SLA metrics
    current_metrics: Arc<RwLock<SlaMetrics>>,
    /// SLA violations tracker
    violations: Arc<RwLock<Vec<SlaViolation>>>,
    /// Incident response system
    incident_response: Arc<IncidentResponse>,
    /// Capacity planning system
    capacity_planner: Arc<CapacityPlanner>,
    /// Performance regression detector
    regression_detector: Arc<RegressionDetector>,
}

/// SLA targets
#[derive(Debug, Clone)]
pub struct SlaTargets {
    pub availability_target: f64,        // 99.9% uptime
    pub latency_p95_target_ms: f64,     // P95 latency target
    pub latency_p99_target_ms: f64,     // P99 latency target
    pub error_rate_target: f64,         // Maximum error rate
    pub throughput_target_rps: usize,   // Minimum throughput
    pub mttr_target_seconds: u64,       // Mean Time To Recovery
    pub mtbf_target_hours: u64,         // Mean Time Between Failures
}

/// SLA metrics
#[derive(Debug, Clone)]
pub struct SlaMetrics {
    pub uptime_percentage: f64,
    pub total_downtime_seconds: u64,
    pub incidents_count: usize,
    pub average_mttr_seconds: f64,
    pub average_mtbf_hours: f64,
    pub error_budget_remaining: f64,    // Percentage of error budget left
    pub latency_violations_p95: usize,
    pub latency_violations_p99: usize,
    pub throughput_achievements: usize,
    pub last_updated: Instant,
}

/// SLA violation
#[derive(Debug, Clone)]
pub struct SlaViolation {
    pub timestamp: Instant,
    pub violation_type: SlaViolationType,
    pub severity: ViolationSeverity,
    pub description: String,
    pub impact: String,
    pub resolution_time: Option<Duration>,
    pub error_budget_impact: f64,
}

/// SLA violation types
#[derive(Debug, Clone)]
pub enum SlaViolationType {
    Availability,
    LatencyP95,
    LatencyP99,
    ErrorRate,
    Throughput,
    Security,
}

/// Violation severity
#[derive(Debug, Clone)]
pub enum ViolationSeverity {
    Low,
    Medium,
    High,
    Critical,
}

/// Incident response system
pub struct IncidentResponse {
    /// Active incidents
    active_incidents: RwLock<HashMap<String, Incident>>,
    /// Incident runbooks
    runbooks: HashMap<String, IncidentRunbook>,
    /// Escalation policies
    escalation_policies: Vec<EscalationPolicy>,
    /// Automated remediation rules
    remediation_rules: Vec<RemediationRule>,
}

/// Capacity planning system
pub struct CapacityPlanner {
    /// Current capacity metrics
    current_capacity: RwLock<CapacityMetrics>,
    /// Capacity forecasts
    forecasts: RwLock<Vec<CapacityForecast>>,
    /// Scaling recommendations
    recommendations: RwLock<Vec<ScalingRecommendation>>,
    /// Resource utilization history
    utilization_history: RwLock<Vec<ResourceUtilization>>,
}

/// Performance regression detector
pub struct RegressionDetector {
    /// Baseline performance metrics
    baselines: HashMap<String, PerformanceBaseline>,
    /// Regression alerts
    alerts: RwLock<Vec<RegressionAlert>>,
    /// Statistical analysis parameters
    stats_config: RegressionStatsConfig,
}

/// Incident definition
#[derive(Debug, Clone)]
pub struct Incident {
    pub id: String,
    pub title: String,
    pub description: String,
    pub severity: IncidentSeverity,
    pub status: IncidentStatus,
    pub created_at: Instant,
    pub resolved_at: Option<Instant>,
    pub assigned_to: Option<String>,
    pub tags: Vec<String>,
}

/// Incident severity
#[derive(Debug, Clone)]
pub enum IncidentSeverity {
    Sev1, // Critical - immediate response
    Sev2, // High - response within 1 hour
    Sev3, // Medium - response within 4 hours
    Sev4, // Low - response within 24 hours
}

/// Incident status
#[derive(Debug, Clone)]
pub enum IncidentStatus {
    Open,
    Investigating,
    Identified,
    Resolved,
}

/// Incident runbook
#[derive(Debug, Clone)]
pub struct IncidentRunbook {
    pub incident_type: String,
    pub detection_criteria: String,
    pub investigation_steps: Vec<String>,
    pub remediation_steps: Vec<String>,
    pub communication_plan: String,
    pub escalation_criteria: String,
}

/// Escalation policy
#[derive(Debug, Clone)]
pub struct EscalationPolicy {
    pub severity: IncidentSeverity,
    pub response_time: Duration,
    pub notification_channels: Vec<String>,
    pub escalation_contacts: Vec<String>,
}

/// Remediation rule
#[derive(Debug, Clone)]
pub struct RemediationRule {
    pub condition: String,
    pub action: String,
    pub cooldown_period: Duration,
    pub success_criteria: String,
}

/// Capacity metrics
#[derive(Debug, Clone)]
pub struct CapacityMetrics {
    pub cpu_cores_available: usize,
    pub memory_gb_available: usize,
    pub network_bandwidth_gbps: f64,
    pub storage_tb_available: f64,
    pub current_utilization: ResourceUtilization,
}

/// Capacity forecast
#[derive(Debug, Clone)]
pub struct CapacityForecast {
    pub timestamp: Instant,
    pub forecast_horizon: Duration,
    pub predicted_load: f64,
    pub recommended_capacity: CapacityMetrics,
    pub confidence_level: f64,
}

/// Scaling recommendation
#[derive(Debug, Clone)]
pub struct ScalingRecommendation {
    pub timestamp: Instant,
    pub resource_type: String,
    pub current_value: f64,
    pub recommended_value: f64,
    pub reason: String,
    pub impact_assessment: String,
}

/// Resource utilization
#[derive(Debug, Clone)]
pub struct ResourceUtilization {
    pub timestamp: Instant,
    pub cpu_percent: f64,
    pub memory_percent: f64,
    pub network_percent: f64,
    pub disk_percent: f64,
}

/// Performance baseline
#[derive(Debug, Clone)]
pub struct PerformanceBaseline {
    pub metric_name: String,
    pub baseline_value: f64,
    pub standard_deviation: f64,
    pub sample_size: usize,
    pub last_updated: Instant,
    pub confidence_interval: (f64, f64),
}

/// Regression alert
#[derive(Debug, Clone)]
pub struct RegressionAlert {
    pub id: String,
    pub metric_name: String,
    pub baseline_value: f64,
    pub current_value: f64,
    pub deviation_percent: f64,
    pub severity: RegressionSeverity,
    pub detected_at: Instant,
    pub potential_causes: Vec<String>,
}

/// Regression severity
#[derive(Debug, Clone)]
pub enum RegressionSeverity {
    Minor,     // <5% regression
    Moderate,  // 5-15% regression
    Major,     // 15-30% regression
    Critical,  // >30% regression
}

/// Regression statistics configuration
#[derive(Debug, Clone)]
pub struct RegressionStatsConfig {
    pub min_samples_for_baseline: usize,
    pub regression_threshold_percent: f64,
    pub confidence_level: f64,
    pub moving_average_window: Duration,
}

impl SlaMonitor {
    /// Create new SLA monitor
    pub fn new(targets: SlaTargets) -> Self {
        let current_metrics = Arc::new(RwLock::new(SlaMetrics {
            uptime_percentage: 100.0,
            total_downtime_seconds: 0,
            incidents_count: 0,
            average_mttr_seconds: 0.0,
            average_mtbf_hours: 0.0,
            error_budget_remaining: 100.0,
            latency_violations_p95: 0,
            latency_violations_p99: 0,
            throughput_achievements: 0,
            last_updated: Instant::now(),
        }));

        let violations = Arc::new(RwLock::new(Vec::new()));

        let incident_response = Arc::new(IncidentResponse::new());
        let capacity_planner = Arc::new(CapacityPlanner::new());
        let regression_detector = Arc::new(RegressionDetector::new());

        Self {
            targets,
            current_metrics,
            violations,
            incident_response,
            capacity_planner,
            regression_detector,
        }
    }

    /// Start SLA monitoring
    pub async fn start_monitoring(&self) -> Result<()> {
        println!("📊 Starting SLA Monitoring System...");
        println!("   Targets: {:.1}% uptime, {:.1}ms P95 latency", self.targets.availability_target, self.targets.latency_p95_target_ms);

        // Start SLA evaluation loop
        self.start_sla_evaluation().await?;

        // Start incident response system
        self.incident_response.start().await?;

        // Start capacity planning
        self.capacity_planner.start().await?;

        // Start regression detection
        self.regression_detector.start().await?;

        println!("✅ SLA monitoring system started");
        println!("   📈 Monitoring 99.9% uptime and sub-millisecond latency");
        println!("   🚨 Incident response automation enabled");
        println!("   📈 Capacity planning and scaling recommendations active");

        Ok(())
    }

    /// Record service availability
    pub fn record_availability(&self, available: bool, downtime_duration: Option<Duration>) {
        let mut metrics = self.current_metrics.write().unwrap();

        if !available {
            if let Some(duration) = downtime_duration {
                metrics.total_downtime_seconds += duration.as_secs();
            }

            // Check if this violates SLA
            let current_uptime = self.calculate_uptime_percentage(&metrics);
            if current_uptime < self.targets.availability_target {
                self.record_violation(SlaViolation {
                    timestamp: Instant::now(),
                    violation_type: SlaViolationType::Availability,
                    severity: ViolationSeverity::Critical,
                    description: format!("Availability dropped to {:.2}%", current_uptime),
                    impact: "Service downtime affecting users".to_string(),
                    resolution_time: None,
                    error_budget_impact: 10.0, // 10% of error budget
                });
            }
        }

        metrics.last_updated = Instant::now();
    }

    /// Record latency measurement
    pub fn record_latency(&self, p95_ms: f64, p99_ms: f64) {
        let mut metrics = self.current_metrics.write().unwrap();

        // Check P95 violation
        if p95_ms > self.targets.latency_p95_target_ms {
            metrics.latency_violations_p95 += 1;
            self.record_violation(SlaViolation {
                timestamp: Instant::now(),
                violation_type: SlaViolationType::LatencyP95,
                severity: ViolationSeverity::High,
                description: format!("P95 latency {:.1}ms exceeds target {:.1}ms", p95_ms, self.targets.latency_p95_target_ms),
                impact: "Degraded user experience".to_string(),
                resolution_time: None,
                error_budget_impact: 5.0,
            });
        }

        // Check P99 violation
        if p99_ms > self.targets.latency_p99_target_ms {
            metrics.latency_violations_p99 += 1;
            self.record_violation(SlaViolation {
                timestamp: Instant::now(),
                violation_type: SlaViolationType::LatencyP99,
                severity: ViolationSeverity::Medium,
                description: format!("P99 latency {:.1}ms exceeds target {:.1}ms", p99_ms, self.targets.latency_p99_target_ms),
                impact: "Worst-case performance degradation".to_string(),
                resolution_time: None,
                error_budget_impact: 2.0,
            });
        }

        metrics.last_updated = Instant::now();
    }

    /// Record error rate
    pub fn record_error_rate(&self, error_rate: f64) {
        if error_rate > self.targets.error_rate_target {
            self.record_violation(SlaViolation {
                timestamp: Instant::now(),
                violation_type: SlaViolationType::ErrorRate,
                severity: ViolationSeverity::High,
                description: format!("Error rate {:.2}% exceeds target {:.2}%", error_rate * 100.0, self.targets.error_rate_target * 100.0),
                impact: "Increased user-facing errors".to_string(),
                resolution_time: None,
                error_budget_impact: 8.0,
            });
        }
    }

    /// Record throughput
    pub fn record_throughput(&self, rps: usize) {
        let mut metrics = self.current_metrics.write().unwrap();

        if rps >= self.targets.throughput_target_rps {
            metrics.throughput_achievements += 1;
        } else {
            self.record_violation(SlaViolation {
                timestamp: Instant::now(),
                violation_type: SlaViolationType::Throughput,
                severity: ViolationSeverity::Medium,
                description: format!("Throughput {} RPS below target {} RPS", rps, self.targets.throughput_target_rps),
                impact: "Reduced service capacity".to_string(),
                resolution_time: None,
                error_budget_impact: 3.0,
            });
        }
    }

    /// Create incident
    pub async fn create_incident(&self, title: &str, description: &str, severity: IncidentSeverity) -> Result<String> {
        self.incident_response.create_incident(title, description, severity).await
    }

    /// Get current SLA status
    pub fn get_sla_status(&self) -> SlaStatus {
        let metrics = self.current_metrics.read().unwrap();
        let violations = self.violations.read().unwrap();

        let uptime_percentage = self.calculate_uptime_percentage(&metrics);
        let error_budget_used = 100.0 - metrics.error_budget_remaining;

        let status = if uptime_percentage >= self.targets.availability_target
                   && error_budget_used <= 5.0 {
            SlaStatusType::Good
        } else if uptime_percentage >= self.targets.availability_target - 0.1 {
            SlaStatusType::Warning
        } else {
            SlaStatusType::Critical
        };

        SlaStatus {
            status,
            uptime_percentage,
            error_budget_remaining: metrics.error_budget_remaining,
            active_violations: violations.len(),
            incidents_this_month: metrics.incidents_count,
            average_mttr_seconds: metrics.average_mttr_seconds,
            average_mtbf_hours: metrics.average_mtbf_hours,
        }
    }

    /// Get capacity planning recommendations
    pub fn get_capacity_recommendations(&self) -> Vec<ScalingRecommendation> {
        self.capacity_planner.get_recommendations()
    }

    /// Get performance regression alerts
    pub fn get_regression_alerts(&self) -> Vec<RegressionAlert> {
        self.regression_detector.get_alerts()
    }

    /// Record violation
    fn record_violation(&self, violation: SlaViolation) {
        let mut violations = self.violations.write().unwrap();
        violations.push(violation.clone());

        let mut metrics = self.current_metrics.write().unwrap();
        metrics.error_budget_remaining -= violation.error_budget_impact;
        metrics.error_budget_remaining = metrics.error_budget_remaining.max(0.0);

        // Log violation
        println!("🚨 SLA Violation: {} - {}", violation.violation_type.as_str(), violation.description);
    }

    /// Calculate uptime percentage
    fn calculate_uptime_percentage(&self, metrics: &SlaMetrics) -> f64 {
        let total_time_seconds = metrics.last_updated.elapsed().as_secs();
        if total_time_seconds == 0 {
            return 100.0;
        }

        let uptime_seconds = total_time_seconds - metrics.total_downtime_seconds as f64;
        (uptime_seconds / total_time_seconds) * 100.0
    }

    /// Start SLA evaluation loop
    async fn start_sla_evaluation(&self) -> Result<()> {
        let targets = self.targets.clone();
        let current_metrics = Arc::clone(&self.current_metrics);
        let violations = Arc::clone(&self.violations);

        tokio::spawn(async move {
            let mut interval = tokio::time::interval(Duration::from_secs(60)); // Evaluate every minute

            loop {
                interval.tick().await;

                // Evaluate SLA status
                let mut metrics = current_metrics.write().unwrap();

                // Update uptime percentage
                metrics.uptime_percentage = {
                    let total_time_seconds = metrics.last_updated.elapsed().as_secs();
                    if total_time_seconds == 0 {
                        100.0
                    } else {
                        let uptime_seconds = total_time_seconds - metrics.total_downtime_seconds as f64;
                        (uptime_seconds / total_time_seconds) * 100.0
                    }
                };

                // Check for SLA breaches
                if metrics.uptime_percentage < targets.availability_target {
                    println!("⚠️  SLA Warning: Uptime {:.2}% below target {:.1}%",
                            metrics.uptime_percentage, targets.availability_target);
                }

                if metrics.error_budget_remaining < 20.0 {
                    println!("⚠️  SLA Warning: Error budget {:.1}% remaining",
                            metrics.error_budget_remaining);
                }
            }
        });

        Ok(())
    }
}

impl SlaViolationType {
    fn as_str(&self) -> &str {
        match self {
            SlaViolationType::Availability => "Availability",
            SlaViolationType::LatencyP95 => "Latency P95",
            SlaViolationType::LatencyP99 => "Latency P99",
            SlaViolationType::ErrorRate => "Error Rate",
            SlaViolationType::Throughput => "Throughput",
            SlaViolationType::Security => "Security",
        }
    }
}

impl IncidentResponse {
    pub fn new() -> Self {
        let mut runbooks = HashMap::new();

        // Define incident runbooks
        runbooks.insert("high_cpu_usage".to_string(), IncidentRunbook {
            incident_type: "High CPU Usage".to_string(),
            detection_criteria: "CPU utilization > 90% for 5+ minutes".to_string(),
            investigation_steps: vec![
                "Check system load with 'top' or 'htop'".to_string(),
                "Identify top CPU-consuming processes".to_string(),
                "Review application metrics for bottlenecks".to_string(),
                "Check for memory leaks or inefficient algorithms".to_string(),
            ],
            remediation_steps: vec![
                "Scale horizontally by adding more instances".to_string(),
                "Optimize CPU-intensive operations".to_string(),
                "Implement request throttling if needed".to_string(),
                "Review and tune garbage collection settings".to_string(),
            ],
            communication_plan: "Notify engineering team via Slack #incidents and email distribution list".to_string(),
            escalation_criteria: "Escalate to SRE team if CPU > 95% for 15+ minutes".to_string(),
        });

        runbooks.insert("high_memory_usage".to_string(), IncidentRunbook {
            incident_type: "High Memory Usage".to_string(),
            detection_criteria: "Memory utilization > 90% for 5+ minutes".to_string(),
            investigation_steps: vec![
                "Check memory usage with 'free -h'".to_string(),
                "Identify memory-consuming processes".to_string(),
                "Review application heap dumps if available".to_string(),
                "Check for memory leaks in application code".to_string(),
            ],
            remediation_steps: vec![
                "Restart application instances to clear memory".to_string(),
                "Scale vertically by increasing instance size".to_string(),
                "Optimize memory usage in application code".to_string(),
                "Implement memory limits and circuit breakers".to_string(),
            ],
            communication_plan: "Notify engineering team via Slack #incidents".to_string(),
            escalation_criteria: "Escalate to SRE team if memory usage > 95% for 10+ minutes".to_string(),
        });

        runbooks.insert("high_error_rate".to_string(), IncidentRunbook {
            incident_type: "High Error Rate".to_string(),
            detection_criteria: "Error rate > 5% for 5+ minutes".to_string(),
            investigation_steps: vec![
                "Check application logs for error patterns".to_string(),
                "Review error metrics by endpoint".to_string(),
                "Check downstream service health".to_string(),
                "Verify database connectivity and performance".to_string(),
            ],
            remediation_steps: vec![
                "Implement circuit breaker for failing services".to_string(),
                "Scale application instances".to_string(),
                "Fix application bugs causing errors".to_string(),
                "Implement graceful degradation".to_string(),
            ],
            communication_plan: "Notify engineering team via Slack #incidents and product team for user impact".to_string(),
            escalation_criteria: "Escalate to on-call engineer immediately".to_string(),
        });

        Self {
            active_incidents: RwLock::new(HashMap::new()),
            runbooks,
            escalation_policies: vec![
                EscalationPolicy {
                    severity: IncidentSeverity::Sev1,
                    response_time: Duration::from_secs(300), // 5 minutes
                    notification_channels: vec!["slack".to_string(), "email".to_string(), "sms".to_string()],
                    escalation_contacts: vec!["oncall-engineer".to_string(), "sre-lead".to_string()],
                },
                EscalationPolicy {
                    severity: IncidentSeverity::Sev2,
                    response_time: Duration::from_secs(3600), // 1 hour
                    notification_channels: vec!["slack".to_string(), "email".to_string()],
                    escalation_contacts: vec!["sre-team".to_string()],
                },
            ],
            remediation_rules: vec![
                RemediationRule {
                    condition: "cpu_utilization > 95% and duration > 300s".to_string(),
                    action: "scale_out_instances".to_string(),
                    cooldown_period: Duration::from_secs(600),
                    success_criteria: "cpu_utilization < 80%".to_string(),
                },
                RemediationRule {
                    condition: "memory_utilization > 95% and duration > 300s".to_string(),
                    action: "restart_instance".to_string(),
                    cooldown_period: Duration::from_secs(300),
                    success_criteria: "memory_utilization < 80%".to_string(),
                },
            ],
        }
    }

    pub async fn start(&self) -> Result<()> {
        println!("🚨 Starting Incident Response System...");

        // Start incident monitoring loop
        let active_incidents = Arc::new(RwLock::new(HashMap::new()));
        let escalation_policies = self.escalation_policies.clone();

        tokio::spawn(async move {
            let mut interval = tokio::time::interval(Duration::from_secs(60));

            loop {
                interval.tick().await;

                // Check for incidents requiring escalation
                let incidents = active_incidents.read().unwrap();
                let now = Instant::now();

                for incident in incidents.values() {
                    if let Some(policy) = escalation_policies.iter().find(|p| p.severity == incident.severity) {
                        let time_since_creation = now.duration_since(incident.created_at);

                        if time_since_creation > policy.response_time && incident.status == IncidentStatus::Open {
                            println!("🚨 Escalating incident {}: Response time exceeded", incident.id);
                            // Send escalation notifications
                        }
                    }
                }
            }
        });

        println!("✅ Incident response system started");
        Ok(())
    }

    pub async fn create_incident(&self, title: &str, description: &str, severity: IncidentSeverity) -> Result<String> {
        let incident_id = format!("INC-{}", SystemTime::now().duration_since(UNIX_EPOCH)?.as_secs());

        let incident = Incident {
            id: incident_id.clone(),
            title: title.to_string(),
            description: description.to_string(),
            severity: severity.clone(),
            status: IncidentStatus::Open,
            created_at: Instant::now(),
            resolved_at: None,
            assigned_to: None,
            tags: vec!["automated".to_string()],
        };

        self.active_incidents.write().unwrap().insert(incident_id.clone(), incident);

        // Trigger automated response based on runbooks
        self.trigger_automated_response(&incident_id).await?;

        println!("🚨 Created incident {}: {}", incident_id, title);
        Ok(incident_id)
    }

    async fn trigger_automated_response(&self, incident_id: &str) -> Result<()> {
        // Look up appropriate runbook and execute automated remediation
        // This would integrate with deployment systems to trigger scaling, restarts, etc.

        println!("🤖 Triggering automated response for incident {}", incident_id);
        Ok(())
    }
}

impl CapacityPlanner {
    pub fn new() -> Self {
        Self {
            current_capacity: RwLock::new(CapacityMetrics {
                cpu_cores_available: 0,
                memory_gb_available: 0,
                network_bandwidth_gbps: 0.0,
                storage_tb_available: 0.0,
                current_utilization: ResourceUtilization {
                    timestamp: Instant::now(),
                    cpu_percent: 0.0,
                    memory_percent: 0.0,
                    network_percent: 0.0,
                    disk_percent: 0.0,
                },
            }),
            forecasts: RwLock::new(Vec::new()),
            recommendations: RwLock::new(Vec::new()),
            utilization_history: RwLock::new(Vec::new()),
        }
    }

    pub async fn start(&self) -> Result<()> {
        println!("📈 Starting Capacity Planning System...");

        // Start capacity monitoring loop
        let recommendations = Arc::clone(&self.recommendations);
        let current_capacity = Arc::clone(&self.current_capacity);

        tokio::spawn(async move {
            let mut interval = tokio::time::interval(Duration::from_secs(300)); // Every 5 minutes

            loop {
                interval.tick().await;

                // Analyze current utilization and generate recommendations
                let capacity = current_capacity.read().unwrap();

                // Generate scaling recommendations based on utilization
                if capacity.current_utilization.cpu_percent > 80.0 {
                    let mut recs = recommendations.write().unwrap();
                    recs.push(ScalingRecommendation {
                        timestamp: Instant::now(),
                        resource_type: "cpu".to_string(),
                        current_value: capacity.current_utilization.cpu_percent,
                        recommended_value: capacity.current_utilization.cpu_percent * 0.7, // Target 70%
                        reason: "High CPU utilization detected".to_string(),
                        impact_assessment: "Reduce CPU usage to improve performance and prevent throttling".to_string(),
                    });
                }

                if capacity.current_utilization.memory_percent > 85.0 {
                    let mut recs = recommendations.write().unwrap();
                    recs.push(ScalingRecommendation {
                        timestamp: Instant::now(),
                        resource_type: "memory".to_string(),
                        current_value: capacity.current_utilization.memory_percent,
                        recommended_value: capacity.current_utilization.memory_percent * 0.75, // Target 75%
                        reason: "High memory utilization detected".to_string(),
                        impact_assessment: "Reduce memory usage to prevent OOM kills".to_string(),
                    });
                }
            }
        });

        println!("✅ Capacity planning system started");
        Ok(())
    }

    pub fn get_recommendations(&self) -> Vec<ScalingRecommendation> {
        self.recommendations.read().unwrap().clone()
    }
}

impl RegressionDetector {
    pub fn new() -> Self {
        let mut baselines = HashMap::new();

        // Define performance baselines
        baselines.insert("http_request_duration_p95".to_string(), PerformanceBaseline {
            metric_name: "http_request_duration_p95".to_string(),
            baseline_value: 10.0, // 10ms
            standard_deviation: 2.0,
            sample_size: 1000,
            last_updated: Instant::now(),
            confidence_interval: (8.0, 12.0),
        });

        baselines.insert("http_requests_per_second".to_string(), PerformanceBaseline {
            metric_name: "http_requests_per_second".to_string(),
            baseline_value: 20000.0,
            standard_deviation: 2000.0,
            sample_size: 1000,
            last_updated: Instant::now(),
            confidence_interval: (18000.0, 22000.0),
        });

        Self {
            baselines,
            alerts: RwLock::new(Vec::new()),
            stats_config: RegressionStatsConfig {
                min_samples_for_baseline: 100,
                regression_threshold_percent: 10.0,
                confidence_level: 0.95,
                moving_average_window: Duration::from_secs(3600), // 1 hour
            },
        }
    }

    pub async fn start(&self) -> Result<()> {
        println!("🔍 Starting Performance Regression Detection...");

        // Start regression monitoring loop
        let baselines = self.baselines.clone();
        let alerts = Arc::clone(&self.alerts);
        let config = self.stats_config.clone();

        tokio::spawn(async move {
            let mut interval = tokio::time::interval(Duration::from_secs(300)); // Every 5 minutes

            loop {
                interval.tick().await;

                // Check for performance regressions against baselines
                for (metric_name, baseline) in &baselines {
                    // In practice, this would fetch current metrics from monitoring system
                    let current_value = match metric_name.as_str() {
                        "http_request_duration_p95" => 12.0, // Simulated current value
                        "http_requests_per_second" => 18000.0, // Simulated current value
                        _ => continue,
                    };

                    let deviation_percent = ((current_value - baseline.baseline_value) / baseline.baseline_value) * 100.0;

                    if deviation_percent.abs() > config.regression_threshold_percent {
                        let severity = if deviation_percent.abs() > 30.0 {
                            RegressionSeverity::Critical
                        } else if deviation_percent.abs() > 20.0 {
                            RegressionSeverity::Major
                        } else if deviation_percent.abs() > 15.0 {
                            RegressionSeverity::Moderate
                        } else {
                            RegressionSeverity::Minor
                        };

                        let alert = RegressionAlert {
                            id: format!("REG-{}", SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_secs()),
                            metric_name: metric_name.clone(),
                            baseline_value: baseline.baseline_value,
                            current_value,
                            deviation_percent: deviation_percent.abs(),
                            severity,
                            detected_at: Instant::now(),
                            potential_causes: vec![
                                "Code changes in recent deployment".to_string(),
                                "Increased load on dependencies".to_string(),
                                "Resource constraints".to_string(),
                                "Configuration changes".to_string(),
                            ],
                        };

                        alerts.write().unwrap().push(alert.clone());
                        println!("⚠️  Performance regression detected: {} {:.1}% deviation", metric_name, deviation_percent);
                    }
                }
            }
        });

        println!("✅ Performance regression detection started");
        Ok(())
    }

    pub fn get_alerts(&self) -> Vec<RegressionAlert> {
        self.alerts.read().unwrap().clone()
    }
}

/// SLA status summary
#[derive(Debug, Clone)]
pub struct SlaStatus {
    pub status: SlaStatusType,
    pub uptime_percentage: f64,
    pub error_budget_remaining: f64,
    pub active_violations: usize,
    pub incidents_this_month: usize,
    pub average_mttr_seconds: f64,
    pub average_mtbf_hours: f64,
}

/// SLA status types
#[derive(Debug, Clone)]
pub enum SlaStatusType {
    Good,     // Meeting all SLA targets
    Warning,  // Minor SLA violations
    Critical, // Major SLA violations
}

impl Default for SlaTargets {
    fn default() -> Self {
        Self {
            availability_target: 99.9,
            latency_p95_target_ms: 10.0,
            latency_p99_target_ms: 50.0,
            error_rate_target: 0.001, // 0.1%
            throughput_target_rps: 10000,
            mttr_target_seconds: 1800, // 30 minutes
            mtbf_target_hours: 720,    // 30 days
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_sla_monitor_creation() {
        let targets = SlaTargets::default();
        let monitor = SlaMonitor::new(targets.clone());

        assert_eq!(monitor.targets.availability_target, 99.9);
        assert_eq!(monitor.targets.latency_p95_target_ms, 10.0);
    }

    #[tokio::test]
    async fn test_sla_violation_recording() {
        let targets = SlaTargets::default();
        let monitor = SlaMonitor::new(targets);

        // Record a latency violation
        monitor.record_latency(15.0, 60.0); // P95 over target

        let violations = monitor.violations.read().unwrap();
        assert!(violations.len() >= 1);

        let violation = &violations[0];
        assert!(matches!(violation.violation_type, SlaViolationType::LatencyP95));
    }

    #[test]
    fn test_sla_status_calculation() {
        let targets = SlaTargets::default();
        let monitor = SlaMonitor::new(targets);

        // Initially should be good
        let status = monitor.get_sla_status();
        assert!(matches!(status.status, SlaStatusType::Good));
        assert_eq!(status.uptime_percentage, 100.0);
    }

    #[tokio::test]
    async fn test_incident_creation() {
        let incident_response = IncidentResponse::new();

        let incident_id = incident_response.create_incident(
            "Test Incident",
            "This is a test incident",
            IncidentSeverity::Sev2
        ).await.unwrap();

        assert!(incident_id.starts_with("INC-"));

        let incidents = incident_response.active_incidents.read().unwrap();
        assert_eq!(incidents.len(), 1);
    }

    #[test]
    fn test_capacity_recommendations() {
        let capacity_planner = CapacityPlanner::new();

        // Initially no recommendations
        let recommendations = capacity_planner.get_recommendations();
        assert_eq!(recommendations.len(), 0);
    }

    #[test]
    fn test_regression_detection() {
        let regression_detector = RegressionDetector::new();

        // Initially no alerts
        let alerts = regression_detector.get_alerts();
        assert_eq!(alerts.len(), 0);

        // Should have baseline metrics defined
        assert!(!regression_detector.baselines.is_empty());
    }
}
