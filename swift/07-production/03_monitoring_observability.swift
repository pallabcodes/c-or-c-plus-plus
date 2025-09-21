/*
 * Swift Production: Monitoring & Observability
 * 
 * This file demonstrates production-grade monitoring and observability practices in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master application monitoring and performance tracking
 * - Understand logging and log aggregation strategies
 * - Implement proper metrics collection and analysis
 * - Apply alerting and incident response procedures
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import os

// MARK: - Application Monitoring

/**
 * Application monitoring manager
 * 
 * This class demonstrates proper application monitoring
 * with comprehensive performance tracking and error monitoring
 */
class ApplicationMonitoringManager {
    
    // MARK: - Properties
    
    private var metrics: [String: Metric] = [:]
    private var events: [Event] = []
    private var alerts: [Alert] = []
    private let logger = Logger(subsystem: "com.app.monitoring", category: "monitoring")
    
    // MARK: - Public Methods
    
    /**
     * Track custom metric
     * 
     * This method demonstrates proper custom metric tracking
     * with comprehensive metric collection
     */
    func trackMetric(name: String, value: Double, tags: [String: String] = [:]) {
        let metric = Metric(
            name: name,
            value: value,
            tags: tags,
            timestamp: Date()
        )
        
        metrics[name] = metric
        logger.info("Metric tracked: \(name) = \(value)")
    }
    
    /**
     * Track event
     * 
     * This method demonstrates proper event tracking
     * with comprehensive event collection
     */
    func trackEvent(name: String, properties: [String: Any] = [:]) {
        let event = Event(
            name: name,
            properties: properties,
            timestamp: Date()
        )
        
        events.append(event)
        logger.info("Event tracked: \(name)")
    }
    
    /**
     * Track error
     * 
     * This method demonstrates proper error tracking
     * with comprehensive error collection and alerting
     */
    func trackError(error: Error, context: [String: Any] = [:]) {
        let errorEvent = Event(
            name: "error",
            properties: [
                "error": error.localizedDescription,
                "context": context
            ],
            timestamp: Date()
        )
        
        events.append(errorEvent)
        logger.error("Error tracked: \(error.localizedDescription)")
        
        // Check if error should trigger alert
        checkErrorThresholds()
    }
    
    /**
     * Get monitoring dashboard
     * 
     * This method demonstrates proper monitoring dashboard
     * with comprehensive metrics and status information
     */
    func getDashboard() -> MonitoringDashboard {
        let systemMetrics = getSystemMetrics()
        let applicationMetrics = getApplicationMetrics()
        let errorMetrics = getErrorMetrics()
        let performanceMetrics = getPerformanceMetrics()
        
        return MonitoringDashboard(
            systemMetrics: systemMetrics,
            applicationMetrics: applicationMetrics,
            errorMetrics: errorMetrics,
            performanceMetrics: performanceMetrics,
            alerts: alerts,
            status: calculateOverallStatus()
        )
    }
    
    // MARK: - Private Methods
    
    private func getSystemMetrics() -> SystemMetrics {
        return SystemMetrics(
            cpuUsage: getCPUUsage(),
            memoryUsage: getMemoryUsage(),
            diskUsage: getDiskUsage(),
            networkUsage: getNetworkUsage()
        )
    }
    
    private func getApplicationMetrics() -> ApplicationMetrics {
        return ApplicationMetrics(
            activeUsers: getActiveUsers(),
            requestRate: getRequestRate(),
            responseTime: getResponseTime(),
            errorRate: getErrorRate()
        )
    }
    
    private func getErrorMetrics() -> ErrorMetrics {
        let errorCount = events.filter { $0.name == "error" }.count
        let errorRate = Double(errorCount) / Double(events.count) * 100
        
        return ErrorMetrics(
            totalErrors: errorCount,
            errorRate: errorRate,
            recentErrors: getRecentErrors()
        )
    }
    
    private func getPerformanceMetrics() -> PerformanceMetrics {
        return PerformanceMetrics(
            averageResponseTime: getResponseTime(),
            p95ResponseTime: getP95ResponseTime(),
            throughput: getThroughput(),
            latency: getLatency()
        )
    }
    
    private func getCPUUsage() -> Double {
        return Double.random(in: 10...80)
    }
    
    private func getMemoryUsage() -> Double {
        return Double.random(in: 20...90)
    }
    
    private func getDiskUsage() -> Double {
        return Double.random(in: 30...95)
    }
    
    private func getNetworkUsage() -> Double {
        return Double.random(in: 5...60)
    }
    
    private func getActiveUsers() -> Int {
        return Int.random(in: 100...10000)
    }
    
    private func getRequestRate() -> Double {
        return Double.random(in: 100...10000)
    }
    
    private func getResponseTime() -> Double {
        return Double.random(in: 0.1...2.0)
    }
    
    private func getErrorRate() -> Double {
        return Double.random(in: 0...5)
    }
    
    private func getP95ResponseTime() -> Double {
        return Double.random(in: 0.5...5.0)
    }
    
    private func getThroughput() -> Double {
        return Double.random(in: 1000...50000)
    }
    
    private func getLatency() -> Double {
        return Double.random(in: 0.05...1.0)
    }
    
    private func getRecentErrors() -> [Event] {
        return events.filter { $0.name == "error" }.suffix(10)
    }
    
    private func checkErrorThresholds() {
        let errorRate = getErrorRate()
        if errorRate > 5.0 {
            createAlert(
                type: .error,
                message: "High error rate detected: \(errorRate)%",
                severity: .high
            )
        }
    }
    
    private func createAlert(type: AlertType, message: String, severity: AlertSeverity) {
        let alert = Alert(
            id: UUID().uuidString,
            type: type,
            message: message,
            severity: severity,
            timestamp: Date(),
            isActive: true
        )
        
        alerts.append(alert)
        logger.warning("Alert created: \(message)")
    }
    
    private func calculateOverallStatus() -> SystemStatus {
        let errorRate = getErrorRate()
        let cpuUsage = getCPUUsage()
        let memoryUsage = getMemoryUsage()
        
        if errorRate > 10 || cpuUsage > 90 || memoryUsage > 95 {
            return .critical
        } else if errorRate > 5 || cpuUsage > 80 || memoryUsage > 85 {
            return .warning
        } else {
            return .healthy
        }
    }
}

// MARK: - Logging Manager

/**
 * Logging manager
 * 
 * This class demonstrates proper logging management
 * with structured logging and log aggregation
 */
class LoggingManager {
    
    // MARK: - Properties
    
    private var logs: [LogEntry] = []
    private let logger = Logger(subsystem: "com.app.logging", category: "logging")
    private var logLevel: LogLevel = .info
    
    // MARK: - Public Methods
    
    /**
     * Log message
     * 
     * This method demonstrates proper message logging
     * with structured logging and level filtering
     */
    func log(level: LogLevel, message: String, context: [String: Any] = [:]) {
        guard level.rawValue >= logLevel.rawValue else { return }
        
        let logEntry = LogEntry(
            level: level,
            message: message,
            context: context,
            timestamp: Date(),
            thread: Thread.current.description
        )
        
        logs.append(logEntry)
        
        // Log to system logger
        switch level {
        case .debug:
            logger.debug("\(message)")
        case .info:
            logger.info("\(message)")
        case .warning:
            logger.warning("\(message)")
        case .error:
            logger.error("\(message)")
        case .critical:
            logger.critical("\(message)")
        }
    }
    
    /**
     * Get logs
     * 
     * This method demonstrates proper log retrieval
     * with filtering and pagination
     */
    func getLogs(
        level: LogLevel? = nil,
        startDate: Date? = nil,
        endDate: Date? = nil,
        limit: Int = 100
    ) -> [LogEntry] {
        var filteredLogs = logs
        
        if let level = level {
            filteredLogs = filteredLogs.filter { $0.level == level }
        }
        
        if let startDate = startDate {
            filteredLogs = filteredLogs.filter { $0.timestamp >= startDate }
        }
        
        if let endDate = endDate {
            filteredLogs = filteredLogs.filter { $0.timestamp <= endDate }
        }
        
        return Array(filteredLogs.suffix(limit))
    }
    
    /**
     * Get log metrics
     * 
     * This method demonstrates proper log metrics collection
     * with comprehensive log analysis
     */
    func getLogMetrics() -> LogMetrics {
        let totalLogs = logs.count
        let debugLogs = logs.filter { $0.level == .debug }.count
        let infoLogs = logs.filter { $0.level == .info }.count
        let warningLogs = logs.filter { $0.level == .warning }.count
        let errorLogs = logs.filter { $0.level == .error }.count
        let criticalLogs = logs.filter { $0.level == .critical }.count
        
        return LogMetrics(
            totalLogs: totalLogs,
            debugLogs: debugLogs,
            infoLogs: infoLogs,
            warningLogs: warningLogs,
            errorLogs: errorLogs,
            criticalLogs: criticalLogs,
            averageLogsPerMinute: calculateAverageLogsPerMinute()
        )
    }
    
    // MARK: - Private Methods
    
    private func calculateAverageLogsPerMinute() -> Double {
        guard !logs.isEmpty else { return 0 }
        
        let timeSpan = logs.last!.timestamp.timeIntervalSince(logs.first!.timestamp)
        let minutes = timeSpan / 60
        return Double(logs.count) / minutes
    }
}

// MARK: - Metrics Collector

/**
 * Metrics collector
 * 
 * This class demonstrates proper metrics collection
 * with comprehensive metrics gathering and analysis
 */
class MetricsCollector {
    
    // MARK: - Properties
    
    private var metrics: [String: Metric] = [:]
    private var counters: [String: Counter] = [:]
    private var gauges: [String: Gauge] = [:]
    private var histograms: [String: Histogram] = [:]
    
    // MARK: - Public Methods
    
    /**
     * Increment counter
     * 
     * This method demonstrates proper counter incrementation
     * with comprehensive counter tracking
     */
    func incrementCounter(name: String, value: Double = 1.0, tags: [String: String] = [:]) {
        if let existingCounter = counters[name] {
            existingCounter.increment(by: value)
        } else {
            let counter = Counter(name: name, value: value, tags: tags)
            counters[name] = counter
        }
    }
    
    /**
     * Set gauge value
     * 
     * This method demonstrates proper gauge value setting
     * with comprehensive gauge tracking
     */
    func setGauge(name: String, value: Double, tags: [String: String] = [:]) {
        if let existingGauge = gauges[name] {
            existingGauge.setValue(value)
        } else {
            let gauge = Gauge(name: name, value: value, tags: tags)
            gauges[name] = gauge
        }
    }
    
    /**
     * Record histogram value
     * 
     * This method demonstrates proper histogram value recording
     * with comprehensive histogram tracking
     */
    func recordHistogram(name: String, value: Double, tags: [String: String] = [:]) {
        if let existingHistogram = histograms[name] {
            existingHistogram.record(value)
        } else {
            let histogram = Histogram(name: name, tags: tags)
            histogram.record(value)
            histograms[name] = histogram
        }
    }
    
    /**
     * Get metrics summary
     * 
     * This method demonstrates proper metrics summary generation
     * with comprehensive metrics analysis
     */
    func getMetricsSummary() -> MetricsSummary {
        return MetricsSummary(
            totalMetrics: metrics.count,
            counters: counters.count,
            gauges: gauges.count,
            histograms: histograms.count,
            topCounters: getTopCounters(),
            topGauges: getTopGauges(),
            topHistograms: getTopHistograms()
        )
    }
    
    // MARK: - Private Methods
    
    private func getTopCounters() -> [Counter] {
        return Array(counters.values.sorted { $0.value > $1.value }.prefix(10))
    }
    
    private func getTopGauges() -> [Gauge] {
        return Array(gauges.values.sorted { $0.value > $1.value }.prefix(10))
    }
    
    private func getTopHistograms() -> [Histogram] {
        return Array(histograms.values.sorted { $0.count > $1.count }.prefix(10))
    }
}

// MARK: - Alerting System

/**
 * Alerting system
 * 
 * This class demonstrates proper alerting system
 * with comprehensive alert management and notification
 */
class AlertingSystem {
    
    // MARK: - Properties
    
    private var alerts: [Alert] = []
    private var alertRules: [AlertRule] = []
    private var notificationChannels: [NotificationChannel] = []
    
    // MARK: - Public Methods
    
    /**
     * Create alert rule
     * 
     * This method demonstrates proper alert rule creation
     * with comprehensive alert rule configuration
     */
    func createAlertRule(
        name: String,
        condition: AlertCondition,
        threshold: Double,
        severity: AlertSeverity,
        notificationChannels: [String]
    ) {
        let rule = AlertRule(
            name: name,
            condition: condition,
            threshold: threshold,
            severity: severity,
            notificationChannels: notificationChannels,
            isEnabled: true
        )
        
        alertRules.append(rule)
    }
    
    /**
     * Check alert conditions
     * 
     * This method demonstrates proper alert condition checking
     * with comprehensive alert evaluation
     */
    func checkAlertConditions(metrics: [String: Double]) {
        for rule in alertRules where rule.isEnabled {
            if let metricValue = metrics[rule.condition.metricName] {
                let shouldTrigger = evaluateCondition(
                    value: metricValue,
                    condition: rule.condition,
                    threshold: rule.threshold
                )
                
                if shouldTrigger {
                    triggerAlert(rule: rule, value: metricValue)
                }
            }
        }
    }
    
    /**
     * Get active alerts
     * 
     * This method demonstrates proper active alert retrieval
     * with comprehensive alert filtering
     */
    func getActiveAlerts() -> [Alert] {
        return alerts.filter { $0.isActive }
    }
    
    /**
     * Acknowledge alert
     * 
     * This method demonstrates proper alert acknowledgment
     * with comprehensive alert management
     */
    func acknowledgeAlert(alertId: String, acknowledgedBy: String) {
        if let index = alerts.firstIndex(where: { $0.id == alertId }) {
            alerts[index].acknowledgedBy = acknowledgedBy
            alerts[index].acknowledgedAt = Date()
            alerts[index].isActive = false
        }
    }
    
    // MARK: - Private Methods
    
    private func evaluateCondition(
        value: Double,
        condition: AlertCondition,
        threshold: Double
    ) -> Bool {
        switch condition.operator {
        case .greaterThan:
            return value > threshold
        case .lessThan:
            return value < threshold
        case .equalTo:
            return value == threshold
        case .notEqualTo:
            return value != threshold
        case .greaterThanOrEqualTo:
            return value >= threshold
        case .lessThanOrEqualTo:
            return value <= threshold
        }
    }
    
    private func triggerAlert(rule: AlertRule, value: Double) {
        let alert = Alert(
            id: UUID().uuidString,
            type: .metric,
            message: "\(rule.name): \(rule.condition.metricName) = \(value) (threshold: \(rule.threshold))",
            severity: rule.severity,
            timestamp: Date(),
            isActive: true
        )
        
        alerts.append(alert)
        
        // Send notifications
        for channelName in rule.notificationChannels {
            sendNotification(alert: alert, channel: channelName)
        }
    }
    
    private func sendNotification(alert: Alert, channel: String) {
        // Simulate notification sending
        // In production, you would send actual notifications
        print("Sending alert to \(channel): \(alert.message)")
    }
}

// MARK: - Supporting Types

/**
 * Metric
 * 
 * This struct demonstrates proper metric modeling
 * for application monitoring
 */
struct Metric {
    let name: String
    let value: Double
    let tags: [String: String]
    let timestamp: Date
}

/**
 * Event
 * 
 * This struct demonstrates proper event modeling
 * for application monitoring
 */
struct Event {
    let name: String
    let properties: [String: Any]
    let timestamp: Date
}

/**
 * Alert
 * 
 * This struct demonstrates proper alert modeling
 * for application monitoring
 */
struct Alert {
    let id: String
    let type: AlertType
    let message: String
    let severity: AlertSeverity
    let timestamp: Date
    var isActive: Bool
    var acknowledgedBy: String?
    var acknowledgedAt: Date?
}

/**
 * Alert type
 * 
 * This enum demonstrates proper alert type modeling
 * for application monitoring
 */
enum AlertType: String, CaseIterable {
    case metric = "metric"
    case error = "error"
    case performance = "performance"
    case security = "security"
}

/**
 * Alert severity
 * 
 * This enum demonstrates proper alert severity modeling
 * for application monitoring
 */
enum AlertSeverity: String, CaseIterable {
    case low = "low"
    case medium = "medium"
    case high = "high"
    case critical = "critical"
}

/**
 * System status
 * 
 * This enum demonstrates proper system status modeling
 * for application monitoring
 */
enum SystemStatus: String, CaseIterable {
    case healthy = "healthy"
    case warning = "warning"
    case critical = "critical"
}

/**
 * Log level
 * 
 * This enum demonstrates proper log level modeling
 * for logging management
 */
enum LogLevel: String, CaseIterable {
    case debug = "debug"
    case info = "info"
    case warning = "warning"
    case error = "error"
    case critical = "critical"
}

/**
 * Log entry
 * 
 * This struct demonstrates proper log entry modeling
 * for logging management
 */
struct LogEntry {
    let level: LogLevel
    let message: String
    let context: [String: Any]
    let timestamp: Date
    let thread: String
}

/**
 * Counter
 * 
 * This class demonstrates proper counter modeling
 * for metrics collection
 */
class Counter {
    let name: String
    var value: Double
    let tags: [String: String]
    
    init(name: String, value: Double, tags: [String: String]) {
        self.name = name
        self.value = value
        self.tags = tags
    }
    
    func increment(by amount: Double = 1.0) {
        value += amount
    }
}

/**
 * Gauge
 * 
 * This class demonstrates proper gauge modeling
 * for metrics collection
 */
class Gauge {
    let name: String
    var value: Double
    let tags: [String: String]
    
    init(name: String, value: Double, tags: [String: String]) {
        self.name = name
        self.value = value
        self.tags = tags
    }
    
    func setValue(_ newValue: Double) {
        value = newValue
    }
}

/**
 * Histogram
 * 
 * This class demonstrates proper histogram modeling
 * for metrics collection
 */
class Histogram {
    let name: String
    let tags: [String: String]
    private var values: [Double] = []
    
    init(name: String, tags: [String: String]) {
        self.name = name
        self.tags = tags
    }
    
    func record(_ value: Double) {
        values.append(value)
    }
    
    var count: Int {
        return values.count
    }
    
    var average: Double {
        guard !values.isEmpty else { return 0 }
        return values.reduce(0, +) / Double(values.count)
    }
    
    var min: Double {
        return values.min() ?? 0
    }
    
    var max: Double {
        return values.max() ?? 0
    }
}

/**
 * Monitoring dashboard
 * 
 * This struct demonstrates proper monitoring dashboard modeling
 * for application monitoring
 */
struct MonitoringDashboard {
    let systemMetrics: SystemMetrics
    let applicationMetrics: ApplicationMetrics
    let errorMetrics: ErrorMetrics
    let performanceMetrics: PerformanceMetrics
    let alerts: [Alert]
    let status: SystemStatus
}

/**
 * System metrics
 * 
 * This struct demonstrates proper system metrics modeling
 * for application monitoring
 */
struct SystemMetrics {
    let cpuUsage: Double
    let memoryUsage: Double
    let diskUsage: Double
    let networkUsage: Double
}

/**
 * Application metrics
 * 
 * This struct demonstrates proper application metrics modeling
 * for application monitoring
 */
struct ApplicationMetrics {
    let activeUsers: Int
    let requestRate: Double
    let responseTime: Double
    let errorRate: Double
}

/**
 * Error metrics
 * 
 * This struct demonstrates proper error metrics modeling
 * for application monitoring
 */
struct ErrorMetrics {
    let totalErrors: Int
    let errorRate: Double
    let recentErrors: [Event]
}

/**
 * Performance metrics
 * 
 * This struct demonstrates proper performance metrics modeling
 * for application monitoring
 */
struct PerformanceMetrics {
    let averageResponseTime: Double
    let p95ResponseTime: Double
    let throughput: Double
    let latency: Double
}

/**
 * Log metrics
 * 
 * This struct demonstrates proper log metrics modeling
 * for logging management
 */
struct LogMetrics {
    let totalLogs: Int
    let debugLogs: Int
    let infoLogs: Int
    let warningLogs: Int
    let errorLogs: Int
    let criticalLogs: Int
    let averageLogsPerMinute: Double
}

/**
 * Metrics summary
 * 
 * This struct demonstrates proper metrics summary modeling
 * for metrics collection
 */
struct MetricsSummary {
    let totalMetrics: Int
    let counters: Int
    let gauges: Int
    let histograms: Int
    let topCounters: [Counter]
    let topGauges: [Gauge]
    let topHistograms: [Histogram]
}

/**
 * Alert rule
 * 
 * This struct demonstrates proper alert rule modeling
 * for alerting system
 */
struct AlertRule {
    let name: String
    let condition: AlertCondition
    let threshold: Double
    let severity: AlertSeverity
    let notificationChannels: [String]
    var isEnabled: Bool
}

/**
 * Alert condition
 * 
 * This struct demonstrates proper alert condition modeling
 * for alerting system
 */
struct AlertCondition {
    let metricName: String
    let `operator`: AlertOperator
}

/**
 * Alert operator
 * 
 * This enum demonstrates proper alert operator modeling
 * for alerting system
 */
enum AlertOperator: String, CaseIterable {
    case greaterThan = ">"
    case lessThan = "<"
    case equalTo = "=="
    case notEqualTo = "!="
    case greaterThanOrEqualTo = ">="
    case lessThanOrEqualTo = "<="
}

/**
 * Notification channel
 * 
 * This struct demonstrates proper notification channel modeling
 * for alerting system
 */
struct NotificationChannel {
    let name: String
    let type: NotificationType
    let configuration: [String: String]
}

/**
 * Notification type
 * 
 * This enum demonstrates proper notification type modeling
 * for alerting system
 */
enum NotificationType: String, CaseIterable {
    case email = "email"
    case slack = "slack"
    case webhook = "webhook"
    case sms = "sms"
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use monitoring and observability
 * 
 * This function shows practical usage of all the monitoring components
 */
func demonstrateMonitoringObservability() {
    print("=== Monitoring & Observability Demonstration ===\n")
    
    // Application Monitoring Manager
    let monitoringManager = ApplicationMonitoringManager()
    print("--- Application Monitoring Manager ---")
    print("Monitoring Manager: \(type(of: monitoringManager))")
    print("Features: Metrics tracking, event tracking, error monitoring, dashboard")
    
    // Logging Manager
    let loggingManager = LoggingManager()
    print("\n--- Logging Manager ---")
    print("Logging Manager: \(type(of: loggingManager))")
    print("Features: Structured logging, log filtering, log metrics")
    
    // Metrics Collector
    let metricsCollector = MetricsCollector()
    print("\n--- Metrics Collector ---")
    print("Metrics Collector: \(type(of: metricsCollector))")
    print("Features: Counter tracking, gauge tracking, histogram recording")
    
    // Alerting System
    let alertingSystem = AlertingSystem()
    print("\n--- Alerting System ---")
    print("Alerting System: \(type(of: alertingSystem))")
    print("Features: Alert rules, condition checking, notification channels")
    
    // Demonstrate monitoring features
    print("\n--- Monitoring Features ---")
    print("Application Monitoring: Performance tracking, error monitoring, metrics collection")
    print("Logging: Structured logging, log aggregation, log analysis")
    print("Metrics: Custom metrics, counters, gauges, histograms")
    print("Alerting: Alert rules, condition checking, notification management")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Implement comprehensive application monitoring")
    print("2. Use structured logging with appropriate levels")
    print("3. Collect relevant metrics and KPIs")
    print("4. Set up proactive alerting and notification")
    print("5. Monitor system health and performance")
    print("6. Implement log aggregation and analysis")
    print("7. Continuously improve monitoring and observability")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateMonitoringObservability()
