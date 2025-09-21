/*
 * Design Patterns: Observer Pattern
 * 
 * This file demonstrates production-grade Observer pattern implementation in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master reactive programming and event-driven architecture
 * - Understand notification center and custom observers
 * - Implement proper memory management and lifecycle
 * - Apply Combine framework and reactive patterns
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import Combine

// MARK: - Observer Protocol

/**
 * Base observer protocol defining common observer behavior
 * 
 * This protocol demonstrates the core Observer pattern interface
 * with proper lifecycle management and event handling
 */
protocol Observer: AnyObject {
    associatedtype Event
    
    func update(with event: Event)
}

/**
 * Observable protocol defining common observable behavior
 * 
 * This protocol demonstrates the core Observable pattern interface
 * with proper observer management and event broadcasting
 */
protocol Observable {
    associatedtype Event
    associatedtype ObserverType: Observer where ObserverType.Event == Event
    
    func addObserver(_ observer: ObserverType)
    func removeObserver(_ observer: ObserverType)
    func notifyObservers(with event: Event)
}

// MARK: - Generic Observer Implementation

/**
 * Generic observer implementation with proper memory management
 * 
 * This class demonstrates proper observer implementation
 * with weak references and automatic cleanup
 */
class GenericObserver<Event>: Observer {
    private let handler: (Event) -> Void
    
    init(handler: @escaping (Event) -> Void) {
        self.handler = handler
    }
    
    func update(with event: Event) {
        handler(event)
    }
}

/**
 * Generic observable implementation with proper observer management
 * 
 * This class demonstrates proper observable implementation
 * with weak references and automatic cleanup
 */
class GenericObservable<Event>: Observable {
    typealias ObserverType = GenericObserver<Event>
    
    private var observers: [WeakObserver<Event>] = []
    private let lock = NSLock()
    
    func addObserver(_ observer: ObserverType) {
        lock.lock()
        defer { lock.unlock() }
        
        // Remove any existing weak references to the same observer
        observers.removeAll { $0.observer === observer }
        
        // Add new weak reference
        observers.append(WeakObserver(observer: observer))
    }
    
    func removeObserver(_ observer: ObserverType) {
        lock.lock()
        defer { lock.unlock() }
        
        observers.removeAll { $0.observer === observer }
    }
    
    func notifyObservers(with event: Event) {
        lock.lock()
        defer { lock.unlock() }
        
        // Clean up nil references
        observers.removeAll { $0.observer == nil }
        
        // Notify all observers
        observers.forEach { weakObserver in
            weakObserver.observer?.update(with: event)
        }
    }
}

/**
 * Weak observer wrapper for proper memory management
 * 
 * This class demonstrates proper weak reference handling
 * to prevent retain cycles and memory leaks
 */
private class WeakObserver<Event> {
    weak var observer: GenericObserver<Event>?
    
    init(observer: GenericObserver<Event>) {
        self.observer = observer
    }
}

// MARK: - Event Types

/**
 * User events for demonstration
 * 
 * This enum demonstrates proper event modeling
 * with associated values and type safety
 */
enum UserEvent {
    case userLoggedIn(User)
    case userLoggedOut
    case userProfileUpdated(User)
    case userSettingsChanged(UserSettings)
    case userDataSynced
}

/**
 * User settings model
 */
struct UserSettings: Codable {
    let notificationsEnabled: Bool
    let darkModeEnabled: Bool
    let language: String
    let timezone: String
}

/**
 * User model
 */
struct User: Codable, Identifiable {
    let id: UUID
    let username: String
    let email: String
    let firstName: String
    let lastName: String
    let createdAt: Date
    
    var fullName: String {
        return "\(firstName) \(lastName)"
    }
}

// MARK: - User Service with Observer Pattern

/**
 * User service implementing observer pattern
 * 
 * This class demonstrates proper observer pattern implementation
 * with event broadcasting and state management
 */
class UserService: Observable {
    typealias Event = UserEvent
    typealias ObserverType = GenericObserver<UserEvent>
    
    private var observers: [WeakObserver<UserEvent>] = []
    private let lock = NSLock()
    private var currentUser: User?
    private var userSettings: UserSettings?
    
    init() {
        // Initialize with default settings
        userSettings = UserSettings(
            notificationsEnabled: true,
            darkModeEnabled: false,
            language: "en",
            timezone: "UTC"
        )
    }
    
    // MARK: - Observer Management
    
    func addObserver(_ observer: ObserverType) {
        lock.lock()
        defer { lock.unlock() }
        
        observers.removeAll { $0.observer === observer }
        observers.append(WeakObserver(observer: observer))
    }
    
    func removeObserver(_ observer: ObserverType) {
        lock.lock()
        defer { lock.unlock() }
        
        observers.removeAll { $0.observer === observer }
    }
    
    func notifyObservers(with event: UserEvent) {
        lock.lock()
        defer { lock.unlock() }
        
        observers.removeAll { $0.observer == nil }
        observers.forEach { weakObserver in
            weakObserver.observer?.update(with: event)
        }
    }
    
    // MARK: - User Operations
    
    func login(username: String, password: String) async throws {
        // Simulate login process
        try await Task.sleep(nanoseconds: 1_000_000_000) // 1 second
        
        let user = User(
            id: UUID(),
            username: username,
            email: "\(username)@example.com",
            firstName: "John",
            lastName: "Doe",
            createdAt: Date()
        )
        
        currentUser = user
        notifyObservers(with: .userLoggedIn(user))
    }
    
    func logout() {
        currentUser = nil
        notifyObservers(with: .userLoggedOut)
    }
    
    func updateProfile(_ user: User) {
        currentUser = user
        notifyObservers(with: .userProfileUpdated(user))
    }
    
    func updateSettings(_ settings: UserSettings) {
        userSettings = settings
        if let user = currentUser {
            notifyObservers(with: .userSettingsChanged(settings))
        }
    }
    
    func syncUserData() async {
        // Simulate data sync
        try? await Task.sleep(nanoseconds: 500_000_000) // 0.5 seconds
        notifyObservers(with: .userDataSynced)
    }
    
    // MARK: - Getters
    
    var isLoggedIn: Bool {
        return currentUser != nil
    }
    
    var loggedInUser: User? {
        return currentUser
    }
    
    var settings: UserSettings? {
        return userSettings
    }
}

// MARK: - Notification Center Implementation

/**
 * Custom notification center implementation
 * 
 * This class demonstrates proper notification center implementation
 * with type safety and performance optimization
 */
class CustomNotificationCenter {
    
    private var observers: [String: [WeakObserver<Any>]] = [:]
    private let lock = NSLock()
    
    static let shared = CustomNotificationCenter()
    
    private init() {}
    
    /**
     * Adds an observer for a specific notification
     * 
     * - Parameters:
     *   - observer: Observer to add
     *   - name: Notification name
     *   - handler: Handler closure
     */
    func addObserver<T>(
        for name: String,
        observer: AnyObject,
        handler: @escaping (T) -> Void
    ) {
        let genericObserver = GenericObserver<T> { handler($0) }
        let weakObserver = WeakObserver(observer: genericObserver)
        
        lock.lock()
        defer { lock.unlock() }
        
        if observers[name] == nil {
            observers[name] = []
        }
        
        observers[name]?.append(weakObserver)
    }
    
    /**
     * Removes an observer for a specific notification
     * 
     * - Parameters:
     *   - observer: Observer to remove
     *   - name: Notification name
     */
    func removeObserver(_ observer: AnyObject, for name: String) {
        lock.lock()
        defer { lock.unlock() }
        
        observers[name]?.removeAll { $0.observer === observer }
    }
    
    /**
     * Posts a notification
     * 
     * - Parameters:
     *   - name: Notification name
     *   - object: Notification object
     */
    func post<T>(name: String, object: T) {
        lock.lock()
        defer { lock.unlock() }
        
        guard let notificationObservers = observers[name] else { return }
        
        // Clean up nil references
        observers[name] = notificationObservers.filter { $0.observer != nil }
        
        // Notify all observers
        notificationObservers.forEach { weakObserver in
            if let genericObserver = weakObserver.observer as? GenericObserver<T> {
                genericObserver.update(with: object)
            }
        }
    }
}

// MARK: - Combine-based Observer Pattern

/**
 * Combine-based observer implementation
 * 
 * This class demonstrates modern reactive programming
 * with Combine framework and proper memory management
 */
class CombineObserverPattern {
    
    private let userSubject = PassthroughSubject<UserEvent, Never>()
    private var cancellables = Set<AnyCancellable>()
    
    /**
     * User event publisher
     * 
     * - Returns: Publisher for user events
     */
    var userEventPublisher: AnyPublisher<UserEvent, Never> {
        return userSubject.eraseToAnyPublisher()
    }
    
    /**
     * Publishes a user event
     * 
     * - Parameter event: Event to publish
     */
    func publishEvent(_ event: UserEvent) {
        userSubject.send(event)
    }
    
    /**
     * Subscribes to user events
     * 
     * - Parameter handler: Event handler closure
     * - Returns: Cancellable for subscription management
     */
    func subscribeToUserEvents(handler: @escaping (UserEvent) -> Void) -> AnyCancellable {
        return userEventPublisher
            .sink(receiveValue: handler)
    }
    
    /**
     * Subscribes to specific user events
     * 
     * - Parameter eventType: Type of event to subscribe to
     * - Parameter handler: Event handler closure
     * - Returns: Cancellable for subscription management
     */
    func subscribeToEvent<T>(
        _ eventType: T.Type,
        handler: @escaping (T) -> Void
    ) -> AnyCancellable {
        return userEventPublisher
            .compactMap { event in
                if case let eventType(event) = event {
                    return event
                }
                return nil
            }
            .sink(receiveValue: handler)
    }
}

// MARK: - Reactive View Model

/**
 * Reactive view model using observer pattern
 * 
 * This class demonstrates proper view model implementation
 * with reactive programming and state management
 */
class ReactiveUserViewModel: ObservableObject {
    
    @Published var currentUser: User?
    @Published var userSettings: UserSettings?
    @Published var isLoggedIn: Bool = false
    @Published var isLoading: Bool = false
    @Published var error: Error?
    
    private let userService: UserService
    private let combineObserver: CombineObserverPattern
    private var cancellables = Set<AnyCancellable>()
    
    init(userService: UserService, combineObserver: CombineObserverPattern) {
        self.userService = userService
        self.combineObserver = combineObserver
        
        setupBindings()
    }
    
    private func setupBindings() {
        // Subscribe to user events
        combineObserver.subscribeToUserEvents { [weak self] event in
            DispatchQueue.main.async {
                self?.handleUserEvent(event)
            }
        }
        .store(in: &cancellables)
    }
    
    private func handleUserEvent(_ event: UserEvent) {
        switch event {
        case .userLoggedIn(let user):
            currentUser = user
            isLoggedIn = true
            error = nil
        case .userLoggedOut:
            currentUser = nil
            isLoggedIn = false
        case .userProfileUpdated(let user):
            currentUser = user
        case .userSettingsChanged(let settings):
            userSettings = settings
        case .userDataSynced:
            // Handle data sync completion
            break
        }
    }
    
    func login(username: String, password: String) {
        isLoading = true
        error = nil
        
        Task {
            do {
                try await userService.login(username: username, password: password)
                await MainActor.run {
                    self.isLoading = false
                }
            } catch {
                await MainActor.run {
                    self.isLoading = false
                    self.error = error
                }
            }
        }
    }
    
    func logout() {
        userService.logout()
    }
    
    func updateProfile(_ user: User) {
        userService.updateProfile(user)
    }
    
    func updateSettings(_ settings: UserSettings) {
        userService.updateSettings(settings)
    }
    
    func syncUserData() {
        Task {
            await userService.syncUserData()
        }
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use the Observer pattern
 * 
 * This function shows practical usage of all the observer components
 */
func demonstrateObserverPattern() {
    print("=== Observer Pattern Demonstration ===\n")
    
    // Create services
    let userService = UserService()
    let combineObserver = CombineObserverPattern()
    
    // Create view model
    let viewModel = ReactiveUserViewModel(
        userService: userService,
        combineObserver: combineObserver
    )
    
    print("--- Generic Observer Pattern ---")
    
    // Create observers
    let loginObserver = GenericObserver<UserEvent> { event in
        switch event {
        case .userLoggedIn(let user):
            print("Observer 1: User logged in: \(user.fullName)")
        case .userLoggedOut:
            print("Observer 1: User logged out")
        case .userProfileUpdated(let user):
            print("Observer 1: Profile updated: \(user.fullName)")
        case .userSettingsChanged(let settings):
            print("Observer 1: Settings changed: \(settings.language)")
        case .userDataSynced:
            print("Observer 1: Data synced")
        }
    }
    
    let settingsObserver = GenericObserver<UserEvent> { event in
        if case .userSettingsChanged(let settings) = event {
            print("Observer 2: Settings updated - Dark mode: \(settings.darkModeEnabled)")
        }
    }
    
    // Add observers to user service
    userService.addObserver(loginObserver)
    userService.addObserver(settingsObserver)
    
    // Simulate user actions
    Task {
        try await userService.login(username: "johndoe", password: "password")
        
        let updatedUser = User(
            id: UUID(),
            username: "johndoe",
            email: "john@example.com",
            firstName: "John",
            lastName: "Smith",
            createdAt: Date()
        )
        userService.updateProfile(updatedUser)
        
        let newSettings = UserSettings(
            notificationsEnabled: true,
            darkModeEnabled: true,
            language: "es",
            timezone: "EST"
        )
        userService.updateSettings(newSettings)
        
        await userService.syncUserData()
        
        userService.logout()
    }
    
    print("\n--- Custom Notification Center ---")
    
    // Create notification center observers
    CustomNotificationCenter.shared.addObserver(
        for: "UserEvent",
        observer: self
    ) { (event: UserEvent) in
        print("Notification Center: Received event: \(event)")
    }
    
    // Post notifications
    CustomNotificationCenter.shared.post(
        name: "UserEvent",
        object: UserEvent.userLoggedIn(User(
            id: UUID(),
            username: "testuser",
            email: "test@example.com",
            firstName: "Test",
            lastName: "User",
            createdAt: Date()
        ))
    )
    
    print("\n--- Combine Observer Pattern ---")
    
    // Subscribe to combine events
    combineObserver.subscribeToUserEvents { event in
        print("Combine Observer: \(event)")
    }
    .store(in: &Set<AnyCancellable>())
    
    // Publish events
    combineObserver.publishEvent(.userLoggedIn(User(
        id: UUID(),
        username: "combineuser",
        email: "combine@example.com",
        firstName: "Combine",
        lastName: "User",
        createdAt: Date()
    )))
    
    print("\n--- Reactive View Model ---")
    
    // Demonstrate view model usage
    print("Initial state - Logged in: \(viewModel.isLoggedIn)")
    
    // Simulate login
    viewModel.login(username: "viewmodeluser", password: "password")
    
    // Wait a moment for async operations
    DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) {
        print("After login - Logged in: \(viewModel.isLoggedIn)")
        print("Current user: \(viewModel.currentUser?.fullName ?? "None")")
    }
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateObserverPattern()
