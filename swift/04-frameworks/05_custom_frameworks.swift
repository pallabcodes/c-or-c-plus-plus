/*
 * iOS Frameworks: Custom Frameworks
 * 
 * This file demonstrates production-grade custom framework development in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master custom framework architecture and design
 * - Understand API design and versioning strategies
 * - Implement proper documentation and distribution
 * - Apply modular design and dependency management
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import UIKit

// MARK: - Framework Architecture

/**
 * Base framework protocol defining common framework behavior
 * 
 * This protocol demonstrates proper framework interface design
 * with versioning and lifecycle management
 */
protocol FrameworkProtocol {
    associatedtype Configuration
    associatedtype Error: Swift.Error
    
    var version: String { get }
    var isInitialized: Bool { get }
    
    func initialize(with configuration: Configuration) throws
    func cleanup() throws
    func handleError(_ error: Error)
}

/**
 * Framework configuration protocol
 * 
 * This protocol demonstrates proper configuration management
 * with validation and default values
 */
protocol FrameworkConfiguration {
    var isValid: Bool { get }
    var validationErrors: [String] { get }
    
    func validate() throws
}

// MARK: - Network Framework

/**
 * Custom network framework for HTTP operations
 * 
 * This framework demonstrates production-grade network layer
 * with proper error handling and performance optimization
 */
public class NetworkFramework: FrameworkProtocol {
    
    // MARK: - Types
    
    public typealias Configuration = NetworkConfiguration
    public typealias Error = NetworkFrameworkError
    
    // MARK: - Properties
    
    public let version = "1.0.0"
    public private(set) var isInitialized = false
    
    private var session: URLSession?
    private var configuration: NetworkConfiguration?
    private var requestQueue: DispatchQueue
    private var responseQueue: DispatchQueue
    
    // MARK: - Initialization
    
    public init() {
        self.requestQueue = DispatchQueue(label: "com.framework.network.request", qos: .userInitiated)
        self.responseQueue = DispatchQueue(label: "com.framework.network.response", qos: .userInitiated)
    }
    
    // MARK: - FrameworkProtocol
    
    public func initialize(with configuration: NetworkConfiguration) throws {
        try configuration.validate()
        
        self.configuration = configuration
        self.session = createURLSession(with: configuration)
        self.isInitialized = true
    }
    
    public func cleanup() throws {
        session?.invalidateAndCancel()
        session = nil
        configuration = nil
        isInitialized = false
    }
    
    public func handleError(_ error: NetworkFrameworkError) {
        // Handle framework-specific errors
        print("Network Framework Error: \(error.localizedDescription)")
    }
    
    // MARK: - Public Methods
    
    public func request<T: Codable>(
        _ endpoint: Endpoint,
        responseType: T.Type
    ) -> AnyPublisher<T, NetworkFrameworkError> {
        guard isInitialized else {
            return Fail(error: .notInitialized)
                .eraseToAnyPublisher()
        }
        
        guard let session = session else {
            return Fail(error: .sessionNotAvailable)
                .eraseToAnyPublisher()
        }
        
        return createRequest(for: endpoint)
            .flatMap { request in
                session.dataTaskPublisher(for: request)
            }
            .map(\.data)
            .decode(type: responseType, decoder: JSONDecoder())
            .mapError { error in
                if let networkError = error as? NetworkFrameworkError {
                    return networkError
                } else {
                    return .decodingFailed(error)
                }
            }
            .receive(on: DispatchQueue.main)
            .eraseToAnyPublisher()
    }
    
    public func upload<T: Codable>(
        _ endpoint: Endpoint,
        data: Data,
        responseType: T.Type
    ) -> AnyPublisher<T, NetworkFrameworkError> {
        guard isInitialized else {
            return Fail(error: .notInitialized)
                .eraseToAnyPublisher()
        }
        
        guard let session = session else {
            return Fail(error: .sessionNotAvailable)
                .eraseToAnyPublisher()
        }
        
        return createUploadRequest(for: endpoint, data: data)
            .flatMap { request in
                session.uploadTaskPublisher(for: request, from: data)
            }
            .map(\.data)
            .decode(type: responseType, decoder: JSONDecoder())
            .mapError { error in
                if let networkError = error as? NetworkFrameworkError {
                    return networkError
                } else {
                    return .decodingFailed(error)
                }
            }
            .receive(on: DispatchQueue.main)
            .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func createURLSession(with configuration: NetworkConfiguration) -> URLSession {
        let config = URLSessionConfiguration.default
        config.timeoutIntervalForRequest = configuration.requestTimeout
        config.timeoutIntervalForResource = configuration.resourceTimeout
        config.waitsForConnectivity = true
        config.allowsCellularAccess = configuration.allowsCellularAccess
        
        return URLSession(configuration: config)
    }
    
    private func createRequest(for endpoint: Endpoint) -> AnyPublisher<URLRequest, NetworkFrameworkError> {
        return Future<URLRequest, NetworkFrameworkError> { promise in
            guard let url = endpoint.url else {
                promise(.failure(.invalidURL))
                return
            }
            
            var request = URLRequest(url: url)
            request.httpMethod = endpoint.method.rawValue
            request.allHTTPHeaderFields = endpoint.headers
            
            if let body = endpoint.body {
                request.httpBody = body
                request.setValue("application/json", forHTTPHeaderField: "Content-Type")
            }
            
            promise(.success(request))
        }
        .eraseToAnyPublisher()
    }
    
    private func createUploadRequest(for endpoint: Endpoint, data: Data) -> AnyPublisher<URLRequest, NetworkFrameworkError> {
        return Future<URLRequest, NetworkFrameworkError> { promise in
            guard let url = endpoint.url else {
                promise(.failure(.invalidURL))
                return
            }
            
            var request = URLRequest(url: url)
            request.httpMethod = endpoint.method.rawValue
            request.allHTTPHeaderFields = endpoint.headers
            request.setValue("application/octet-stream", forHTTPHeaderField: "Content-Type")
            
            promise(.success(request))
        }
        .eraseToAnyPublisher()
    }
}

// MARK: - Network Configuration

/**
 * Network framework configuration
 * 
 * This class demonstrates proper configuration management
 * with validation and default values
 */
public class NetworkConfiguration: FrameworkConfiguration {
    
    // MARK: - Properties
    
    public let baseURL: URL
    public let requestTimeout: TimeInterval
    public let resourceTimeout: TimeInterval
    public let allowsCellularAccess: Bool
    public let retryCount: Int
    public let retryDelay: TimeInterval
    
    // MARK: - Computed Properties
    
    public var isValid: Bool {
        return validationErrors.isEmpty
    }
    
    public var validationErrors: [String] {
        var errors: [String] = []
        
        if requestTimeout <= 0 {
            errors.append("Request timeout must be greater than 0")
        }
        
        if resourceTimeout <= 0 {
            errors.append("Resource timeout must be greater than 0")
        }
        
        if retryCount < 0 {
            errors.append("Retry count must be non-negative")
        }
        
        if retryDelay < 0 {
            errors.append("Retry delay must be non-negative")
        }
        
        return errors
    }
    
    // MARK: - Initialization
    
    public init(
        baseURL: URL,
        requestTimeout: TimeInterval = 30.0,
        resourceTimeout: TimeInterval = 60.0,
        allowsCellularAccess: Bool = true,
        retryCount: Int = 3,
        retryDelay: TimeInterval = 1.0
    ) {
        self.baseURL = baseURL
        self.requestTimeout = requestTimeout
        self.resourceTimeout = resourceTimeout
        self.allowsCellularAccess = allowsCellularAccess
        self.retryCount = retryCount
        self.retryDelay = retryDelay
    }
    
    // MARK: - Validation
    
    public func validate() throws {
        if !isValid {
            throw NetworkFrameworkError.invalidConfiguration(validationErrors)
        }
    }
}

// MARK: - Endpoint Definition

/**
 * HTTP endpoint definition
 * 
 * This struct demonstrates proper endpoint modeling
 * with type safety and flexibility
 */
public struct Endpoint {
    
    // MARK: - Properties
    
    public let path: String
    public let method: HTTPMethod
    public let headers: [String: String]
    public let body: Data?
    public let queryParameters: [String: String]
    
    // MARK: - Computed Properties
    
    public var url: URL? {
        var components = URLComponents()
        components.path = path
        components.queryItems = queryParameters.map { key, value in
            URLQueryItem(name: key, value: value)
        }
        return components.url
    }
    
    // MARK: - Initialization
    
    public init(
        path: String,
        method: HTTPMethod = .GET,
        headers: [String: String] = [:],
        body: Data? = nil,
        queryParameters: [String: String] = [:]
    ) {
        self.path = path
        self.method = method
        self.headers = headers
        self.body = body
        self.queryParameters = queryParameters
    }
}

// MARK: - HTTP Method

/**
 * HTTP method enumeration
 * 
 * This enum demonstrates proper HTTP method modeling
 * with type safety and extensibility
 */
public enum HTTPMethod: String {
    case GET = "GET"
    case POST = "POST"
    case PUT = "PUT"
    case DELETE = "DELETE"
    case PATCH = "PATCH"
    case HEAD = "HEAD"
    case OPTIONS = "OPTIONS"
}

// MARK: - Error Types

/**
 * Network framework error types
 * 
 * This enum demonstrates proper error modeling
 * with detailed error information
 */
public enum NetworkFrameworkError: Error, LocalizedError {
    case notInitialized
    case sessionNotAvailable
    case invalidURL
    case invalidConfiguration([String])
    case requestFailed(Error)
    case decodingFailed(Error)
    case networkUnavailable
    case timeout
    case serverError(Int)
    case clientError(Int)
    
    public var errorDescription: String? {
        switch self {
        case .notInitialized:
            return "Network framework is not initialized"
        case .sessionNotAvailable:
            return "URL session is not available"
        case .invalidURL:
            return "Invalid URL provided"
        case .invalidConfiguration(let errors):
            return "Invalid configuration: \(errors.joined(separator: ", "))"
        case .requestFailed(let error):
            return "Request failed: \(error.localizedDescription)"
        case .decodingFailed(let error):
            return "Decoding failed: \(error.localizedDescription)"
        case .networkUnavailable:
            return "Network is unavailable"
        case .timeout:
            return "Request timed out"
        case .serverError(let code):
            return "Server error: \(code)"
        case .clientError(let code):
            return "Client error: \(code)"
        }
    }
}

// MARK: - Analytics Framework

/**
 * Custom analytics framework for event tracking
 * 
 * This framework demonstrates production-grade analytics
 * with proper event modeling and performance optimization
 */
public class AnalyticsFramework: FrameworkProtocol {
    
    // MARK: - Types
    
    public typealias Configuration = AnalyticsConfiguration
    public typealias Error = AnalyticsFrameworkError
    
    // MARK: - Properties
    
    public let version = "1.0.0"
    public private(set) var isInitialized = false
    
    private var configuration: AnalyticsConfiguration?
    private var eventQueue: DispatchQueue
    private var batchProcessor: BatchProcessor?
    
    // MARK: - Initialization
    
    public init() {
        self.eventQueue = DispatchQueue(label: "com.framework.analytics.events", qos: .utility)
    }
    
    // MARK: - FrameworkProtocol
    
    public func initialize(with configuration: AnalyticsConfiguration) throws {
        try configuration.validate()
        
        self.configuration = configuration
        self.batchProcessor = BatchProcessor(configuration: configuration)
        self.isInitialized = true
    }
    
    public func cleanup() throws {
        batchProcessor?.flush()
        batchProcessor = nil
        configuration = nil
        isInitialized = false
    }
    
    public func handleError(_ error: AnalyticsFrameworkError) {
        // Handle framework-specific errors
        print("Analytics Framework Error: \(error.localizedDescription)")
    }
    
    // MARK: - Public Methods
    
    public func trackEvent(_ event: AnalyticsEvent) {
        guard isInitialized else {
            handleError(.notInitialized)
            return
        }
        
        eventQueue.async { [weak self] in
            self?.batchProcessor?.addEvent(event)
        }
    }
    
    public func trackScreenView(_ screenName: String, properties: [String: Any] = [:]) {
        let event = AnalyticsEvent(
            name: "screen_view",
            properties: [
                "screen_name": screenName,
                "timestamp": Date().timeIntervalSince1970
            ].merging(properties) { _, new in new }
        )
        trackEvent(event)
    }
    
    public func trackUserAction(_ action: String, properties: [String: Any] = [:]) {
        let event = AnalyticsEvent(
            name: "user_action",
            properties: [
                "action": action,
                "timestamp": Date().timeIntervalSince1970
            ].merging(properties) { _, new in new }
        )
        trackEvent(event)
    }
    
    public func setUserProperties(_ properties: [String: Any]) {
        let event = AnalyticsEvent(
            name: "user_properties",
            properties: properties
        )
        trackEvent(event)
    }
    
    public func flush() {
        batchProcessor?.flush()
    }
}

// MARK: - Analytics Configuration

/**
 * Analytics framework configuration
 * 
 * This class demonstrates proper configuration management
 * for analytics with performance optimization
 */
public class AnalyticsConfiguration: FrameworkConfiguration {
    
    // MARK: - Properties
    
    public let apiKey: String
    public let baseURL: URL
    public let batchSize: Int
    public let flushInterval: TimeInterval
    public let enableDebugLogging: Bool
    
    // MARK: - Computed Properties
    
    public var isValid: Bool {
        return validationErrors.isEmpty
    }
    
    public var validationErrors: [String] {
        var errors: [String] = []
        
        if apiKey.isEmpty {
            errors.append("API key cannot be empty")
        }
        
        if batchSize <= 0 {
            errors.append("Batch size must be greater than 0")
        }
        
        if flushInterval <= 0 {
            errors.append("Flush interval must be greater than 0")
        }
        
        return errors
    }
    
    // MARK: - Initialization
    
    public init(
        apiKey: String,
        baseURL: URL,
        batchSize: Int = 100,
        flushInterval: TimeInterval = 30.0,
        enableDebugLogging: Bool = false
    ) {
        self.apiKey = apiKey
        self.baseURL = baseURL
        self.batchSize = batchSize
        self.flushInterval = flushInterval
        self.enableDebugLogging = enableDebugLogging
    }
    
    // MARK: - Validation
    
    public func validate() throws {
        if !isValid {
            throw AnalyticsFrameworkError.invalidConfiguration(validationErrors)
        }
    }
}

// MARK: - Analytics Event

/**
 * Analytics event model
 * 
 * This struct demonstrates proper event modeling
 * with type safety and validation
 */
public struct AnalyticsEvent {
    
    // MARK: - Properties
    
    public let name: String
    public let properties: [String: Any]
    public let timestamp: Date
    
    // MARK: - Initialization
    
    public init(
        name: String,
        properties: [String: Any] = [:],
        timestamp: Date = Date()
    ) {
        self.name = name
        self.properties = properties
        self.timestamp = timestamp
    }
}

// MARK: - Batch Processor

/**
 * Batch processor for analytics events
 * 
 * This class demonstrates proper batch processing
 * with performance optimization and error handling
 */
private class BatchProcessor {
    
    // MARK: - Properties
    
    private let configuration: AnalyticsConfiguration
    private var events: [AnalyticsEvent] = []
    private let lock = NSLock()
    private var timer: Timer?
    
    // MARK: - Initialization
    
    init(configuration: AnalyticsConfiguration) {
        self.configuration = configuration
        setupTimer()
    }
    
    // MARK: - Public Methods
    
    func addEvent(_ event: AnalyticsEvent) {
        lock.lock()
        defer { lock.unlock() }
        
        events.append(event)
        
        if events.count >= configuration.batchSize {
            flush()
        }
    }
    
    func flush() {
        lock.lock()
        defer { lock.unlock() }
        
        guard !events.isEmpty else { return }
        
        let eventsToSend = events
        events.removeAll()
        
        sendEvents(eventsToSend)
    }
    
    // MARK: - Private Methods
    
    private func setupTimer() {
        timer = Timer.scheduledTimer(withTimeInterval: configuration.flushInterval, repeats: true) { [weak self] _ in
            self?.flush()
        }
    }
    
    private func sendEvents(_ events: [AnalyticsEvent]) {
        // In production, you would send events to your analytics service
        print("Sending \(events.count) analytics events")
    }
}

// MARK: - Analytics Error Types

/**
 * Analytics framework error types
 * 
 * This enum demonstrates proper error modeling
 * for analytics framework
 */
public enum AnalyticsFrameworkError: Error, LocalizedError {
    case notInitialized
    case invalidConfiguration([String])
    case eventTrackingFailed(Error)
    case networkError(Error)
    
    public var errorDescription: String? {
        switch self {
        case .notInitialized:
            return "Analytics framework is not initialized"
        case .invalidConfiguration(let errors):
            return "Invalid configuration: \(errors.joined(separator: ", "))"
        case .eventTrackingFailed(let error):
            return "Event tracking failed: \(error.localizedDescription)"
        case .networkError(let error):
            return "Network error: \(error.localizedDescription)"
        }
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use custom frameworks
 * 
 * This function shows practical usage of all the framework components
 */
func demonstrateCustomFrameworks() {
    print("=== Custom Frameworks Demonstration ===\n")
    
    // Network Framework
    let networkConfig = NetworkConfiguration(
        baseURL: URL(string: "https://api.example.com")!,
        requestTimeout: 30.0,
        resourceTimeout: 60.0
    )
    
    let networkFramework = NetworkFramework()
    try? networkFramework.initialize(with: networkConfig)
    
    print("--- Network Framework ---")
    print("Version: \(networkFramework.version)")
    print("Initialized: \(networkFramework.isInitialized)")
    print("Configuration: \(networkConfig.isValid)")
    
    // Analytics Framework
    let analyticsConfig = AnalyticsConfiguration(
        apiKey: "your-api-key",
        baseURL: URL(string: "https://analytics.example.com")!,
        batchSize: 100,
        flushInterval: 30.0
    )
    
    let analyticsFramework = AnalyticsFramework()
    try? analyticsFramework.initialize(with: analyticsConfig)
    
    print("\n--- Analytics Framework ---")
    print("Version: \(analyticsFramework.version)")
    print("Initialized: \(analyticsFramework.isInitialized)")
    print("Configuration: \(analyticsConfig.isValid)")
    
    // Demonstrate event tracking
    analyticsFramework.trackScreenView("HomeScreen")
    analyticsFramework.trackUserAction("button_tap", properties: ["button_id": "login"])
    analyticsFramework.setUserProperties(["user_id": "123", "plan": "premium"])
    
    print("\n--- Framework Features ---")
    print("Network Framework: HTTP requests, uploads, error handling")
    print("Analytics Framework: Event tracking, batch processing, user properties")
    print("Configuration: Validation, default values, error handling")
    print("Error Handling: Localized errors, proper error types")
    print("Performance: Batch processing, background queues, optimization")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateCustomFrameworks()
