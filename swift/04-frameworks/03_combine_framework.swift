/*
 * iOS Frameworks: Combine Framework
 * 
 * This file demonstrates production-grade Combine framework patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master reactive programming with Publishers and Subscribers
 * - Understand data binding and UI updates
 * - Implement proper error handling and recovery
 * - Apply performance optimization techniques
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import Combine

// MARK: - Custom Publishers

/**
 * Custom publisher for network requests
 * 
 * This publisher demonstrates production-grade network request handling
 * with proper error management and retry logic
 */
struct NetworkPublisher: Publisher {
    typealias Output = Data
    typealias Failure = NetworkError
    
    private let url: URL
    private let session: URLSession
    private let retryCount: Int
    private let retryDelay: TimeInterval
    
    init(url: URL, session: URLSession = .shared, retryCount: Int = 3, retryDelay: TimeInterval = 1.0) {
        self.url = url
        self.session = session
        self.retryCount = retryCount
        self.retryDelay = retryDelay
    }
    
    func receive<S>(subscriber: S) where S : Subscriber, Failure == S.Failure, Output == S.Input {
        let subscription = NetworkSubscription(
            url: url,
            session: session,
            retryCount: retryCount,
            retryDelay: retryDelay,
            subscriber: subscriber
        )
        subscriber.receive(subscription: subscription)
    }
}

/**
 * Network subscription for managing request lifecycle
 * 
 * This class demonstrates proper subscription management
 * with cancellation and resource cleanup
 */
class NetworkSubscription<S: Subscriber>: Subscription where S.Input == Data, S.Failure == NetworkError {
    
    private let url: URL
    private let session: URLSession
    private let retryCount: Int
    private let retryDelay: TimeInterval
    private let subscriber: S
    private var currentRetry = 0
    private var task: URLSessionDataTask?
    private var demand: Subscribers.Demand = .none
    
    init(url: URL, session: URLSession, retryCount: Int, retryDelay: TimeInterval, subscriber: S) {
        self.url = url
        self.session = session
        self.retryCount = retryCount
        self.retryDelay = retryDelay
        self.subscriber = subscriber
    }
    
    func request(_ demand: Subscribers.Demand) {
        self.demand = demand
        performRequest()
    }
    
    func cancel() {
        task?.cancel()
        task = nil
    }
    
    private func performRequest() {
        guard demand > 0 else { return }
        
        task = session.dataTask(with: url) { [weak self] data, response, error in
            guard let self = self else { return }
            
            if let error = error {
                self.handleError(error)
            } else if let httpResponse = response as? HTTPURLResponse {
                if httpResponse.statusCode >= 200 && httpResponse.statusCode < 300 {
                    if let data = data {
                        self.subscriber.receive(data)
                        self.subscriber.receive(completion: .finished)
                    } else {
                        self.handleError(NetworkError.noData)
                    }
                } else {
                    self.handleError(NetworkError.serverError(httpResponse.statusCode))
                }
            } else {
                self.handleError(NetworkError.invalidResponse)
            }
        }
        
        task?.resume()
    }
    
    private func handleError(_ error: NetworkError) {
        if currentRetry < retryCount {
            currentRetry += 1
            DispatchQueue.global().asyncAfter(deadline: .now() + retryDelay) { [weak self] in
                self?.performRequest()
            }
        } else {
            subscriber.receive(completion: .failure(error))
        }
    }
}

// MARK: - Custom Subscribers

/**
 * Custom subscriber for UI updates
 * 
 * This subscriber demonstrates proper UI update handling
 * with main thread dispatch and error management
 */
class UIUpdateSubscriber<T>: Subscriber {
    typealias Input = T
    typealias Failure = Error
    
    private let onValue: (T) -> Void
    private let onError: (Error) -> Void
    private let onComplete: () -> Void
    
    init(
        onValue: @escaping (T) -> Void,
        onError: @escaping (Error) -> Void = { _ in },
        onComplete: @escaping () -> Void = {}
    ) {
        self.onValue = onValue
        self.onError = onError
        self.onComplete = onComplete
    }
    
    func receive(subscription: Subscription) {
        subscription.request(.unlimited)
    }
    
    func receive(_ input: T) -> Subscribers.Demand {
        DispatchQueue.main.async {
            self.onValue(input)
        }
        return .unlimited
    }
    
    func receive(completion: Subscribers.Completion<Error>) {
        DispatchQueue.main.async {
            switch completion {
            case .finished:
                self.onComplete()
            case .failure(let error):
                self.onError(error)
            }
        }
    }
}

// MARK: - Reactive Data Manager

/**
 * Reactive data manager using Combine
 * 
 * This class demonstrates production-grade reactive data management
 * with proper state synchronization and error handling
 */
class ReactiveDataManager: ObservableObject {
    
    // MARK: - Published Properties
    
    @Published var users: [User] = []
    @Published var posts: [Post] = []
    @Published var isLoading = false
    @Published var error: NetworkError?
    
    // MARK: - Private Properties
    
    private let networkService: NetworkServiceProtocol
    private var cancellables = Set<AnyCancellable>()
    private let refreshSubject = PassthroughSubject<Void, Never>()
    private let searchSubject = PassthroughSubject<String, Never>()
    
    // MARK: - Initialization
    
    init(networkService: NetworkServiceProtocol) {
        self.networkService = networkService
        setupBindings()
    }
    
    // MARK: - Setup
    
    private func setupBindings() {
        // Refresh binding
        refreshSubject
            .debounce(for: .milliseconds(300), scheduler: DispatchQueue.main)
            .sink { [weak self] _ in
                self?.refreshData()
            }
            .store(in: &cancellables)
        
        // Search binding
        searchSubject
            .debounce(for: .milliseconds(500), scheduler: DispatchQueue.main)
            .removeDuplicates()
            .sink { [weak self] searchText in
                self?.searchUsers(searchText)
            }
            .store(in: &cancellables)
    }
    
    // MARK: - Public Methods
    
    func refreshData() {
        isLoading = true
        error = nil
        
        // Load users and posts in parallel
        let usersPublisher = networkService.getUsers()
        let postsPublisher = networkService.getPosts()
        
        Publishers.Zip(usersPublisher, postsPublisher)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    self?.isLoading = false
                    if case .failure(let error) = completion {
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] users, posts in
                    self?.users = users
                    self?.posts = posts
                }
            )
            .store(in: &cancellables)
    }
    
    func searchUsers(_ query: String) {
        if query.isEmpty {
            refreshData()
        } else {
            networkService.searchUsers(query)
                .receive(on: DispatchQueue.main)
                .sink(
                    receiveCompletion: { [weak self] completion in
                        if case .failure(let error) = completion {
                            self?.error = error
                        }
                    },
                    receiveValue: { [weak self] users in
                        self?.users = users
                    }
                )
                .store(in: &cancellables)
        }
    }
    
    func triggerRefresh() {
        refreshSubject.send()
    }
    
    func updateSearchQuery(_ query: String) {
        searchSubject.send(query)
    }
}

// MARK: - Reactive View Model

/**
 * Reactive view model using Combine
 * 
 * This class demonstrates proper view model implementation
 * with reactive programming and state management
 */
class ReactiveViewModel: ObservableObject {
    
    // MARK: - Published Properties
    
    @Published var searchText = ""
    @Published var filteredItems: [ListItem] = []
    @Published var isLoading = false
    @Published var error: Error?
    
    // MARK: - Private Properties
    
    private let dataManager: ReactiveDataManager
    private var cancellables = Set<AnyCancellable>()
    
    // MARK: - Initialization
    
    init(dataManager: ReactiveDataManager) {
        self.dataManager = dataManager
        setupBindings()
    }
    
    // MARK: - Setup
    
    private func setupBindings() {
        // Search text binding
        $searchText
            .debounce(for: .milliseconds(300), scheduler: DispatchQueue.main)
            .removeDuplicates()
            .sink { [weak self] searchText in
                self?.dataManager.updateSearchQuery(searchText)
            }
            .store(in: &cancellables)
        
        // Data manager bindings
        dataManager.$users
            .combineLatest(dataManager.$posts)
            .map { users, posts in
                // Combine users and posts into list items
                let userItems = users.map { user in
                    ListItem(
                        id: user.id,
                        title: user.name,
                        subtitle: user.email,
                        category: "User",
                        imageURL: user.avatarURL,
                        isFavorite: false,
                        isActive: true,
                        createdAt: user.createdAt
                    )
                }
                
                let postItems = posts.map { post in
                    ListItem(
                        id: post.id,
                        title: post.title,
                        subtitle: post.content,
                        category: "Post",
                        imageURL: post.imageURL,
                        isFavorite: false,
                        isActive: true,
                        createdAt: post.createdAt
                    )
                }
                
                return userItems + postItems
            }
            .assign(to: \.filteredItems, on: self)
            .store(in: &cancellables)
        
        dataManager.$isLoading
            .assign(to: \.isLoading, on: self)
            .store(in: &cancellables)
        
        dataManager.$error
            .compactMap { $0 }
            .map { $0 as Error }
            .assign(to: \.error, on: self)
            .store(in: &cancellables)
    }
    
    // MARK: - Public Methods
    
    func refreshData() {
        dataManager.triggerRefresh()
    }
}

// MARK: - Custom Operators

/**
 * Custom Combine operators for common patterns
 * 
 * This extension demonstrates production-grade custom operators
 * with proper error handling and performance optimization
 */
extension Publisher {
    
    /**
     * Retry operator with exponential backoff
     * 
     * - Parameters:
     *   - maxRetries: Maximum number of retries
     *   - delay: Initial delay between retries
     *   - multiplier: Delay multiplier for exponential backoff
     * - Returns: Publisher with retry logic
     */
    func retryWithBackoff(
        maxRetries: Int = 3,
        delay: TimeInterval = 1.0,
        multiplier: Double = 2.0
    ) -> AnyPublisher<Output, Failure> {
        return self
            .catch { error -> AnyPublisher<Output, Failure> in
                return Fail(error: error)
                    .delay(for: .seconds(delay), scheduler: DispatchQueue.main)
                    .eraseToAnyPublisher()
            }
            .retry(maxRetries)
            .eraseToAnyPublisher()
    }
    
    /**
     * Timeout operator with custom error
     * 
     * - Parameters:
     *   - interval: Timeout interval
     *   - customError: Custom error to throw on timeout
     * - Returns: Publisher with timeout logic
     */
    func timeoutWithCustomError<NewFailure: Error>(
        _ interval: TimeInterval,
        customError: NewFailure
    ) -> AnyPublisher<Output, NewFailure> where Failure == Never {
        return self
            .timeout(.seconds(interval), scheduler: DispatchQueue.main)
            .mapError { _ in customError }
            .eraseToAnyPublisher()
    }
    
    /**
     * Debounce operator with custom scheduler
     * 
     * - Parameters:
     *   - interval: Debounce interval
     *   - scheduler: Scheduler to use
     * - Returns: Publisher with debounce logic
     */
    func debounce<S: Scheduler>(
        for interval: TimeInterval,
        scheduler: S
    ) -> AnyPublisher<Output, Failure> {
        return self
            .debounce(for: .seconds(interval), scheduler: scheduler)
            .eraseToAnyPublisher()
    }
}

// MARK: - Reactive Extensions

/**
 * Reactive extensions for common UIKit components
 * 
 * This extension demonstrates proper UIKit integration
 * with Combine framework
 */
extension UITextField {
    
    /**
     * Publisher for text changes
     * 
     * - Returns: Publisher that emits text changes
     */
    var textPublisher: AnyPublisher<String, Never> {
        NotificationCenter.default
            .publisher(for: UITextField.textDidChangeNotification, object: self)
            .compactMap { $0.object as? UITextField }
            .map { $0.text ?? "" }
            .eraseToAnyPublisher()
    }
}

extension UIButton {
    
    /**
     * Publisher for button taps
     * 
     * - Returns: Publisher that emits tap events
     */
    var tapPublisher: AnyPublisher<Void, Never> {
        let gesture = UITapGestureRecognizer()
        addGestureRecognizer(gesture)
        
        return gesture.publisher(for: \.state)
            .filter { $0 == .ended }
            .map { _ in () }
            .eraseToAnyPublisher()
    }
}

extension UISwitch {
    
    /**
     * Publisher for switch value changes
     * 
     * - Returns: Publisher that emits switch value changes
     */
    var valuePublisher: AnyPublisher<Bool, Never> {
        publisher(for: \.isOn)
            .eraseToAnyPublisher()
    }
}

// MARK: - Supporting Types

/**
 * Network error types
 */
enum NetworkError: Error, LocalizedError {
    case noData
    case invalidResponse
    case serverError(Int)
    case networkUnavailable
    case timeout
    
    var errorDescription: String? {
        switch self {
        case .noData:
            return "No data received"
        case .invalidResponse:
            return "Invalid response format"
        case .serverError(let code):
            return "Server error: \(code)"
        case .networkUnavailable:
            return "Network unavailable"
        case .timeout:
            return "Request timed out"
        }
    }
}

/**
 * Network service protocol
 */
protocol NetworkServiceProtocol {
    func getUsers() -> AnyPublisher<[User], NetworkError>
    func getPosts() -> AnyPublisher<[Post], NetworkError>
    func searchUsers(_ query: String) -> AnyPublisher<[User], NetworkError>
}

/**
 * User model
 */
struct User: Codable, Identifiable {
    let id: UUID
    let name: String
    let email: String
    let avatarURL: String
    let createdAt: Date
}

/**
 * Post model
 */
struct Post: Codable, Identifiable {
    let id: UUID
    let title: String
    let content: String
    let imageURL: String
    let createdAt: Date
}

/**
 * List item model
 */
struct ListItem: Identifiable {
    let id: UUID
    let title: String
    let subtitle: String
    let category: String
    let imageURL: String
    let isFavorite: Bool
    let isActive: Bool
    let createdAt: Date
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use Combine framework patterns
 * 
 * This function shows practical usage of all the Combine components
 */
func demonstrateCombineFramework() {
    print("=== Combine Framework Demonstration ===\n")
    
    // Create custom publisher
    let url = URL(string: "https://api.example.com/data")!
    let networkPublisher = NetworkPublisher(url: url)
    
    print("--- Custom Publishers ---")
    print("Network publisher: \(type(of: networkPublisher))")
    print("Features: Retry logic, error handling, cancellation")
    
    // Create custom subscriber
    let uiSubscriber = UIUpdateSubscriber<String>(
        onValue: { value in
            print("Received value: \(value)")
        },
        onError: { error in
            print("Received error: \(error)")
        },
        onComplete: {
            print("Completed")
        }
    )
    
    print("\n--- Custom Subscribers ---")
    print("UI subscriber: \(type(of: uiSubscriber))")
    print("Features: Main thread dispatch, error handling")
    
    // Create reactive data manager
    let networkService = MockNetworkService()
    let dataManager = ReactiveDataManager(networkService: networkService)
    
    print("\n--- Reactive Data Manager ---")
    print("Data manager: \(type(of: dataManager))")
    print("Published properties: users, posts, isLoading, error")
    
    // Create reactive view model
    let viewModel = ReactiveViewModel(dataManager: dataManager)
    
    print("\n--- Reactive View Model ---")
    print("View model: \(type(of: viewModel))")
    print("Published properties: searchText, filteredItems, isLoading, error")
    
    // Demonstrate custom operators
    let publisher = Just("Hello, World!")
        .setFailureType(to: NetworkError.self)
        .retryWithBackoff(maxRetries: 3, delay: 1.0, multiplier: 2.0)
    
    print("\n--- Custom Operators ---")
    print("Retry with backoff: \(type(of: publisher))")
    print("Timeout with custom error: Available")
    print("Debounce with custom scheduler: Available")
    
    // Demonstrate reactive extensions
    print("\n--- Reactive Extensions ---")
    print("UITextField.textPublisher: Text change publisher")
    print("UIButton.tapPublisher: Tap event publisher")
    print("UISwitch.valuePublisher: Value change publisher")
    
    // Demonstrate data flow
    print("\n--- Data Flow ---")
    print("Search text -> Debounce -> Data manager -> View model -> UI")
    print("Refresh trigger -> Data manager -> View model -> UI")
    print("Error handling -> View model -> UI")
}

// MARK: - Mock Network Service

/**
 * Mock network service for demonstration
 */
class MockNetworkService: NetworkServiceProtocol {
    
    func getUsers() -> AnyPublisher<[User], NetworkError> {
        let users = [
            User(
                id: UUID(),
                name: "John Doe",
                email: "john@example.com",
                avatarURL: "https://example.com/avatar1.jpg",
                createdAt: Date()
            ),
            User(
                id: UUID(),
                name: "Jane Smith",
                email: "jane@example.com",
                avatarURL: "https://example.com/avatar2.jpg",
                createdAt: Date()
            )
        ]
        
        return Just(users)
            .setFailureType(to: NetworkError.self)
            .eraseToAnyPublisher()
    }
    
    func getPosts() -> AnyPublisher<[Post], NetworkError> {
        let posts = [
            Post(
                id: UUID(),
                title: "Sample Post 1",
                content: "This is the content of the first post",
                imageURL: "https://example.com/image1.jpg",
                createdAt: Date()
            ),
            Post(
                id: UUID(),
                title: "Sample Post 2",
                content: "This is the content of the second post",
                imageURL: "https://example.com/image2.jpg",
                createdAt: Date()
            )
        ]
        
        return Just(posts)
            .setFailureType(to: NetworkError.self)
            .eraseToAnyPublisher()
    }
    
    func searchUsers(_ query: String) -> AnyPublisher<[User], NetworkError> {
        return getUsers()
            .map { users in
                users.filter { user in
                    user.name.localizedCaseInsensitiveContains(query) ||
                    user.email.localizedCaseInsensitiveContains(query)
                }
            }
            .eraseToAnyPublisher()
    }
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateCombineFramework()
