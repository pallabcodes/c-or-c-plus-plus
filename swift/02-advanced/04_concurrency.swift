/*
 * Advanced Swift: Concurrency
 * 
 * This file demonstrates production-grade concurrency patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master modern async/await concurrency patterns
 * - Understand actors and thread-safe data access
 * - Implement task management and coordination
 * - Apply advanced concurrency patterns for performance
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Async/Await Fundamentals

/**
 * Demonstrates modern async/await concurrency patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic async/await syntax and patterns
 * - Async functions and error handling
 * - Async sequences and iteration
 * - Performance considerations for async operations
 */
class AsyncAwaitFundamentals {
    
    // MARK: - Basic Async Functions
    
    /**
     * Demonstrates basic async function with data fetching
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Data fetched from the URL
     * - Throws: NetworkError if the operation fails
     */
    func fetchData(from url: URL) async throws -> Data {
        // Simulate network delay
        try await Task.sleep(nanoseconds: 1_000_000_000) // 1 second
        
        // Simulate network request
        let (data, response) = try await URLSession.shared.data(from: url)
        
        guard let httpResponse = response as? HTTPURLResponse else {
            throw NetworkError.invalidResponse
        }
        
        guard httpResponse.statusCode == 200 else {
            throw NetworkError.serverError(httpResponse.statusCode)
        }
        
        return data
    }
    
    /**
     * Demonstrates async function with multiple operations
     * 
     * - Parameter urls: Array of URLs to fetch data from
     * - Returns: Array of data fetched from URLs
     * - Throws: NetworkError if any operation fails
     */
    func fetchMultipleData(from urls: [URL]) async throws -> [Data] {
        var results: [Data] = []
        
        for url in urls {
            let data = try await fetchData(from: url)
            results.append(data)
        }
        
        return results
    }
    
    /**
     * Demonstrates async function with concurrent operations
     * 
     * - Parameter urls: Array of URLs to fetch data from
     * - Returns: Array of data fetched from URLs
     * - Throws: NetworkError if any operation fails
     */
    func fetchMultipleDataConcurrently(from urls: [URL]) async throws -> [Data] {
        return try await withThrowingTaskGroup(of: Data.self) { group in
            for url in urls {
                group.addTask {
                    try await self.fetchData(from: url)
                }
            }
            
            var results: [Data] = []
            for try await data in group {
                results.append(data)
            }
            
            return results
        }
    }
    
    // MARK: - Async Error Handling
    
    /**
     * Demonstrates async function with comprehensive error handling
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Result with data or error
     */
    func fetchDataWithErrorHandling(from url: URL) async -> Result<Data, NetworkError> {
        do {
            let data = try await fetchData(from: url)
            return .success(data)
        } catch let error as NetworkError {
            return .failure(error)
        } catch {
            return .failure(.unknown(error.localizedDescription))
        }
    }
    
    /**
     * Demonstrates async function with retry logic
     * 
     * - Parameters:
     *   - url: URL to fetch data from
     *   - maxRetries: Maximum number of retries
     * - Returns: Data fetched from the URL
     * - Throws: NetworkError if all retries fail
     */
    func fetchDataWithRetry(from url: URL, maxRetries: Int = 3) async throws -> Data {
        var lastError: Error?
        
        for attempt in 1...maxRetries {
            do {
                return try await fetchData(from: url)
            } catch {
                lastError = error
                
                if attempt < maxRetries {
                    // Exponential backoff
                    let delay = UInt64(pow(2.0, Double(attempt - 1)) * 1_000_000_000)
                    try await Task.sleep(nanoseconds: delay)
                }
            }
        }
        
        throw lastError ?? NetworkError.unknown("Unknown error")
    }
    
    // MARK: - Async Sequences
    
    /**
     * Demonstrates async sequence for streaming data
     * 
     * - Parameter url: URL to stream data from
     * - Returns: Async sequence of data chunks
     */
    func streamData(from url: URL) -> AsyncThrowingStream<Data, Error> {
        return AsyncThrowingStream { continuation in
            Task {
                do {
                    let (data, response) = try await URLSession.shared.data(from: url)
                    
                    guard let httpResponse = response as? HTTPURLResponse,
                          httpResponse.statusCode == 200 else {
                        continuation.finish(throwing: NetworkError.serverError(500))
                        return
                    }
                    
                    // Simulate streaming by chunking data
                    let chunkSize = 1024
                    var offset = 0
                    
                    while offset < data.count {
                        let endIndex = min(offset + chunkSize, data.count)
                        let chunk = data.subdata(in: offset..<endIndex)
                        continuation.yield(chunk)
                        offset = endIndex
                        
                        // Simulate network delay
                        try await Task.sleep(nanoseconds: 100_000_000) // 0.1 seconds
                    }
                    
                    continuation.finish()
                } catch {
                    continuation.finish(throwing: error)
                }
            }
        }
    }
    
    /**
     * Demonstrates async sequence iteration
     * 
     * - Parameter url: URL to stream data from
     * - Returns: Total bytes received
     */
    func processStreamedData(from url: URL) async throws -> Int {
        var totalBytes = 0
        
        for try await chunk in streamData(from: url) {
            totalBytes += chunk.count
            print("Received chunk: \(chunk.count) bytes, total: \(totalBytes)")
        }
        
        return totalBytes
    }
}

// MARK: - Actors

/**
 * Demonstrates actor patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic actor definition and usage
 * - Actor isolation and thread safety
 * - Actor communication and messaging
 * - Performance considerations for actors
 */
class ActorPatterns {
    
    // MARK: - Basic Actor
    
    /**
     * Actor for managing user data with thread-safe access
     * 
     * This actor demonstrates basic actor patterns and isolation
     */
    actor UserDataManager {
        private var users: [UUID: User] = [:]
        private var lastUpdate: Date = Date()
        
        /**
         * Adds a user to the manager
         * 
         * - Parameter user: User to add
         */
        func addUser(_ user: User) {
            users[user.id] = user
            lastUpdate = Date()
        }
        
        /**
         * Gets a user by ID
         * 
         * - Parameter id: User ID to look up
         * - Returns: User if found, nil otherwise
         */
        func getUser(id: UUID) -> User? {
            return users[id]
        }
        
        /**
         * Gets all users
         * 
         * - Returns: Array of all users
         */
        func getAllUsers() -> [User] {
            return Array(users.values)
        }
        
        /**
         * Updates a user
         * 
         * - Parameter user: User to update
         */
        func updateUser(_ user: User) {
            users[user.id] = user
            lastUpdate = Date()
        }
        
        /**
         * Removes a user
         * 
         * - Parameter id: User ID to remove
         */
        func removeUser(id: UUID) {
            users.removeValue(forKey: id)
            lastUpdate = Date()
        }
        
        /**
         * Gets the count of users
         * 
         * - Returns: Number of users
         */
        func getUserCount() -> Int {
            return users.count
        }
        
        /**
         * Gets the last update time
         * 
         * - Returns: Last update time
         */
        func getLastUpdate() -> Date {
            return lastUpdate
        }
    }
    
    // MARK: - Actor with State Management
    
    /**
     * Actor for managing application state with thread-safe access
     * 
     * This actor demonstrates state management patterns
     */
    actor ApplicationStateManager {
        private var state: ApplicationState = .initializing
        private var stateHistory: [ApplicationState] = []
        private let maxHistorySize = 100
        
        /**
         * Updates the application state
         * 
         * - Parameter newState: New state to set
         */
        func updateState(_ newState: ApplicationState) {
            stateHistory.append(state)
            if stateHistory.count > maxHistorySize {
                stateHistory.removeFirst()
            }
            state = newState
        }
        
        /**
         * Gets the current state
         * 
         * - Returns: Current application state
         */
        func getCurrentState() -> ApplicationState {
            return state
        }
        
        /**
         * Gets the state history
         * 
         * - Returns: Array of previous states
         */
        func getStateHistory() -> [ApplicationState] {
            return stateHistory
        }
        
        /**
         * Checks if the state can transition to a new state
         * 
         * - Parameter newState: State to transition to
         * - Returns: True if transition is allowed
         */
        func canTransitionTo(_ newState: ApplicationState) -> Bool {
            switch (state, newState) {
            case (.initializing, .loading):
                return true
            case (.loading, .ready):
                return true
            case (.ready, .running):
                return true
            case (.running, .paused):
                return true
            case (.paused, .running):
                return true
            case (.running, .stopping):
                return true
            case (.stopping, .stopped):
                return true
            case (.stopped, .initializing):
                return true
            default:
                return false
            }
        }
        
        /**
         * Transitions to a new state if allowed
         * 
         * - Parameter newState: State to transition to
         * - Returns: True if transition was successful
         */
        func transitionTo(_ newState: ApplicationState) -> Bool {
            if canTransitionTo(newState) {
                updateState(newState)
                return true
            }
            return false
        }
    }
    
    // MARK: - Actor Communication
    
    /**
     * Actor for managing notifications with thread-safe access
     * 
     * This actor demonstrates actor communication patterns
     */
    actor NotificationManager {
        private var observers: [UUID: NotificationObserver] = [:]
        private var notificationQueue: [Notification] = []
        private let maxQueueSize = 1000
        
        /**
         * Adds an observer
         * 
         * - Parameter observer: Observer to add
         * - Returns: Observer ID
         */
        func addObserver(_ observer: NotificationObserver) -> UUID {
            let id = UUID()
            observers[id] = observer
            return id
        }
        
        /**
         * Removes an observer
         * 
         * - Parameter id: Observer ID to remove
         */
        func removeObserver(id: UUID) {
            observers.removeValue(forKey: id)
        }
        
        /**
         * Posts a notification
         * 
         * - Parameter notification: Notification to post
         */
        func postNotification(_ notification: Notification) {
            notificationQueue.append(notification)
            if notificationQueue.count > maxQueueSize {
                notificationQueue.removeFirst()
            }
            
            // Notify all observers
            for observer in observers.values {
                Task {
                    await observer.receiveNotification(notification)
                }
            }
        }
        
        /**
         * Gets the notification queue
         * 
         * - Returns: Array of notifications
         */
        func getNotificationQueue() -> [Notification] {
            return notificationQueue
        }
        
        /**
         * Clears the notification queue
         */
        func clearNotificationQueue() {
            notificationQueue.removeAll()
        }
    }
}

// MARK: - Task Management

/**
 * Demonstrates task management patterns used in production iOS apps
 * 
 * This class covers:
 * - Task creation and cancellation
 * - Task coordination and dependencies
 * - Task groups and parallel execution
 * - Performance considerations for task management
 */
class TaskManagement {
    
    // MARK: - Basic Task Management
    
    /**
     * Demonstrates basic task creation and management
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Data fetched from the URL
     */
    func fetchDataWithTask(from url: URL) async throws -> Data {
        return try await withCheckedThrowingContinuation { continuation in
            let task = Task {
                do {
                    let data = try await URLSession.shared.data(from: url).0
                    continuation.resume(returning: data)
                } catch {
                    continuation.resume(throwing: error)
                }
            }
            
            // Store task for potential cancellation
            // In production, you would store this in a task manager
        }
    }
    
    /**
     * Demonstrates task cancellation
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Data fetched from the URL or nil if cancelled
     */
    func fetchDataWithCancellation(from url: URL) async -> Data? {
        return await withTaskCancellationHandler {
            do {
                return try await URLSession.shared.data(from: url).0
            } catch {
                return nil
            }
        } onCancel: {
            print("Task was cancelled")
        }
    }
    
    // MARK: - Task Coordination
    
    /**
     * Demonstrates task coordination with dependencies
     * 
     * - Parameter urls: Array of URLs to fetch data from
     * - Returns: Array of data fetched from URLs
     */
    func fetchDataWithDependencies(from urls: [URL]) async throws -> [Data] {
        var results: [Data] = []
        
        for url in urls {
            let data = try await fetchDataWithTask(from: url)
            results.append(data)
            
            // Process data before fetching next
            let processedData = processData(data)
            results[results.count - 1] = processedData
        }
        
        return results
    }
    
    /**
     * Demonstrates task coordination with parallel execution
     * 
     * - Parameter urls: Array of URLs to fetch data from
     * - Returns: Array of data fetched from URLs
     */
    func fetchDataInParallel(from urls: [URL]) async throws -> [Data] {
        return try await withThrowingTaskGroup(of: Data.self) { group in
            for url in urls {
                group.addTask {
                    try await self.fetchDataWithTask(from: url)
                }
            }
            
            var results: [Data] = []
            for try await data in group {
                results.append(data)
            }
            
            return results
        }
    }
    
    // MARK: - Task Groups
    
    /**
     * Demonstrates task groups with different task types
     * 
     * - Parameter urls: Array of URLs to fetch data from
     * - Returns: Array of processed data
     */
    func fetchAndProcessData(from urls: [URL]) async throws -> [ProcessedData] {
        return try await withThrowingTaskGroup(of: ProcessedData.self) { group in
            for url in urls {
                group.addTask {
                    let data = try await self.fetchDataWithTask(from: url)
                    return self.processData(data)
                }
            }
            
            var results: [ProcessedData] = []
            for try await processedData in group {
                results.append(processedData)
            }
            
            return results
        }
    }
    
    /**
     * Demonstrates task groups with error handling
     * 
     * - Parameter urls: Array of URLs to fetch data from
     * - Returns: Array of successful data fetches
     */
    func fetchDataWithErrorHandling(from urls: [URL]) async -> [Data] {
        return await withTaskGroup(of: Data?.self) { group in
            for url in urls {
                group.addTask {
                    do {
                        return try await self.fetchDataWithTask(from: url)
                    } catch {
                        print("Failed to fetch data from \(url): \(error)")
                        return nil
                    }
                }
            }
            
            var results: [Data] = []
            for await data in group {
                if let data = data {
                    results.append(data)
                }
            }
            
            return results
        }
    }
    
    // MARK: - Helper Methods
    
    /**
     * Processes data (placeholder implementation)
     * 
     * - Parameter data: Data to process
     * - Returns: Processed data
     */
    private func processData(_ data: Data) -> Data {
        // In production, this would perform actual data processing
        return data
    }
}

// MARK: - Advanced Concurrency Patterns

/**
 * Demonstrates advanced concurrency patterns used in production iOS apps
 * 
 * This class covers:
 * - Producer-consumer patterns
 * - Pipeline processing patterns
 * - Rate limiting and throttling
 * - Performance optimization techniques
 */
class AdvancedConcurrencyPatterns {
    
    // MARK: - Producer-Consumer Pattern
    
    /**
     * Actor for managing producer-consumer pattern
     * 
     * This actor demonstrates producer-consumer patterns with async/await
     */
    actor ProducerConsumerManager<T> {
        private var queue: [T] = []
        private let maxQueueSize: Int
        private var isProducing = false
        private var isConsuming = false
        
        init(maxQueueSize: Int = 100) {
            self.maxQueueSize = maxQueueSize
        }
        
        /**
         * Produces an item
         * 
         * - Parameter item: Item to produce
         * - Returns: True if item was produced, false if queue is full
         */
        func produce(_ item: T) -> Bool {
            guard queue.count < maxQueueSize else {
                return false
            }
            
            queue.append(item)
            return true
        }
        
        /**
         * Consumes an item
         * 
         * - Returns: Item if available, nil otherwise
         */
        func consume() -> T? {
            guard !queue.isEmpty else {
                return nil
            }
            
            return queue.removeFirst()
        }
        
        /**
         * Gets the queue size
         * 
         * - Returns: Number of items in queue
         */
        func getQueueSize() -> Int {
            return queue.count
        }
        
        /**
         * Checks if the queue is empty
         * 
         * - Returns: True if queue is empty
         */
        func isEmpty() -> Bool {
            return queue.isEmpty
        }
        
        /**
         * Checks if the queue is full
         * 
         * - Returns: True if queue is full
         */
        func isFull() -> Bool {
            return queue.count >= maxQueueSize
        }
    }
    
    // MARK: - Pipeline Processing Pattern
    
    /**
     * Demonstrates pipeline processing pattern
     * 
     * - Parameter data: Array of data to process
     * - Returns: Array of processed data
     */
    func processDataInPipeline(_ data: [Data]) async -> [ProcessedData] {
        return await withTaskGroup(of: ProcessedData?.self) { group in
            for item in data {
                group.addTask {
                    await self.processDataInPipeline(item)
                }
            }
            
            var results: [ProcessedData] = []
            for await processedData in group {
                if let processedData = processedData {
                    results.append(processedData)
                }
            }
            
            return results
        }
    }
    
    /**
     * Processes a single data item in pipeline
     * 
     * - Parameter data: Data to process
     * - Returns: Processed data
     */
    private func processDataInPipeline(_ data: Data) async -> ProcessedData? {
        // Stage 1: Validate data
        guard !data.isEmpty else {
            return nil
        }
        
        // Stage 2: Transform data
        let transformedData = await transformData(data)
        
        // Stage 3: Process data
        let processedData = await processData(transformedData)
        
        return processedData
    }
    
    /**
     * Transforms data (placeholder implementation)
     * 
     * - Parameter data: Data to transform
     * - Returns: Transformed data
     */
    private func transformData(_ data: Data) async -> Data {
        // Simulate transformation delay
        try? await Task.sleep(nanoseconds: 100_000_000) // 0.1 seconds
        return data
    }
    
    /**
     * Processes data (placeholder implementation)
     * 
     * - Parameter data: Data to process
     * - Returns: Processed data
     */
    private func processData(_ data: Data) async -> ProcessedData {
        // Simulate processing delay
        try? await Task.sleep(nanoseconds: 200_000_000) // 0.2 seconds
        return ProcessedData(data: data, processedAt: Date())
    }
    
    // MARK: - Rate Limiting and Throttling
    
    /**
     * Actor for managing rate limiting
     * 
     * This actor demonstrates rate limiting patterns
     */
    actor RateLimiter {
        private var requestCount = 0
        private var lastResetTime = Date()
        private let maxRequests: Int
        private let timeWindow: TimeInterval
        
        init(maxRequests: Int = 100, timeWindow: TimeInterval = 60.0) {
            self.maxRequests = maxRequests
            self.timeWindow = timeWindow
        }
        
        /**
         * Checks if a request is allowed
         * 
         * - Returns: True if request is allowed
         */
        func isRequestAllowed() -> Bool {
            let now = Date()
            
            // Reset counter if time window has passed
            if now.timeIntervalSince(lastResetTime) >= timeWindow {
                requestCount = 0
                lastResetTime = now
            }
            
            // Check if we're within the limit
            if requestCount < maxRequests {
                requestCount += 1
                return true
            }
            
            return false
        }
        
        /**
         * Gets the current request count
         * 
         * - Returns: Number of requests in current window
         */
        func getRequestCount() -> Int {
            return requestCount
        }
        
        /**
         * Gets the time until next reset
         * 
         * - Returns: Time until next reset
         */
        func getTimeUntilReset() -> TimeInterval {
            let now = Date()
            let timeSinceReset = now.timeIntervalSince(lastResetTime)
            return max(0, timeWindow - timeSinceReset)
        }
    }
    
    // MARK: - Performance Optimization
    
    /**
     * Demonstrates performance optimization with concurrent processing
     * 
     * - Parameter data: Array of data to process
     * - Returns: Array of processed data
     */
    func processDataWithOptimization(_ data: [Data]) async -> [ProcessedData] {
        let batchSize = min(10, data.count) // Process in batches of 10
        var results: [ProcessedData] = []
        
        for i in stride(from: 0, to: data.count, by: batchSize) {
            let batch = Array(data[i..<min(i + batchSize, data.count)])
            let batchResults = await processBatch(batch)
            results.append(contentsOf: batchResults)
        }
        
        return results
    }
    
    /**
     * Processes a batch of data
     * 
     * - Parameter batch: Batch of data to process
     * - Returns: Array of processed data
     */
    private func processBatch(_ batch: [Data]) async -> [ProcessedData] {
        return await withTaskGroup(of: ProcessedData.self) { group in
            for item in batch {
                group.addTask {
                    await self.processData(item)
                }
            }
            
            var results: [ProcessedData] = []
            for await processedData in group {
                results.append(processedData)
            }
            
            return results
        }
    }
}

// MARK: - Supporting Types

/**
 * Network error types for demonstration
 */
enum NetworkError: Error, LocalizedError {
    case invalidResponse
    case serverError(Int)
    case unknown(String)
    
    var errorDescription: String? {
        switch self {
        case .invalidResponse:
            return "Invalid response received"
        case .serverError(let code):
            return "Server error with code: \(code)"
        case .unknown(let message):
            return "Unknown error: \(message)"
        }
    }
}

/**
 * User data structure for demonstration
 */
struct User {
    let id: UUID
    let name: String
    let email: String
}

/**
 * Application state enumeration
 */
enum ApplicationState {
    case initializing
    case loading
    case ready
    case running
    case paused
    case stopping
    case stopped
}

/**
 * Notification structure for demonstration
 */
struct Notification {
    let id: UUID
    let title: String
    let message: String
    let timestamp: Date
}

/**
 * Notification observer protocol
 */
protocol NotificationObserver: AnyObject {
    func receiveNotification(_ notification: Notification) async
}

/**
 * Processed data structure for demonstration
 */
struct ProcessedData {
    let data: Data
    let processedAt: Date
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the concurrency patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateConcurrency() {
    print("=== Swift Concurrency Demonstration ===\n")
    
    // Async/Await Fundamentals
    let asyncExample = AsyncAwaitFundamentals()
    
    print("--- Async/Await Fundamentals ---")
    let url = URL(string: "https://api.example.com/data")!
    
    Task {
        do {
            let data = try await asyncExample.fetchData(from: url)
            print("Data fetched successfully: \(data.count) bytes")
        } catch {
            print("Error fetching data: \(error)")
        }
    }
    
    // Actor Patterns
    let actorExample = ActorPatterns()
    
    print("\n--- Actor Patterns ---")
    let userManager = ActorPatterns.UserDataManager()
    let stateManager = ActorPatterns.ApplicationStateManager()
    
    Task {
        let user = User(id: UUID(), name: "John Doe", email: "john@example.com")
        await userManager.addUser(user)
        
        let userCount = await userManager.getUserCount()
        print("User count: \(userCount)")
        
        await stateManager.updateState(.running)
        let currentState = await stateManager.getCurrentState()
        print("Current state: \(currentState)")
    }
    
    // Task Management
    let taskExample = TaskManagement()
    
    print("\n--- Task Management ---")
    let urls = [
        URL(string: "https://api.example.com/data1")!,
        URL(string: "https://api.example.com/data2")!,
        URL(string: "https://api.example.com/data3")!
    ]
    
    Task {
        do {
            let data = try await taskExample.fetchDataInParallel(from: urls)
            print("Fetched \(data.count) data items in parallel")
        } catch {
            print("Error fetching data in parallel: \(error)")
        }
    }
    
    // Advanced Concurrency Patterns
    let advancedExample = AdvancedConcurrencyPatterns()
    
    print("\n--- Advanced Concurrency Patterns ---")
    let producerConsumer = AdvancedConcurrencyPatterns.ProducerConsumerManager<String>()
    let rateLimiter = AdvancedConcurrencyPatterns.RateLimiter()
    
    Task {
        // Test producer-consumer
        let produced = await producerConsumer.produce("Item 1")
        print("Item produced: \(produced)")
        
        let consumed = await producerConsumer.consume()
        print("Item consumed: \(consumed ?? "None")")
        
        // Test rate limiting
        let allowed = await rateLimiter.isRequestAllowed()
        print("Request allowed: \(allowed)")
        
        let requestCount = await rateLimiter.getRequestCount()
        print("Request count: \(requestCount)")
    }
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateConcurrency()
