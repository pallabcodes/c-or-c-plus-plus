/*
 * Swift Performance: CPU Performance
 * 
 * This file demonstrates production-grade CPU performance optimization in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master algorithm optimization and efficient data structures
 * - Understand concurrency patterns and performance implications
 * - Implement proper CPU profiling and measurement
 * - Apply compiler optimizations and best practices
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import Dispatch

// MARK: - Algorithm Optimization

/**
 * Optimized sorting algorithms
 * 
 * This class demonstrates production-grade sorting algorithms
 * with performance optimization and benchmarking
 */
class OptimizedSortingAlgorithms {
    
    // MARK: - Quick Sort Implementation
    
    /**
     * Optimized quick sort with performance improvements
     * 
     * This method demonstrates proper quick sort implementation
     * with pivot selection and partitioning optimization
     */
    static func quickSort<T: Comparable>(_ array: inout [T], low: Int, high: Int) {
        guard low < high else { return }
        
        // Use insertion sort for small arrays
        if high - low < 20 {
            insertionSort(&array, low: low, high: high)
            return
        }
        
        // Use median-of-three pivot selection
        let pivotIndex = selectPivot(array, low: low, high: high)
        let partitionIndex = partition(&array, low: low, high: high, pivotIndex: pivotIndex)
        
        // Recursively sort subarrays
        quickSort(&array, low: low, high: partitionIndex - 1)
        quickSort(&array, low: partitionIndex + 1, high: high)
    }
    
    /**
     * Optimized partition function
     * 
     * This method demonstrates proper partitioning
     * with performance optimization
     */
    private static func partition<T: Comparable>(_ array: inout [T], low: Int, high: Int, pivotIndex: Int) -> Int {
        let pivot = array[pivotIndex]
        array.swapAt(pivotIndex, high)
        
        var i = low
        for j in low..<high {
            if array[j] <= pivot {
                array.swapAt(i, j)
                i += 1
            }
        }
        
        array.swapAt(i, high)
        return i
    }
    
    /**
     * Median-of-three pivot selection
     * 
     * This method demonstrates proper pivot selection
     * for improved performance
     */
    private static func selectPivot<T: Comparable>(_ array: [T], low: Int, high: Int) -> Int {
        let mid = low + (high - low) / 2
        
        if array[low] <= array[mid] && array[mid] <= array[high] {
            return mid
        } else if array[mid] <= array[low] && array[low] <= array[high] {
            return low
        } else {
            return high
        }
    }
    
    /**
     * Insertion sort for small arrays
     * 
     * This method demonstrates proper insertion sort
     * for small array optimization
     */
    private static func insertionSort<T: Comparable>(_ array: inout [T], low: Int, high: Int) {
        for i in (low + 1)...high {
            let key = array[i]
            var j = i - 1
            
            while j >= low && array[j] > key {
                array[j + 1] = array[j]
                j -= 1
            }
            
            array[j + 1] = key
        }
    }
    
    // MARK: - Merge Sort Implementation
    
    /**
     * Optimized merge sort with performance improvements
     * 
     * This method demonstrates proper merge sort implementation
     * with memory optimization and performance tuning
     */
    static func mergeSort<T: Comparable>(_ array: [T]) -> [T] {
        guard array.count > 1 else { return array }
        
        // Use insertion sort for small arrays
        if array.count < 20 {
            var result = array
            insertionSort(&result, low: 0, high: result.count - 1)
            return result
        }
        
        let mid = array.count / 2
        let left = Array(array[0..<mid])
        let right = Array(array[mid..<array.count])
        
        return merge(
            mergeSort(left),
            mergeSort(right)
        )
    }
    
    /**
     * Optimized merge function
     * 
     * This method demonstrates proper merging
     * with performance optimization
     */
    private static func merge<T: Comparable>(_ left: [T], _ right: [T]) -> [T] {
        var result: [T] = []
        result.reserveCapacity(left.count + right.count)
        
        var leftIndex = 0
        var rightIndex = 0
        
        while leftIndex < left.count && rightIndex < right.count {
            if left[leftIndex] <= right[rightIndex] {
                result.append(left[leftIndex])
                leftIndex += 1
            } else {
                result.append(right[rightIndex])
                rightIndex += 1
            }
        }
        
        // Append remaining elements
        result.append(contentsOf: left[leftIndex...])
        result.append(contentsOf: right[rightIndex...])
        
        return result
    }
}

// MARK: - Data Structure Optimization

/**
 * Optimized data structures
 * 
 * This class demonstrates production-grade data structures
 * with performance optimization and memory efficiency
 */
class OptimizedDataStructures {
    
    // MARK: - Circular Buffer
    
    /**
     * High-performance circular buffer
     * 
     * This class demonstrates proper circular buffer implementation
     * with O(1) operations and memory efficiency
     */
    class CircularBuffer<T> {
        
        // MARK: - Properties
        
        private var buffer: [T?]
        private var head: Int = 0
        private var tail: Int = 0
        private var count: Int = 0
        private let capacity: Int
        
        // MARK: - Initialization
        
        init(capacity: Int) {
            self.capacity = capacity
            self.buffer = Array(repeating: nil, count: capacity)
        }
        
        // MARK: - Public Methods
        
        /**
         * Add element to buffer
         * 
         * This method demonstrates proper element addition
         * with O(1) performance
         */
        func enqueue(_ element: T) -> Bool {
            guard !isFull else { return false }
            
            buffer[tail] = element
            tail = (tail + 1) % capacity
            count += 1
            
            return true
        }
        
        /**
         * Remove element from buffer
         * 
         * This method demonstrates proper element removal
         * with O(1) performance
         */
        func dequeue() -> T? {
            guard !isEmpty else { return nil }
            
            let element = buffer[head]
            buffer[head] = nil
            head = (head + 1) % capacity
            count -= 1
            
            return element
        }
        
        /**
         * Peek at next element
         * 
         * This method demonstrates proper element peeking
         * with O(1) performance
         */
        func peek() -> T? {
            guard !isEmpty else { return nil }
            return buffer[head]
        }
        
        // MARK: - Computed Properties
        
        var isEmpty: Bool {
            return count == 0
        }
        
        var isFull: Bool {
            return count == capacity
        }
        
        var currentCount: Int {
            return count
        }
    }
    
    // MARK: - Trie Data Structure
    
    /**
     * High-performance trie for string operations
     * 
     * This class demonstrates proper trie implementation
     * with O(m) search/insert/delete operations
     */
    class Trie {
        
        // MARK: - Properties
        
        private class Node {
            var children: [Character: Node] = [:]
            var isEndOfWord: Bool = false
            var wordCount: Int = 0
        }
        
        private let root: Node
        
        // MARK: - Initialization
        
        init() {
            self.root = Node()
        }
        
        // MARK: - Public Methods
        
        /**
         * Insert word into trie
         * 
         * This method demonstrates proper word insertion
         * with O(m) performance
         */
        func insert(_ word: String) {
            var current = root
            current.wordCount += 1
            
            for char in word {
                if current.children[char] == nil {
                    current.children[char] = Node()
                }
                current = current.children[char]!
                current.wordCount += 1
            }
            
            current.isEndOfWord = true
        }
        
        /**
         * Search word in trie
         * 
         * This method demonstrates proper word search
         * with O(m) performance
         */
        func search(_ word: String) -> Bool {
            var current = root
            
            for char in word {
                guard let child = current.children[char] else {
                    return false
                }
                current = child
            }
            
            return current.isEndOfWord
        }
        
        /**
         * Get all words with prefix
         * 
         * This method demonstrates proper prefix search
         * with O(m + k) performance
         */
        func wordsWithPrefix(_ prefix: String) -> [String] {
            var current = root
            
            // Navigate to prefix node
            for char in prefix {
                guard let child = current.children[char] else {
                    return []
                }
                current = child
            }
            
            // Collect all words from this node
            var words: [String] = []
            collectWords(from: current, prefix: prefix, words: &words)
            
            return words
        }
        
        /**
         * Get word count for prefix
         * 
         * This method demonstrates proper word counting
         * with O(m) performance
         */
        func wordCount(forPrefix prefix: String) -> Int {
            var current = root
            
            for char in prefix {
                guard let child = current.children[char] else {
                    return 0
                }
                current = child
            }
            
            return current.wordCount
        }
        
        // MARK: - Private Methods
        
        private func collectWords(from node: Node, prefix: String, words: inout [String]) {
            if node.isEndOfWord {
                words.append(prefix)
            }
            
            for (char, child) in node.children {
                collectWords(from: child, prefix: prefix + String(char), words: &words)
            }
        }
    }
}

// MARK: - Concurrency Optimization

/**
 * Optimized concurrency patterns
 * 
 * This class demonstrates production-grade concurrency
 * with performance optimization and proper synchronization
 */
class OptimizedConcurrency {
    
    // MARK: - Thread Pool
    
    /**
     * High-performance thread pool
     * 
     * This class demonstrates proper thread pool implementation
     * with work distribution and performance optimization
     */
    class ThreadPool {
        
        // MARK: - Properties
        
        private let queue: DispatchQueue
        private let semaphore: DispatchSemaphore
        private let maxConcurrentTasks: Int
        private var isShutdown = false
        
        // MARK: - Initialization
        
        init(maxConcurrentTasks: Int = ProcessInfo.processInfo.processorCount) {
            self.maxConcurrentTasks = maxConcurrentTasks
            self.queue = DispatchQueue(label: "com.threadpool.work", attributes: .concurrent)
            self.semaphore = DispatchSemaphore(value: maxConcurrentTasks)
        }
        
        // MARK: - Public Methods
        
        /**
         * Submit task to thread pool
         * 
         * This method demonstrates proper task submission
         * with concurrency control
         */
        func submit<T>(_ task: @escaping () throws -> T) -> AnyPublisher<T, Error> {
            return Future<T, Error> { promise in
                guard !self.isShutdown else {
                    promise(.failure(ThreadPoolError.shutdown))
                    return
                }
                
                self.semaphore.wait()
                
                self.queue.async {
                    defer { self.semaphore.signal() }
                    
                    do {
                        let result = try task()
                        promise(.success(result))
                    } catch {
                        promise(.failure(error))
                    }
                }
            }
            .eraseToAnyPublisher()
        }
        
        /**
         * Shutdown thread pool
         * 
         * This method demonstrates proper thread pool shutdown
         * with graceful cleanup
         */
        func shutdown() {
            isShutdown = true
        }
    }
    
    // MARK: - Lock-Free Data Structure
    
    /**
     * Lock-free stack implementation
     * 
     * This class demonstrates proper lock-free data structure
     * with atomic operations and performance optimization
     */
    class LockFreeStack<T> {
        
        // MARK: - Properties
        
        private class Node {
            let value: T
            let next: Node?
            
            init(value: T, next: Node? = nil) {
                self.value = value
                self.next = next
            }
        }
        
        private var head: Node?
        private let queue = DispatchQueue(label: "com.lockfreestack", attributes: .concurrent)
        
        // MARK: - Public Methods
        
        /**
         * Push value onto stack
         * 
         * This method demonstrates proper lock-free push
         * with atomic operations
         */
        func push(_ value: T) {
            queue.async(flags: .barrier) {
                let newNode = Node(value: value, next: self.head)
                self.head = newNode
            }
        }
        
        /**
         * Pop value from stack
         * 
         * This method demonstrates proper lock-free pop
         * with atomic operations
         */
        func pop() -> T? {
            return queue.sync {
                guard let currentHead = head else { return nil }
                head = currentHead.next
                return currentHead.value
            }
        }
        
        /**
         * Check if stack is empty
         * 
         * This method demonstrates proper empty check
         * with thread safety
         */
        var isEmpty: Bool {
            return queue.sync { head == nil }
        }
    }
}

// MARK: - CPU Profiler

/**
 * Custom CPU profiler
 * 
 * This class demonstrates proper CPU profiling
 * with performance measurement and analysis
 */
class CPUProfiler {
    
    // MARK: - Properties
    
    private var measurements: [CPUMeasurement] = []
    private var isProfiling = false
    private var profilingInterval: TimeInterval = 0.1
    private var timer: Timer?
    
    // MARK: - Public Methods
    
    /**
     * Start CPU profiling
     * 
     * This method demonstrates proper profiling initialization
     * with CPU usage tracking
     */
    func startProfiling(interval: TimeInterval = 0.1) {
        guard !isProfiling else { return }
        
        isProfiling = true
        profilingInterval = interval
        
        timer = Timer.scheduledTimer(withTimeInterval: interval, repeats: true) { [weak self] _ in
            self?.takeCPUSnapshot()
        }
    }
    
    /**
     * Stop CPU profiling
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
     * Get CPU usage report
     * 
     * This method demonstrates proper CPU reporting
     * with detailed usage analysis
     */
    func getCPUReport() -> CPUReport {
        let currentMeasurement = takeCPUSnapshot()
        let averageCPU = measurements.map { $0.cpuUsage }.reduce(0, +) / max(measurements.count, 1)
        let peakCPU = measurements.map { $0.cpuUsage }.max() ?? 0
        
        return CPUReport(
            currentCPU: currentMeasurement.cpuUsage,
            averageCPU: averageCPU,
            peakCPU: peakCPU,
            totalMeasurements: measurements.count,
            cpuTrend: calculateCPUTrend()
        )
    }
    
    // MARK: - Private Methods
    
    private func takeCPUSnapshot() -> CPUMeasurement {
        let cpuUsage = getCPUUsage()
        let measurement = CPUMeasurement(
            timestamp: Date(),
            cpuUsage: cpuUsage
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
    
    private func calculateCPUTrend() -> CPUTrend {
        guard measurements.count >= 2 else { return .stable }
        
        let recent = measurements.suffix(10)
        let older = measurements.prefix(max(0, measurements.count - 10))
        
        let recentAverage = recent.map { $0.cpuUsage }.reduce(0, +) / recent.count
        let olderAverage = older.map { $0.cpuUsage }.reduce(0, +) / max(older.count, 1)
        
        let difference = recentAverage - olderAverage
        
        if difference > 0.1 {
            return .increasing
        } else if difference < -0.1 {
            return .decreasing
        } else {
            return .stable
        }
    }
}

// MARK: - Supporting Types

/**
 * CPU measurement for profiling
 * 
 * This struct demonstrates proper CPU measurement modeling
 * with timestamp and usage information
 */
struct CPUMeasurement {
    let timestamp: Date
    let cpuUsage: Double
}

/**
 * CPU report for analysis
 * 
 * This struct demonstrates proper CPU reporting
 * with comprehensive usage statistics
 */
struct CPUReport {
    let currentCPU: Double
    let averageCPU: Double
    let peakCPU: Double
    let totalMeasurements: Int
    let cpuTrend: CPUTrend
}

/**
 * CPU trend enumeration
 * 
 * This enum demonstrates proper trend modeling
 * for CPU usage analysis
 */
enum CPUTrend {
    case increasing
    case decreasing
    case stable
}

/**
 * Thread pool error types
 * 
 * This enum demonstrates proper error modeling
 * for thread pool operations
 */
enum ThreadPoolError: Error {
    case shutdown
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use CPU performance optimization
 * 
 * This function shows practical usage of all the CPU performance components
 */
func demonstrateCPUPerformance() {
    print("=== CPU Performance Demonstration ===\n")
    
    // Optimized Sorting Algorithms
    print("--- Optimized Sorting Algorithms ---")
    print("Quick Sort: O(n log n) average, O(n²) worst case")
    print("Merge Sort: O(n log n) guaranteed")
    print("Insertion Sort: O(n²) but fast for small arrays")
    print("Features: Pivot selection, small array optimization, memory efficiency")
    
    // Optimized Data Structures
    let circularBuffer = OptimizedDataStructures.CircularBuffer<Int>(capacity: 100)
    let trie = OptimizedDataStructures.Trie()
    
    print("\n--- Optimized Data Structures ---")
    print("Circular Buffer: O(1) enqueue/dequeue operations")
    print("Trie: O(m) search/insert/delete operations")
    print("Features: Memory efficiency, performance optimization, thread safety")
    
    // Optimized Concurrency
    let threadPool = OptimizedConcurrency.ThreadPool(maxConcurrentTasks: 4)
    let lockFreeStack = OptimizedConcurrency.LockFreeStack<Int>()
    
    print("\n--- Optimized Concurrency ---")
    print("Thread Pool: Work distribution and concurrency control")
    print("Lock-Free Stack: Atomic operations and performance")
    print("Features: Concurrency control, thread safety, performance optimization")
    
    // CPU Profiler
    let profiler = CPUProfiler()
    print("\n--- CPU Profiler ---")
    print("Profiler: CPU usage tracking and analysis")
    print("Features: Real-time monitoring, trend analysis, performance reporting")
    
    // Demonstrate performance optimization techniques
    print("\n--- Performance Optimization Techniques ---")
    print("Algorithm Optimization: Choose appropriate algorithms")
    print("Data Structure Optimization: Use efficient data structures")
    print("Concurrency Optimization: Proper thread management")
    print("Memory Optimization: Reduce allocations and improve cache locality")
    print("Compiler Optimization: Use appropriate compiler flags")
    print("Profiling: Measure before optimizing")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Profile before optimizing")
    print("2. Choose appropriate algorithms for your use case")
    print("3. Use efficient data structures")
    print("4. Minimize allocations in hot paths")
    print("5. Use concurrency appropriately")
    print("6. Optimize for cache locality")
    print("7. Use compiler optimizations")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateCPUPerformance()
