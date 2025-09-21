/*
 * Swift Performance: Profiling Tools
 * 
 * This file demonstrates production-grade profiling tools and techniques in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master custom profilers and performance monitoring
 * - Understand metrics collection and analysis
 * - Implement proper performance testing strategies
 * - Apply automated performance validation
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import os

// MARK: - Performance Profiler

/**
 * Custom performance profiler
 * 
 * This class demonstrates production-grade performance profiling
 * with detailed metrics collection and analysis
 */
class PerformanceProfiler {
    
    // MARK: - Properties
    
    private var measurements: [PerformanceMeasurement] = []
    private var isProfiling = false
    private var profilingInterval: TimeInterval = 0.1
    private var timer: Timer?
    private let logger = Logger(subsystem: "com.performance.profiler", category: "profiler")
    
    // MARK: - Public Methods
    
    /**
     * Start performance profiling
     * 
     * This method demonstrates proper profiling initialization
     * with comprehensive metrics collection
     */
    func startProfiling(interval: TimeInterval = 0.1) {
        guard !isProfiling else { return }
        
        isProfiling = true
        profilingInterval = interval
        
        logger.info("Starting performance profiling with interval: \(interval)s")
        
        timer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            self?.takePerformanceSnapshot()
        }
    }
    
    /**
     * Stop performance profiling
     * 
     * This method demonstrates proper profiling cleanup
     * with resource management
     */
    func stopProfiling() {
        isProfiling = false
        timer?.invalidate()
        timer = nil
        
        logger.info("Stopped performance profiling")
    }
    
    /**
     * Get performance report
     * 
     * This method demonstrates proper performance reporting
     * with comprehensive analysis
     */
    func getPerformanceReport() -> PerformanceReport {
        let currentMeasurement = takePerformanceSnapshot()
        let averageCPU = measurements.map { $0.cpuUsage }.reduce(0, +) / max(measurements.count, 1)
        let peakCPU = measurements.map { $0.cpuUsage }.max() ?? 0
        let averageMemory = measurements.map { $0.memoryUsage }.reduce(0, +) / max(measurements.count, 1)
        let peakMemory = measurements.map { $0.memoryUsage }.max() ?? 0
        
        return PerformanceReport(
            currentCPU: currentMeasurement.cpuUsage,
            averageCPU: averageCPU,
            peakCPU: peakCPU,
            currentMemory: currentMeasurement.memoryUsage,
            averageMemory: averageMemory,
            peakMemory: peakMemory,
            totalMeasurements: measurements.count,
            performanceTrend: calculatePerformanceTrend()
        )
    }
    
    // MARK: - Private Methods
    
    private func takePerformanceSnapshot() -> PerformanceMeasurement {
        let cpuUsage = getCPUUsage()
        let memoryUsage = getMemoryUsage()
        let timestamp = Date()
        
        let measurement = PerformanceMeasurement(
            timestamp: timestamp,
            cpuUsage: cpuUsage,
            memoryUsage: memoryUsage
        )
        
        measurements.append(measurement)
        
        // Keep only last 1000 measurements
        if measurements.count > 1000 {
            measurements.removeFirst()
        }
        
        return measurement
    }
    
    private func getCPUUsage() -> Double {
        var info = mach_task_basic_info()
        var count = mach_msg_type_number_t(MemoryLayout<mach_task_basic_info>.size) / MemoryLayout<natural_t>.size
        
        let result = withUnsafeMutablePointer(to: &info) {
            $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                task_info(mach_task_self_, task_flavor_t(MACH_TASK_BASIC_INFO), $0, &count)
            }
        }
        
        if result == KERN_SUCCESS {
            let userTime = Double(info.user_time.seconds) + Double(info.user_time.microseconds) / 1_000_000.0
            let systemTime = Double(info.system_time.seconds) + Double(info.system_time.microseconds) / 1_000_000.0
            let totalTime = userTime + systemTime
            
            return totalTime
        }
        
        return 0.0
    }
    
    private func getMemoryUsage() -> UInt64 {
        var info = mach_task_basic_info()
        var count = mach_msg_type_number_t(MemoryLayout<mach_task_basic_info>.size) / MemoryLayout<natural_t>.size
        
        let result = withUnsafeMutablePointer(to: &info) {
            $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                task_info(mach_task_self_, task_flavor_t(MACH_TASK_BASIC_INFO), $0, &count)
            }
        }
        
        if result == KERN_SUCCESS {
            return info.resident_size
        }
        
        return 0
    }
    
    private func calculatePerformanceTrend() -> PerformanceTrend {
        guard measurements.count >= 2 else { return .stable }
        
        let recent = measurements.suffix(10)
        let older = measurements.prefix(max(0, measurements.count - 10))
        
        let recentCPUAverage = recent.map { $0.cpuUsage }.reduce(0, +) / recent.count
        let olderCPUAverage = older.map { $0.cpuUsage }.reduce(0, +) / max(older.count, 1)
        
        let recentMemoryAverage = recent.map { $0.memoryUsage }.reduce(0, +) / recent.count
        let olderMemoryAverage = older.map { $0.memoryUsage }.reduce(0, +) / max(older.count, 1)
        
        let cpuDifference = recentCPUAverage - olderCPUAverage
        let memoryDifference = Double(recentMemoryAverage) - Double(olderMemoryAverage)
        
        if cpuDifference > 0.1 || memoryDifference > 0.1 {
            return .degrading
        } else if cpuDifference < -0.1 || memoryDifference < -0.1 {
            return .improving
        } else {
            return .stable
        }
    }
}

// MARK: - Custom Metrics Collector

/**
 * Custom metrics collector
 * 
 * This class demonstrates production-grade metrics collection
 * with proper data aggregation and analysis
 */
class MetricsCollector {
    
    // MARK: - Properties
    
    private var metrics: [String: MetricData] = [:]
    private let lock = NSLock()
    private let logger = Logger(subsystem: "com.metrics.collector", category: "metrics")
    
    // MARK: - Public Methods
    
    /**
     * Record metric value
     * 
     * This method demonstrates proper metric recording
     * with data aggregation and analysis
     */
    func recordMetric(_ name: String, value: Double, timestamp: Date = Date()) {
        lock.lock()
        defer { lock.unlock() }
        
        if var existingMetric = metrics[name] {
            existingMetric.addValue(value, timestamp: timestamp)
            metrics[name] = existingMetric
        } else {
            let newMetric = MetricData(name: name)
            newMetric.addValue(value, timestamp: timestamp)
            metrics[name] = newMetric
        }
        
        logger.debug("Recorded metric: \(name) = \(value)")
    }
    
    /**
     * Get metric summary
     * 
     * This method demonstrates proper metric summarization
     * with statistical analysis
     */
    func getMetricSummary(_ name: String) -> MetricSummary? {
        lock.lock()
        defer { lock.unlock() }
        
        guard let metric = metrics[name] else { return nil }
        
        return MetricSummary(
            name: name,
            count: metric.values.count,
            average: metric.values.reduce(0, +) / Double(metric.values.count),
            min: metric.values.min() ?? 0,
            max: metric.values.max() ?? 0,
            latest: metric.values.last ?? 0,
            trend: calculateMetricTrend(metric)
        )
    }
    
    /**
     * Get all metrics summary
     * 
     * This method demonstrates proper metrics aggregation
     * with comprehensive analysis
     */
    func getAllMetricsSummary() -> [MetricSummary] {
        lock.lock()
        defer { lock.unlock() }
        
        return metrics.values.map { metric in
            MetricSummary(
                name: metric.name,
                count: metric.values.count,
                average: metric.values.reduce(0, +) / Double(metric.values.count),
                min: metric.values.min() ?? 0,
                max: metric.values.max() ?? 0,
                latest: metric.values.last ?? 0,
                trend: calculateMetricTrend(metric)
            )
        }
    }
    
    // MARK: - Private Methods
    
    private func calculateMetricTrend(_ metric: MetricData) -> MetricTrend {
        guard metric.values.count >= 2 else { return .stable }
        
        let recent = Array(metric.values.suffix(5))
        let older = Array(metric.values.prefix(max(0, metric.values.count - 5)))
        
        let recentAverage = recent.reduce(0, +) / Double(recent.count)
        let olderAverage = older.reduce(0, +) / Double(max(older.count, 1))
        
        let difference = recentAverage - olderAverage
        let percentageChange = difference / olderAverage * 100
        
        if percentageChange > 10 {
            return .increasing
        } else if percentageChange < -10 {
            return .decreasing
        } else {
            return .stable
        }
    }
}

// MARK: - Performance Testing

/**
 * Performance testing framework
 * 
 * This class demonstrates production-grade performance testing
 * with automated validation and benchmarking
 */
class PerformanceTestingFramework {
    
    // MARK: - Properties
    
    private let profiler: PerformanceProfiler
    private let metricsCollector: MetricsCollector
    private var testResults: [TestResult] = []
    
    // MARK: - Initialization
    
    init() {
        self.profiler = PerformanceProfiler()
        self.metricsCollector = MetricsCollector()
    }
    
    // MARK: - Public Methods
    
    /**
     * Run performance test
     * 
     * This method demonstrates proper performance testing
     * with automated validation and benchmarking
     */
    func runPerformanceTest<T>(
        name: String,
        iterations: Int = 100,
        test: @escaping () throws -> T
    ) -> TestResult {
        let startTime = Date()
        var results: [T] = []
        var errors: [Error] = []
        
        // Start profiling
        profiler.startProfiling(interval: 0.01)
        
        // Run test iterations
        for i in 0..<iterations {
            do {
                let result = try test()
                results.append(result)
                
                // Record metrics
                metricsCollector.recordMetric("\(name)_success", value: 1.0)
            } catch {
                errors.append(error)
                metricsCollector.recordMetric("\(name)_error", value: 1.0)
            }
        }
        
        // Stop profiling
        profiler.stopProfiling()
        
        let endTime = Date()
        let duration = endTime.timeIntervalSince(startTime)
        
        // Calculate performance metrics
        let successRate = Double(results.count) / Double(iterations) * 100
        let averageTime = duration / Double(iterations)
        let throughput = Double(iterations) / duration
        
        // Record performance metrics
        metricsCollector.recordMetric("\(name)_duration", value: duration)
        metricsCollector.recordMetric("\(name)_average_time", value: averageTime)
        metricsCollector.recordMetric("\(name)_throughput", value: throughput)
        metricsCollector.recordMetric("\(name)_success_rate", value: successRate)
        
        // Create test result
        let testResult = TestResult(
            name: name,
            iterations: iterations,
            successCount: results.count,
            errorCount: errors.count,
            successRate: successRate,
            totalDuration: duration,
            averageTime: averageTime,
            throughput: throughput,
            errors: errors
        )
        
        testResults.append(testResult)
        return testResult
    }
    
    /**
     * Run benchmark test
     * 
     * This method demonstrates proper benchmark testing
     * with performance comparison and analysis
     */
    func runBenchmarkTest<T>(
        name: String,
        implementations: [(String, () throws -> T)],
        iterations: Int = 100
    ) -> BenchmarkResult {
        var implementationResults: [String: TestResult] = [:]
        
        for (implName, implementation) in implementations {
            let testResult = runPerformanceTest(
                name: "\(name)_\(implName)",
                iterations: iterations,
                test: implementation
            )
            implementationResults[implName] = testResult
        }
        
        return BenchmarkResult(
            name: name,
            implementations: implementationResults,
            iterations: iterations
        )
    }
    
    /**
     * Get test results summary
     * 
     * This method demonstrates proper test results summarization
     * with comprehensive analysis
     */
    func getTestResultsSummary() -> TestResultsSummary {
        let totalTests = testResults.count
        let totalIterations = testResults.map { $0.iterations }.reduce(0, +)
        let totalSuccesses = testResults.map { $0.successCount }.reduce(0, +)
        let totalErrors = testResults.map { $0.errorCount }.reduce(0, +)
        let averageSuccessRate = testResults.map { $0.successRate }.reduce(0, +) / Double(max(testResults.count, 1))
        let averageThroughput = testResults.map { $0.throughput }.reduce(0, +) / Double(max(testResults.count, 1))
        
        return TestResultsSummary(
            totalTests: totalTests,
            totalIterations: totalIterations,
            totalSuccesses: totalSuccesses,
            totalErrors: totalErrors,
            averageSuccessRate: averageSuccessRate,
            averageThroughput: averageThroughput,
            testResults: testResults
        )
    }
}

// MARK: - Supporting Types

/**
 * Performance measurement
 * 
 * This struct demonstrates proper performance measurement modeling
 * with timestamp and usage information
 */
struct PerformanceMeasurement {
    let timestamp: Date
    let cpuUsage: Double
    let memoryUsage: UInt64
}

/**
 * Performance report
 * 
 * This struct demonstrates proper performance reporting
 * with comprehensive usage statistics
 */
struct PerformanceReport {
    let currentCPU: Double
    let averageCPU: Double
    let peakCPU: Double
    let currentMemory: UInt64
    let averageMemory: UInt64
    let peakMemory: UInt64
    let totalMeasurements: Int
    let performanceTrend: PerformanceTrend
}

/**
 * Performance trend enumeration
 * 
 * This enum demonstrates proper trend modeling
 * for performance analysis
 */
enum PerformanceTrend {
    case improving
    case stable
    case degrading
}

/**
 * Metric data
 * 
 * This class demonstrates proper metric data modeling
 * for metrics collection and analysis
 */
class MetricData {
    let name: String
    var values: [Double] = []
    var timestamps: [Date] = []
    
    init(name: String) {
        self.name = name
    }
    
    func addValue(_ value: Double, timestamp: Date) {
        values.append(value)
        timestamps.append(timestamp)
        
        // Keep only last 1000 values
        if values.count > 1000 {
            values.removeFirst()
            timestamps.removeFirst()
        }
    }
}

/**
 * Metric summary
 * 
 * This struct demonstrates proper metric summarization
 * with statistical analysis
 */
struct MetricSummary {
    let name: String
    let count: Int
    let average: Double
    let min: Double
    let max: Double
    let latest: Double
    let trend: MetricTrend
}

/**
 * Metric trend enumeration
 * 
 * This enum demonstrates proper trend modeling
 * for metric analysis
 */
enum MetricTrend {
    case increasing
    case stable
    case decreasing
}

/**
 * Test result
 * 
 * This struct demonstrates proper test result modeling
 * for performance testing
 */
struct TestResult {
    let name: String
    let iterations: Int
    let successCount: Int
    let errorCount: Int
    let successRate: Double
    let totalDuration: TimeInterval
    let averageTime: TimeInterval
    let throughput: Double
    let errors: [Error]
}

/**
 * Benchmark result
 * 
 * This struct demonstrates proper benchmark result modeling
 * for performance comparison
 */
struct BenchmarkResult {
    let name: String
    let implementations: [String: TestResult]
    let iterations: Int
}

/**
 * Test results summary
 * 
 * This struct demonstrates proper test results summarization
 * with comprehensive analysis
 */
struct TestResultsSummary {
    let totalTests: Int
    let totalIterations: Int
    let totalSuccesses: Int
    let totalErrors: Int
    let averageSuccessRate: Double
    let averageThroughput: Double
    let testResults: [TestResult]
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use profiling tools
 * 
 * This function shows practical usage of all the profiling components
 */
func demonstrateProfilingTools() {
    print("=== Profiling Tools Demonstration ===\n")
    
    // Performance Profiler
    let profiler = PerformanceProfiler()
    print("--- Performance Profiler ---")
    print("Profiler: \(type(of: profiler))")
    print("Features: CPU monitoring, memory tracking, trend analysis")
    
    // Metrics Collector
    let metricsCollector = MetricsCollector()
    print("\n--- Metrics Collector ---")
    print("Collector: \(type(of: metricsCollector))")
    print("Features: Metric recording, data aggregation, statistical analysis")
    
    // Performance Testing Framework
    let testingFramework = PerformanceTestingFramework()
    print("\n--- Performance Testing Framework ---")
    print("Framework: \(type(of: testingFramework))")
    print("Features: Automated testing, benchmarking, validation")
    
    // Demonstrate profiling techniques
    print("\n--- Profiling Techniques ---")
    print("Performance Profiling: Real-time monitoring, trend analysis")
    print("Metrics Collection: Data aggregation, statistical analysis")
    print("Performance Testing: Automated validation, benchmarking")
    print("Custom Profilers: Tailored monitoring, specific metrics")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Profile before optimizing")
    print("2. Use appropriate profiling tools")
    print("3. Collect relevant metrics")
    print("4. Automate performance testing")
    print("5. Monitor performance trends")
    print("6. Set performance baselines")
    print("7. Validate optimizations")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateProfilingTools()
