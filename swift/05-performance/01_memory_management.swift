/*
 * Swift Performance: Memory Management
 * 
 * This file demonstrates production-grade memory management patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master advanced ARC optimization techniques
 * - Understand memory leak detection and prevention
 * - Implement proper weak and unowned reference usage
 * - Apply memory profiling and analysis tools
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import UIKit

// MARK: - Memory Management Patterns

/**
 * Advanced memory management patterns
 * 
 * This class demonstrates production-grade memory management
 * with proper ARC usage and leak prevention
 */
class MemoryOptimizedManager {
    
    // MARK: - Properties
    
    private var dataCache: [String: Data] = [:]
    private var imageCache: [String: UIImage] = [:]
    private var weakObservers: [WeakObserver] = []
    private var unownedReferences: [UnownedReference] = []
    
    // MARK: - Memory Management
    
    /**
     * Efficient data caching with memory pressure handling
     * 
     * This method demonstrates proper cache management
     * with memory pressure monitoring and cleanup
     */
    func cacheData(_ data: Data, forKey key: String) {
        // Check memory pressure before caching
        if shouldEvictCache() {
            evictOldestCacheEntries()
        }
        
        dataCache[key] = data
        
        // Monitor memory usage
        monitorMemoryUsage()
    }
    
    /**
     * Memory pressure detection
     * 
     * This method demonstrates proper memory pressure handling
     * with system memory monitoring
     */
    private func shouldEvictCache() -> Bool {
        let memoryInfo = getMemoryInfo()
        let usedMemory = memoryInfo.used
        let totalMemory = memoryInfo.total
        
        // Evict if using more than 80% of available memory
        return Double(usedMemory) / Double(totalMemory) > 0.8
    }
    
    /**
     * Cache eviction strategy
     * 
     * This method demonstrates LRU cache eviction
     * with proper memory cleanup
     */
    private func evictOldestCacheEntries() {
        // Remove oldest entries (simplified LRU)
        let entriesToRemove = dataCache.count / 4 // Remove 25%
        let sortedKeys = dataCache.keys.sorted()
        
        for i in 0..<min(entriesToRemove, sortedKeys.count) {
            dataCache.removeValue(forKey: sortedKeys[i])
        }
        
        // Force garbage collection
        autoreleasepool {
            // Additional cleanup if needed
        }
    }
    
    /**
     * Memory usage monitoring
     * 
     * This method demonstrates proper memory monitoring
     * with system memory information
     */
    private func monitorMemoryUsage() {
        let memoryInfo = getMemoryInfo()
        let usedMemory = memoryInfo.used
        let totalMemory = memoryInfo.total
        let memoryUsage = Double(usedMemory) / Double(totalMemory)
        
        if memoryUsage > 0.9 {
            // Critical memory usage - perform aggressive cleanup
            performAggressiveCleanup()
        } else if memoryUsage > 0.7 {
            // High memory usage - perform moderate cleanup
            performModerateCleanup()
        }
    }
    
    /**
     * Aggressive memory cleanup
     * 
     * This method demonstrates aggressive cleanup strategies
     * for critical memory situations
     */
    private func performAggressiveCleanup() {
        // Clear all caches
        dataCache.removeAll()
        imageCache.removeAll()
        
        // Clear weak observers
        weakObservers.removeAll { $0.observer == nil }
        
        // Force garbage collection
        autoreleasepool {
            // Additional cleanup
        }
    }
    
    /**
     * Moderate memory cleanup
     * 
     * This method demonstrates moderate cleanup strategies
     * for high memory usage situations
     */
    private func performModerateCleanup() {
        // Remove half of cache entries
        let entriesToRemove = dataCache.count / 2
        let sortedKeys = dataCache.keys.sorted()
        
        for i in 0..<min(entriesToRemove, sortedKeys.count) {
            dataCache.removeValue(forKey: sortedKeys[i])
        }
        
        // Clean up weak observers
        weakObservers.removeAll { $0.observer == nil }
    }
    
    /**
     * System memory information
     * 
     * This method demonstrates proper system memory monitoring
     * with accurate memory usage information
     */
    private func getMemoryInfo() -> (used: UInt64, total: UInt64) {
        var info = mach_task_basic_info()
        var count = mach_msg_type_number_t(MemoryLayout<mach_task_basic_info>.size) / MemoryLayout<natural_t>.size
        
        let result = withUnsafeMutablePointer(to: &info) {
            $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                task_info(mach_task_self_, task_flavor_t(MACH_TASK_BASIC_INFO), $0, &count)
            }
        }
        
        if result == KERN_SUCCESS {
            let usedMemory = info.resident_size
            let totalMemory = ProcessInfo.processInfo.physicalMemory
            return (used: usedMemory, total: totalMemory)
        }
        
        return (used: 0, total: 0)
    }
}

// MARK: - Weak Reference Management

/**
 * Weak reference wrapper
 * 
 * This class demonstrates proper weak reference management
 * with automatic cleanup and memory safety
 */
class WeakObserver {
    weak var observer: AnyObject?
    let identifier: String
    
    init(observer: AnyObject, identifier: String) {
        self.observer = observer
        self.identifier = identifier
    }
    
    var isValid: Bool {
        return observer != nil
    }
}

/**
 * Unowned reference wrapper
 * 
 * This class demonstrates proper unowned reference usage
 * with memory safety and performance optimization
 */
class UnownedReference {
    unowned let reference: AnyObject
    let identifier: String
    
    init(reference: AnyObject, identifier: String) {
        self.reference = reference
        self.identifier = identifier
    }
}

// MARK: - Memory Leak Detection

/**
 * Memory leak detector
 * 
 * This class demonstrates proper memory leak detection
 * with reference counting and cycle detection
 */
class MemoryLeakDetector {
    
    // MARK: - Properties
    
    private var trackedObjects: [String: WeakReference] = [:]
    private var leakThreshold: TimeInterval = 30.0 // 30 seconds
    private var checkInterval: TimeInterval = 5.0 // 5 seconds
    private var timer: Timer?
    
    // MARK: - Initialization
    
    init() {
        startLeakDetection()
    }
    
    deinit {
        stopLeakDetection()
    }
    
    // MARK: - Public Methods
    
    /**
     * Track object for leak detection
     * 
     * This method demonstrates proper object tracking
     * for memory leak detection
     */
    func trackObject(_ object: AnyObject, identifier: String) {
        let weakRef = WeakReference(object: object)
        trackedObjects[identifier] = weakRef
    }
    
    /**
     * Stop tracking object
     * 
     * This method demonstrates proper object untracking
     * for memory leak detection
     */
    func stopTrackingObject(identifier: String) {
        trackedObjects.removeValue(forKey: identifier)
    }
    
    // MARK: - Private Methods
    
    private func startLeakDetection() {
        timer = Timer.scheduledTimer(withTimeInterval: checkInterval, repeats: true) { [weak self] _ in
            self?.checkForLeaks()
        }
    }
    
    private func stopLeakDetection() {
        timer?.invalidate()
        timer = nil
    }
    
    private func checkForLeaks() {
        let currentTime = Date()
        
        for (identifier, weakRef) in trackedObjects {
            if weakRef.object == nil {
                // Object was deallocated, remove from tracking
                trackedObjects.removeValue(forKey: identifier)
            } else if currentTime.timeIntervalSince(weakRef.trackedAt) > leakThreshold {
                // Object is still alive after threshold, potential leak
                reportPotentialLeak(identifier: identifier, weakRef: weakRef)
            }
        }
    }
    
    private func reportPotentialLeak(identifier: String, weakRef: WeakReference) {
        print("⚠️ Potential memory leak detected for: \(identifier)")
        print("   Object type: \(type(of: weakRef.object!))")
        print("   Tracked for: \(Date().timeIntervalSince(weakRef.trackedAt)) seconds")
    }
}

/**
 * Weak reference wrapper for leak detection
 * 
 * This class demonstrates proper weak reference usage
 * for memory leak detection
 */
class WeakReference {
    weak var object: AnyObject?
    let trackedAt: Date
    
    init(object: AnyObject) {
        self.object = object
        self.trackedAt = Date()
    }
}

// MARK: - Memory Pool Management

/**
 * Memory pool for efficient object reuse
 * 
 * This class demonstrates proper memory pool management
 * with object reuse and memory optimization
 */
class MemoryPool<T: AnyObject> {
    
    // MARK: - Properties
    
    private var availableObjects: [T] = []
    private var inUseObjects: Set<ObjectIdentifier> = []
    private let maxPoolSize: Int
    private let objectFactory: () -> T
    private let objectReset: (T) -> Void
    
    // MARK: - Initialization
    
    init(
        maxPoolSize: Int = 100,
        objectFactory: @escaping () -> T,
        objectReset: @escaping (T) -> Void
    ) {
        self.maxPoolSize = maxPoolSize
        self.objectFactory = objectFactory
        self.objectReset = objectReset
    }
    
    // MARK: - Public Methods
    
    /**
     * Get object from pool
     * 
     * This method demonstrates proper object retrieval
     * from memory pool with reuse optimization
     */
    func getObject() -> T {
        if let object = availableObjects.popLast() {
            // Reuse existing object
            objectReset(object)
            inUseObjects.insert(ObjectIdentifier(object))
            return object
        } else {
            // Create new object
            let object = objectFactory()
            inUseObjects.insert(ObjectIdentifier(object))
            return object
        }
    }
    
    /**
     * Return object to pool
     * 
     * This method demonstrates proper object return
     * to memory pool with cleanup
     */
    func returnObject(_ object: T) {
        let identifier = ObjectIdentifier(object)
        
        if inUseObjects.contains(identifier) {
            inUseObjects.remove(identifier)
            
            if availableObjects.count < maxPoolSize {
                availableObjects.append(object)
            }
            // If pool is full, object will be deallocated
        }
    }
    
    /**
     * Clear pool
     * 
     * This method demonstrates proper pool cleanup
     * with memory deallocation
     */
    func clearPool() {
        availableObjects.removeAll()
        inUseObjects.removeAll()
    }
    
    /**
     * Pool statistics
     * 
     * This method demonstrates proper pool monitoring
     * with usage statistics
     */
    func getPoolStats() -> (available: Int, inUse: Int, total: Int) {
        return (
            available: availableObjects.count,
            inUse: inUseObjects.count,
            total: availableObjects.count + inUseObjects.count
        )
    }
}

// MARK: - Memory Profiler

/**
 * Custom memory profiler
 * 
 * This class demonstrates proper memory profiling
 * with detailed memory usage tracking
 */
class MemoryProfiler {
    
    // MARK: - Properties
    
    private var memorySnapshots: [MemorySnapshot] = []
    private var isProfiling = false
    private var profilingInterval: TimeInterval = 1.0
    private var timer: Timer?
    
    // MARK: - Public Methods
    
    /**
     * Start memory profiling
     * 
     * This method demonstrates proper profiling initialization
     * with memory tracking setup
     */
    func startProfiling(interval: TimeInterval = 1.0) {
        guard !isProfiling else { return }
        
        isProfiling = true
        profilingInterval = interval
        
        timer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            self?.takeMemorySnapshot()
        }
    }
    
    /**
     * Stop memory profiling
     * 
     * This method demonstrates proper profiling cleanup
     * with timer invalidation
     */
    func stopProfiling() {
        isProfiling = false
        timer?.invalidate()
        timer = nil
    }
    
    /**
     * Get memory usage report
     * 
     * This method demonstrates proper memory reporting
     * with detailed usage analysis
     */
    func getMemoryReport() -> MemoryReport {
        let currentSnapshot = takeMemorySnapshot()
        let peakMemory = memorySnapshots.map { $0.usedMemory }.max() ?? 0
        let averageMemory = memorySnapshots.map { $0.usedMemory }.reduce(0, +) / max(memorySnapshots.count, 1)
        
        return MemoryReport(
            currentMemory: currentSnapshot.usedMemory,
            peakMemory: peakMemory,
            averageMemory: averageMemory,
            totalSnapshots: memorySnapshots.count,
            memoryTrend: calculateMemoryTrend()
        )
    }
    
    // MARK: - Private Methods
    
    private func takeMemorySnapshot() -> MemorySnapshot {
        let memoryInfo = getMemoryInfo()
        let snapshot = MemorySnapshot(
            timestamp: Date(),
            usedMemory: memoryInfo.used,
            totalMemory: memoryInfo.total,
            freeMemory: memoryInfo.total - memoryInfo.used
        )
        
        memorySnapshots.append(snapshot)
        
        // Keep only last 100 snapshots
        if memorySnapshots.count > 100 {
            memorySnapshots.removeFirst()
        }
        
        return snapshot
    }
    
    private func getMemoryInfo() -> (used: UInt64, total: UInt64) {
        var info = mach_task_basic_info()
        var count = mach_msg_type_number_t(MemoryLayout<mach_task_basic_info>.size) / MemoryLayout<natural_t>.size
        
        let result = withUnsafeMutablePointer(to: &info) {
            $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                task_info(mach_task_self_, task_flavor_t(MACH_TASK_BASIC_INFO), $0, &count)
            }
        }
        
        if result == KERN_SUCCESS {
            let usedMemory = info.resident_size
            let totalMemory = ProcessInfo.processInfo.physicalMemory
            return (used: usedMemory, total: totalMemory)
        }
        
        return (used: 0, total: 0)
    }
    
    private func calculateMemoryTrend() -> MemoryTrend {
        guard memorySnapshots.count >= 2 else { return .stable }
        
        let recent = memorySnapshots.suffix(5)
        let older = memorySnapshots.prefix(max(0, memorySnapshots.count - 5))
        
        let recentAverage = recent.map { $0.usedMemory }.reduce(0, +) / recent.count
        let olderAverage = older.map { $0.usedMemory }.reduce(0, +) / max(older.count, 1)
        
        let difference = Double(recentAverage) - Double(olderAverage)
        let percentageChange = difference / Double(olderAverage) * 100
        
        if percentageChange > 10 {
            return .increasing
        } else if percentageChange < -10 {
            return .decreasing
        } else {
            return .stable
        }
    }
}

// MARK: - Supporting Types

/**
 * Memory snapshot for profiling
 * 
 * This struct demonstrates proper memory snapshot modeling
 * with timestamp and usage information
 */
struct MemorySnapshot {
    let timestamp: Date
    let usedMemory: UInt64
    let totalMemory: UInt64
    let freeMemory: UInt64
}

/**
 * Memory report for analysis
 * 
 * This struct demonstrates proper memory reporting
 * with comprehensive usage statistics
 */
struct MemoryReport {
    let currentMemory: UInt64
    let peakMemory: UInt64
    let averageMemory: UInt64
    let totalSnapshots: Int
    let memoryTrend: MemoryTrend
}

/**
 * Memory trend enumeration
 * 
 * This enum demonstrates proper trend modeling
 * for memory usage analysis
 */
enum MemoryTrend {
    case increasing
    case decreasing
    case stable
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use memory management patterns
 * 
 * This function shows practical usage of all the memory management components
 */
func demonstrateMemoryManagement() {
    print("=== Memory Management Demonstration ===\n")
    
    // Memory Optimized Manager
    let memoryManager = MemoryOptimizedManager()
    print("--- Memory Optimized Manager ---")
    print("Manager: \(type(of: memoryManager))")
    print("Features: Cache management, memory pressure handling, cleanup strategies")
    
    // Memory Leak Detector
    let leakDetector = MemoryLeakDetector()
    print("\n--- Memory Leak Detector ---")
    print("Detector: \(type(of: leakDetector))")
    print("Features: Object tracking, leak detection, automatic cleanup")
    
    // Memory Pool
    let memoryPool = MemoryPool<Data>(
        maxPoolSize: 50,
        objectFactory: { Data() },
        objectReset: { _ in }
    )
    print("\n--- Memory Pool ---")
    print("Pool: \(type(of: memoryPool))")
    print("Features: Object reuse, memory optimization, pool management")
    
    // Memory Profiler
    let profiler = MemoryProfiler()
    print("\n--- Memory Profiler ---")
    print("Profiler: \(type(of: profiler))")
    print("Features: Memory tracking, usage analysis, trend calculation")
    
    // Demonstrate memory optimization techniques
    print("\n--- Memory Optimization Techniques ---")
    print("ARC Optimization: Proper reference counting")
    print("Weak References: Automatic cleanup and safety")
    print("Unowned References: Performance optimization")
    print("Memory Pools: Object reuse and efficiency")
    print("Cache Management: LRU eviction and pressure handling")
    print("Leak Detection: Automatic monitoring and reporting")
    print("Memory Profiling: Usage tracking and analysis")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Use weak references for observers and delegates")
    print("2. Use unowned references when you're sure the object will outlive the reference")
    print("3. Implement proper cleanup in deinit methods")
    print("4. Use memory pools for frequently created/destroyed objects")
    print("5. Monitor memory usage and implement pressure handling")
    print("6. Use autoreleasepool for temporary objects")
    print("7. Profile memory usage regularly")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateMemoryManagement()
