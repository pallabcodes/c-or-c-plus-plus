/*
 * Design Patterns: Dependency Injection
 * 
 * This file demonstrates production-grade Dependency Injection patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master IoC container and service registration
 * - Understand factory patterns and object creation
 * - Implement proper singleton management
 * - Apply testing and mocking strategies
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Dependency Injection Container

/**
 * Main dependency injection container for managing service registration and resolution
 * 
 * This class demonstrates proper IoC container implementation
 * with service lifecycle management and dependency resolution
 */
class DIContainer {
    
    // MARK: - Service Registration Types
    
    private enum ServiceRegistration {
        case singleton(() -> Any)
        case transient(() -> Any)
        case scoped(() -> Any)
    }
    
    // MARK: - Private Properties
    
    private var services: [String: ServiceRegistration] = [:]
    private var singletons: [String: Any] = [:]
    private var scopedInstances: [String: Any] = [:]
    private let lock = NSLock()
    
    // MARK: - Service Registration
    
    /**
     * Registers a singleton service
     * 
     * - Parameters:
     *   - type: Service type to register
     *   - factory: Factory closure to create the service
     */
    func registerSingleton<T>(_ type: T.Type, factory: @escaping () -> T) {
        let key = String(describing: type)
        services[key] = .singleton(factory)
    }
    
    /**
     * Registers a transient service
     * 
     * - Parameters:
     *   - type: Service type to register
     *   - factory: Factory closure to create the service
     */
    func registerTransient<T>(_ type: T.Type, factory: @escaping () -> T) {
        let key = String(describing: type)
        services[key] = .transient(factory)
    }
    
    /**
     * Registers a scoped service
     * 
     * - Parameters:
     *   - type: Service type to register
     *   - factory: Factory closure to create the service
     */
    func registerScoped<T>(_ type: T.Type, factory: @escaping () -> T) {
        let key = String(describing: type)
        services[key] = .scoped(factory)
    }
    
    /**
     * Registers a service with a specific instance
     * 
     * - Parameters:
     *   - type: Service type to register
     *   - instance: Instance to register
     */
    func registerInstance<T>(_ type: T.Type, instance: T) {
        let key = String(describing: type)
        services[key] = .singleton { instance }
        singletons[key] = instance
    }
    
    // MARK: - Service Resolution
    
    /**
     * Resolves a service instance
     * 
     * - Parameter type: Service type to resolve
     * - Returns: Resolved service instance
     */
    func resolve<T>(_ type: T.Type) -> T {
        let key = String(describing: type)
        
        lock.lock()
        defer { lock.unlock() }
        
        guard let registration = services[key] else {
            fatalError("Service \(type) not registered")
        }
        
        switch registration {
        case .singleton(let factory):
            if let instance = singletons[key] as? T {
                return instance
            } else {
                let instance = factory() as! T
                singletons[key] = instance
                return instance
            }
        case .transient(let factory):
            return factory() as! T
        case .scoped(let factory):
            if let instance = scopedInstances[key] as? T {
                return instance
            } else {
                let instance = factory() as! T
                scopedInstances[key] = instance
                return instance
            }
        }
    }
    
    /**
     * Resolves an optional service instance
     * 
     * - Parameter type: Service type to resolve
     * - Returns: Resolved service instance or nil
     */
    func resolveOptional<T>(_ type: T.Type) -> T? {
        let key = String(describing: type)
        
        lock.lock()
        defer { lock.unlock() }
        
        guard let registration = services[key] else {
            return nil
        }
        
        switch registration {
        case .singleton(let factory):
            if let instance = singletons[key] as? T {
                return instance
            } else {
                let instance = factory() as! T
                singletons[key] = instance
                return instance
            }
        case .transient(let factory):
            return factory() as! T
        case .scoped(let factory):
            if let instance = scopedInstances[key] as? T {
                return instance
            } else {
                let instance = factory() as! T
                scopedInstances[key] = instance
                return instance
            }
        }
    }
    
    // MARK: - Scope Management
    
    /**
     * Creates a new scope for scoped services
     * 
     * - Returns: New scoped container
     */
    func createScope() -> DIContainer {
        let scopedContainer = DIContainer()
        scopedContainer.services = services
        return scopedContainer
    }
    
    /**
     * Clears the current scope
     */
    func clearScope() {
        lock.lock()
        defer { lock.unlock() }
        scopedInstances.removeAll()
    }
    
    // MARK: - Service Validation
    
    /**
     * Validates that all required services are registered
     * 
     * - Throws: ValidationError if services are missing
     */
    func validate() throws {
        // In production, this would validate all required services
        // and check for circular dependencies
    }
}

// MARK: - Service Locator Pattern

/**
 * Service locator for global service access
 * 
 * This class demonstrates the Service Locator pattern
 * with proper service discovery and registration
 */
class ServiceLocator {
    
    private static var container: DIContainer?
    
    /**
     * Sets the global container
     * 
     * - Parameter container: Container to set as global
     */
    static func setContainer(_ container: DIContainer) {
        self.container = container
    }
    
    /**
     * Resolves a service from the global container
     * 
     * - Parameter type: Service type to resolve
     * - Returns: Resolved service instance
     */
    static func resolve<T>(_ type: T.Type) -> T {
        guard let container = container else {
            fatalError("No container set in ServiceLocator")
        }
        return container.resolve(type)
    }
    
    /**
     * Resolves an optional service from the global container
     * 
     * - Parameter type: Service type to resolve
     * - Returns: Resolved service instance or nil
     */
    static func resolveOptional<T>(_ type: T.Type) -> T? {
        guard let container = container else {
            return nil
        }
        return container.resolveOptional(type)
    }
}

// MARK: - Factory Pattern

/**
 * Factory protocol for creating objects
 * 
 * This protocol demonstrates the Factory pattern
 * with proper object creation and configuration
 */
protocol Factory {
    associatedtype Product
    
    func create() -> Product
}

/**
 * Generic factory implementation
 * 
 * This class demonstrates generic factory implementation
 * with proper object creation and configuration
 */
class GenericFactory<T>: Factory {
    typealias Product = T
    
    private let factory: () -> T
    
    init(factory: @escaping () -> T) {
        self.factory = factory
    }
    
    func create() -> T {
        return factory()
    }
}

/**
 * Factory for creating network services
 * 
 * This class demonstrates domain-specific factory implementation
 * with proper configuration and dependency injection
 */
class NetworkServiceFactory: Factory {
    typealias Product = NetworkServiceProtocol
    
    private let baseURL: URL
    private let configuration: URLSessionConfiguration
    
    init(baseURL: URL, configuration: URLSessionConfiguration = .default) {
        self.baseURL = baseURL
        self.configuration = configuration
    }
    
    func create() -> NetworkServiceProtocol {
        return NetworkService(baseURL: baseURL, configuration: configuration)
    }
}

/**
 * Factory for creating user services
 * 
 * This class demonstrates factory with dependencies
 * and proper service composition
 */
class UserServiceFactory: Factory {
    typealias Product = UserServiceProtocol
    
    private let networkService: NetworkServiceProtocol
    private let cacheService: CacheServiceProtocol
    
    init(networkService: NetworkServiceProtocol, cacheService: CacheServiceProtocol) {
        self.networkService = networkService
        self.cacheService = cacheService
    }
    
    func create() -> UserServiceProtocol {
        return UserService(
            networkService: networkService,
            cacheService: cacheService
        )
    }
}

// MARK: - Singleton Management

/**
 * Singleton manager for controlled singleton patterns
 * 
 * This class demonstrates proper singleton management
 * with lifecycle control and testing support
 */
class SingletonManager {
    
    private static var instances: [String: Any] = [:]
    private static let lock = NSLock()
    
    /**
     * Gets or creates a singleton instance
     * 
     * - Parameters:
     *   - type: Type of the singleton
     *   - factory: Factory closure to create the instance
     * - Returns: Singleton instance
     */
    static func getInstance<T>(_ type: T.Type, factory: @escaping () -> T) -> T {
        let key = String(describing: type)
        
        lock.lock()
        defer { lock.unlock() }
        
        if let instance = instances[key] as? T {
            return instance
        } else {
            let instance = factory()
            instances[key] = instance
            return instance
        }
    }
    
    /**
     * Resets all singleton instances
     * 
     * This method is useful for testing
     */
    static func reset() {
        lock.lock()
        defer { lock.unlock() }
        instances.removeAll()
    }
    
    /**
     * Removes a specific singleton instance
     * 
     * - Parameter type: Type of the singleton to remove
     */
    static func removeInstance<T>(_ type: T.Type) {
        let key = String(describing: type)
        
        lock.lock()
        defer { lock.unlock() }
        instances.removeValue(forKey: key)
    }
}

// MARK: - Property Wrapper for Dependency Injection

/**
 * Property wrapper for automatic dependency injection
 * 
 * This property wrapper demonstrates automatic dependency resolution
 * with proper type safety and error handling
 */
@propertyWrapper
struct Injected<T> {
    private let type: T.Type
    private var value: T?
    
    init(_ type: T.Type) {
        self.type = type
    }
    
    var wrappedValue: T {
        get {
            if let value = value {
                return value
            } else {
                let resolvedValue = ServiceLocator.resolve(type)
                value = resolvedValue
                return resolvedValue
            }
        }
        set {
            value = newValue
        }
    }
}

/**
 * Property wrapper for optional dependency injection
 * 
 * This property wrapper demonstrates optional dependency resolution
 * with proper nil handling
 */
@propertyWrapper
struct InjectedOptional<T> {
    private let type: T.Type
    private var value: T?
    
    init(_ type: T.Type) {
        self.type = type
    }
    
    var wrappedValue: T? {
        get {
            if let value = value {
                return value
            } else {
                let resolvedValue = ServiceLocator.resolveOptional(type)
                value = resolvedValue
                return resolvedValue
            }
        }
        set {
            value = newValue
        }
    }
}

// MARK: - Service Protocols

/**
 * Network service protocol
 */
protocol NetworkServiceProtocol {
    func getData(from url: URL) async throws -> Data
}

/**
 * Cache service protocol
 */
protocol CacheServiceProtocol {
    func get<T>(_ key: String, type: T.Type) -> T?
    func set<T>(_ key: String, value: T)
    func remove(_ key: String)
}

/**
 * User service protocol
 */
protocol UserServiceProtocol {
    func getUser(id: UUID) async throws -> User?
    func createUser(_ user: User) async throws -> User
    func updateUser(_ user: User) async throws -> User
    func deleteUser(id: UUID) async throws
}

// MARK: - Service Implementations

/**
 * Network service implementation
 */
class NetworkService: NetworkServiceProtocol {
    private let baseURL: URL
    private let session: URLSession
    
    init(baseURL: URL, configuration: URLSessionConfiguration = .default) {
        self.baseURL = baseURL
        self.session = URLSession(configuration: configuration)
    }
    
    func getData(from url: URL) async throws -> Data {
        let (data, _) = try await session.data(from: url)
        return data
    }
}

/**
 * Cache service implementation
 */
class CacheService: CacheServiceProtocol {
    private var cache: [String: Any] = [:]
    private let lock = NSLock()
    
    func get<T>(_ key: String, type: T.Type) -> T? {
        lock.lock()
        defer { lock.unlock() }
        return cache[key] as? T
    }
    
    func set<T>(_ key: String, value: T) {
        lock.lock()
        defer { lock.unlock() }
        cache[key] = value
    }
    
    func remove(_ key: String) {
        lock.lock()
        defer { lock.unlock() }
        cache.removeValue(forKey: key)
    }
}

/**
 * User service implementation
 */
class UserService: UserServiceProtocol {
    private let networkService: NetworkServiceProtocol
    private let cacheService: CacheServiceProtocol
    
    init(networkService: NetworkServiceProtocol, cacheService: CacheServiceProtocol) {
        self.networkService = networkService
        self.cacheService = cacheService
    }
    
    func getUser(id: UUID) async throws -> User? {
        // Check cache first
        if let cachedUser = cacheService.get("user_\(id)", type: User.self) {
            return cachedUser
        }
        
        // Fetch from network
        let url = URL(string: "https://api.example.com/users/\(id)")!
        let data = try await networkService.getData(from: url)
        let user = try JSONDecoder().decode(User.self, from: data)
        
        // Cache the result
        cacheService.set("user_\(id)", value: user)
        
        return user
    }
    
    func createUser(_ user: User) async throws -> User {
        // Implementation would create user via network
        return user
    }
    
    func updateUser(_ user: User) async throws -> User {
        // Implementation would update user via network
        return user
    }
    
    func deleteUser(id: UUID) async throws {
        // Implementation would delete user via network
    }
}

// MARK: - Supporting Types

/**
 * User model for demonstration
 */
struct User: Codable, Identifiable {
    let id: UUID
    let name: String
    let email: String
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use dependency injection patterns
 * 
 * This function shows practical usage of all the DI components
 */
func demonstrateDependencyInjection() {
    print("=== Dependency Injection Demonstration ===\n")
    
    // Create and configure container
    let container = DIContainer()
    
    // Register services
    container.registerSingleton(NetworkServiceProtocol.self) {
        NetworkService(baseURL: URL(string: "https://api.example.com")!)
    }
    
    container.registerSingleton(CacheServiceProtocol.self) {
        CacheService()
    }
    
    container.registerTransient(UserServiceProtocol.self) {
        UserService(
            networkService: container.resolve(NetworkServiceProtocol.self),
            cacheService: container.resolve(CacheServiceProtocol.self)
        )
    }
    
    // Set global container
    ServiceLocator.setContainer(container)
    
    print("--- Service Registration ---")
    print("Services registered successfully")
    
    // Resolve services
    let networkService = container.resolve(NetworkServiceProtocol.self)
    let cacheService = container.resolve(CacheServiceProtocol.self)
    let userService = container.resolve(UserServiceProtocol.self)
    
    print("\n--- Service Resolution ---")
    print("Network service: \(type(of: networkService))")
    print("Cache service: \(type(of: cacheService))")
    print("User service: \(type(of: userService))")
    
    // Demonstrate property wrapper injection
    class ExampleViewController {
        @Injected(NetworkServiceProtocol.self) var networkService
        @InjectedOptional(CacheServiceProtocol.self) var cacheService
        
        func doSomething() {
            print("Network service: \(type(of: networkService))")
            print("Cache service: \(type(of: cacheService))")
        }
    }
    
    let viewController = ExampleViewController()
    viewController.doSomething()
    
    // Demonstrate factory pattern
    let networkFactory = NetworkServiceFactory(baseURL: URL(string: "https://api.example.com")!)
    let createdNetworkService = networkFactory.create()
    print("\n--- Factory Pattern ---")
    print("Created network service: \(type(of: createdNetworkService))")
    
    // Demonstrate singleton management
    let singleton = SingletonManager.getInstance(NetworkServiceProtocol.self) {
        NetworkService(baseURL: URL(string: "https://api.example.com")!)
    }
    print("\n--- Singleton Management ---")
    print("Singleton instance: \(type(of: singleton))")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateDependencyInjection()
