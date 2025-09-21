/*
 * Design Patterns: MVVM (Model-View-ViewModel)
 * 
 * This file demonstrates production-grade MVVM architecture patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master MVVM architecture fundamentals
 * - Understand reactive bindings and data flow
 * - Implement proper separation of concerns
 * - Apply dependency injection and testing patterns
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import Combine

// MARK: - Model Layer

/**
 * User model representing user data
 * 
 * This struct demonstrates the Model layer in MVVM architecture
 * with proper data modeling and validation
 */
struct User: Codable, Equatable, Identifiable {
    let id: UUID
    let username: String
    let email: String
    let firstName: String
    let lastName: String
    let profileImageURL: String?
    let isVerified: Bool
    let createdAt: Date
    let lastLoginAt: Date?
    
    init(
        id: UUID = UUID(),
        username: String,
        email: String,
        firstName: String,
        lastName: String,
        profileImageURL: String? = nil,
        isVerified: Bool = false,
        createdAt: Date = Date(),
        lastLoginAt: Date? = nil
    ) {
        self.id = id
        self.username = username
        self.email = email
        self.firstName = firstName
        self.lastName = lastName
        self.profileImageURL = profileImageURL
        self.isVerified = isVerified
        self.createdAt = createdAt
        self.lastLoginAt = lastLoginAt
    }
    
    var fullName: String {
        return "\(firstName) \(lastName)"
    }
    
    var displayName: String {
        return isVerified ? "✓ \(username)" : username
    }
}

/**
 * User profile model with additional profile information
 * 
 * This struct demonstrates extended model data with computed properties
 */
struct UserProfile: Codable, Equatable {
    let user: User
    let bio: String?
    let location: String?
    let website: String?
    let followersCount: Int
    let followingCount: Int
    let postsCount: Int
    let isFollowing: Bool
    let isBlocked: Bool
    
    init(
        user: User,
        bio: String? = nil,
        location: String? = nil,
        website: String? = nil,
        followersCount: Int = 0,
        followingCount: Int = 0,
        postsCount: Int = 0,
        isFollowing: Bool = false,
        isBlocked: Bool = false
    ) {
        self.user = user
        self.bio = bio
        self.location = location
        self.website = website
        self.followersCount = followersCount
        self.followingCount = followingCount
        self.postsCount = postsCount
        self.isFollowing = isFollowing
        self.isBlocked = isBlocked
    }
}

// MARK: - ViewModel Layer

/**
 * Base ViewModel protocol defining common ViewModel behavior
 * 
 * This protocol demonstrates the ViewModel layer interface
 * with proper lifecycle management and state handling
 */
protocol ViewModel: ObservableObject {
    associatedtype State
    associatedtype Action
    
    var state: State { get }
    var isLoading: Bool { get }
    var error: Error? { get }
    
    func handleAction(_ action: Action)
    func loadData()
    func refreshData()
}

/**
 * User profile ViewModel implementing MVVM patterns
 * 
 * This class demonstrates proper ViewModel implementation
 * with reactive programming and state management
 */
@MainActor
class UserProfileViewModel: ViewModel {
    
    // MARK: - State Definition
    
    /**
     * User profile state representing the current UI state
     * 
     * This enum demonstrates proper state modeling for MVVM
     */
    enum State {
        case initial
        case loading
        case loaded(UserProfile)
        case error(Error)
        case refreshing(UserProfile)
    }
    
    /**
     * User profile actions representing user interactions
     * 
     * This enum demonstrates proper action modeling for MVVM
     */
    enum Action {
        case loadProfile(userId: UUID)
        case refreshProfile
        case followUser
        case unfollowUser
        case blockUser
        case unblockUser
        case retry
    }
    
    // MARK: - Published Properties
    
    @Published var state: State = .initial
    @Published var isLoading: Bool = false
    @Published var error: Error?
    
    // MARK: - Private Properties
    
    private let userId: UUID
    private let userService: UserServiceProtocol
    private let profileService: ProfileServiceProtocol
    private var cancellables = Set<AnyCancellable>()
    
    // MARK: - Computed Properties
    
    var currentProfile: UserProfile? {
        switch state {
        case .loaded(let profile), .refreshing(let profile):
            return profile
        default:
            return nil
        }
    }
    
    var isRefreshing: Bool {
        if case .refreshing = state {
            return true
        }
        return false
    }
    
    var canFollow: Bool {
        guard let profile = currentProfile else { return false }
        return !profile.isFollowing && !profile.isBlocked
    }
    
    var canUnfollow: Bool {
        guard let profile = currentProfile else { return false }
        return profile.isFollowing && !profile.isBlocked
    }
    
    var canBlock: Bool {
        guard let profile = currentProfile else { return false }
        return !profile.isBlocked
    }
    
    var canUnblock: Bool {
        guard let profile = currentProfile else { return false }
        return profile.isBlocked
    }
    
    // MARK: - Initialization
    
    init(
        userId: UUID,
        userService: UserServiceProtocol,
        profileService: ProfileServiceProtocol
    ) {
        self.userId = userId
        self.userService = userService
        self.profileService = profileService
        
        setupBindings()
    }
    
    // MARK: - Public Methods
    
    func handleAction(_ action: Action) {
        switch action {
        case .loadProfile(let userId):
            loadProfile(userId: userId)
        case .refreshProfile:
            refreshProfile()
        case .followUser:
            followUser()
        case .unfollowUser:
            unfollowUser()
        case .blockUser:
            blockUser()
        case .unblockUser:
            unblockUser()
        case .retry:
            retry()
        }
    }
    
    func loadData() {
        loadProfile(userId: userId)
    }
    
    func refreshData() {
        refreshProfile()
    }
    
    // MARK: - Private Methods
    
    private func setupBindings() {
        // Setup any reactive bindings here
        // This is where you would connect to external data sources
    }
    
    private func loadProfile(userId: UUID) {
        state = .loading
        isLoading = true
        error = nil
        
        profileService.getProfile(userId: userId)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    self?.isLoading = false
                    if case .failure(let error) = completion {
                        self?.state = .error(error)
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] profile in
                    self?.state = .loaded(profile)
                    self?.error = nil
                }
            )
            .store(in: &cancellables)
    }
    
    private func refreshProfile() {
        guard let currentProfile = currentProfile else {
            loadProfile(userId: userId)
            return
        }
        
        state = .refreshing(currentProfile)
        
        profileService.getProfile(userId: userId)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    if case .failure(let error) = completion {
                        self?.state = .error(error)
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] profile in
                    self?.state = .loaded(profile)
                    self?.error = nil
                }
            )
            .store(in: &cancellables)
    }
    
    private func followUser() {
        guard let profile = currentProfile else { return }
        
        profileService.followUser(userId: profile.user.id)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    if case .failure(let error) = completion {
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] updatedProfile in
                    self?.state = .loaded(updatedProfile)
                }
            )
            .store(in: &cancellables)
    }
    
    private func unfollowUser() {
        guard let profile = currentProfile else { return }
        
        profileService.unfollowUser(userId: profile.user.id)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    if case .failure(let error) = completion {
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] updatedProfile in
                    self?.state = .loaded(updatedProfile)
                }
            )
            .store(in: &cancellables)
    }
    
    private func blockUser() {
        guard let profile = currentProfile else { return }
        
        profileService.blockUser(userId: profile.user.id)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    if case .failure(let error) = completion {
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] updatedProfile in
                    self?.state = .loaded(updatedProfile)
                }
            )
            .store(in: &cancellables)
    }
    
    private func unblockUser() {
        guard let profile = currentProfile else { return }
        
        profileService.unblockUser(userId: profile.user.id)
            .receive(on: DispatchQueue.main)
            .sink(
                receiveCompletion: { [weak self] completion in
                    if case .failure(let error) = completion {
                        self?.error = error
                    }
                },
                receiveValue: { [weak self] updatedProfile in
                    self?.state = .loaded(updatedProfile)
                }
            )
            .store(in: &cancellables)
    }
    
    private func retry() {
        loadProfile(userId: userId)
    }
}

// MARK: - Service Layer

/**
 * User service protocol defining user-related operations
 * 
 * This protocol demonstrates proper service layer abstraction
 * for dependency injection and testing
 */
protocol UserServiceProtocol {
    func getUser(id: UUID) -> AnyPublisher<User, Error>
    func updateUser(_ user: User) -> AnyPublisher<User, Error>
    func deleteUser(id: UUID) -> AnyPublisher<Void, Error>
}

/**
 * Profile service protocol defining profile-related operations
 * 
 * This protocol demonstrates proper service layer abstraction
 * for profile management and social features
 */
protocol ProfileServiceProtocol {
    func getProfile(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func updateProfile(_ profile: UserProfile) -> AnyPublisher<UserProfile, Error>
    func followUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func unfollowUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func blockUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func unblockUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
}

/**
 * User service implementation
 * 
 * This class demonstrates proper service implementation
 * with network calls and error handling
 */
class UserService: UserServiceProtocol {
    
    private let networkService: NetworkServiceProtocol
    private let cacheService: CacheServiceProtocol
    
    init(
        networkService: NetworkServiceProtocol,
        cacheService: CacheServiceProtocol
    ) {
        self.networkService = networkService
        self.cacheService = cacheService
    }
    
    func getUser(id: UUID) -> AnyPublisher<User, Error> {
        // Check cache first
        if let cachedUser = cacheService.getUser(id: id) {
            return Just(cachedUser)
                .setFailureType(to: Error.self)
                .eraseToAnyPublisher()
        }
        
        // Fetch from network
        return networkService.getUser(id: id)
            .handleEvents(receiveOutput: { [weak self] user in
                self?.cacheService.cacheUser(user)
            })
            .eraseToAnyPublisher()
    }
    
    func updateUser(_ user: User) -> AnyPublisher<User, Error> {
        return networkService.updateUser(user)
            .handleEvents(receiveOutput: { [weak self] updatedUser in
                self?.cacheService.cacheUser(updatedUser)
            })
            .eraseToAnyPublisher()
    }
    
    func deleteUser(id: UUID) -> AnyPublisher<Void, Error> {
        return networkService.deleteUser(id: id)
            .handleEvents(receiveOutput: { [weak self] _ in
                self?.cacheService.removeUser(id: id)
            })
            .eraseToAnyPublisher()
    }
}

/**
 * Profile service implementation
 * 
 * This class demonstrates proper service implementation
 * for profile management and social features
 */
class ProfileService: ProfileServiceProtocol {
    
    private let networkService: NetworkServiceProtocol
    private let cacheService: CacheServiceProtocol
    
    init(
        networkService: NetworkServiceProtocol,
        cacheService: CacheServiceProtocol
    ) {
        self.networkService = networkService
        self.cacheService = cacheService
    }
    
    func getProfile(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        // Check cache first
        if let cachedProfile = cacheService.getProfile(userId: userId) {
            return Just(cachedProfile)
                .setFailureType(to: Error.self)
                .eraseToAnyPublisher()
        }
        
        // Fetch from network
        return networkService.getProfile(userId: userId)
            .handleEvents(receiveOutput: { [weak self] profile in
                self?.cacheService.cacheProfile(profile)
            })
            .eraseToAnyPublisher()
    }
    
    func updateProfile(_ profile: UserProfile) -> AnyPublisher<UserProfile, Error> {
        return networkService.updateProfile(profile)
            .handleEvents(receiveOutput: { [weak self] updatedProfile in
                self?.cacheService.cacheProfile(updatedProfile)
            })
            .eraseToAnyPublisher()
    }
    
    func followUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        return networkService.followUser(userId: userId)
            .handleEvents(receiveOutput: { [weak self] updatedProfile in
                self?.cacheService.cacheProfile(updatedProfile)
            })
            .eraseToAnyPublisher()
    }
    
    func unfollowUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        return networkService.unfollowUser(userId: userId)
            .handleEvents(receiveOutput: { [weak self] updatedProfile in
                self?.cacheService.cacheProfile(updatedProfile)
            })
            .eraseToAnyPublisher()
    }
    
    func blockUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        return networkService.blockUser(userId: userId)
            .handleEvents(receiveOutput: { [weak self] updatedProfile in
                self?.cacheService.cacheProfile(updatedProfile)
            })
            .eraseToAnyPublisher()
    }
    
    func unblockUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        return networkService.unblockUser(userId: userId)
            .handleEvents(receiveOutput: { [weak self] updatedProfile in
                self?.cacheService.cacheProfile(updatedProfile)
            })
            .eraseToAnyPublisher()
    }
}

// MARK: - Network Layer

/**
 * Network service protocol defining network operations
 * 
 * This protocol demonstrates proper network layer abstraction
 * for HTTP requests and API communication
 */
protocol NetworkServiceProtocol {
    func getUser(id: UUID) -> AnyPublisher<User, Error>
    func updateUser(_ user: User) -> AnyPublisher<User, Error>
    func deleteUser(id: UUID) -> AnyPublisher<Void, Error>
    func getProfile(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func updateProfile(_ profile: UserProfile) -> AnyPublisher<UserProfile, Error>
    func followUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func unfollowUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func blockUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
    func unblockUser(userId: UUID) -> AnyPublisher<UserProfile, Error>
}

/**
 * Network service implementation
 * 
 * This class demonstrates proper network service implementation
 * with URLSession and Combine
 */
class NetworkService: NetworkServiceProtocol {
    
    private let baseURL: URL
    private let session: URLSession
    
    init(baseURL: URL, session: URLSession = .shared) {
        self.baseURL = baseURL
        self.session = session
    }
    
    func getUser(id: UUID) -> AnyPublisher<User, Error> {
        let url = baseURL.appendingPathComponent("users/\(id)")
        return performRequest(url: url, responseType: User.self)
    }
    
    func updateUser(_ user: User) -> AnyPublisher<User, Error> {
        let url = baseURL.appendingPathComponent("users/\(user.id)")
        return performRequest(url: url, method: "PUT", body: user, responseType: User.self)
    }
    
    func deleteUser(id: UUID) -> AnyPublisher<Void, Error> {
        let url = baseURL.appendingPathComponent("users/\(id)")
        return performRequest(url: url, method: "DELETE", responseType: Void.self)
    }
    
    func getProfile(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        let url = baseURL.appendingPathComponent("profiles/\(userId)")
        return performRequest(url: url, responseType: UserProfile.self)
    }
    
    func updateProfile(_ profile: UserProfile) -> AnyPublisher<UserProfile, Error> {
        let url = baseURL.appendingPathComponent("profiles/\(profile.user.id)")
        return performRequest(url: url, method: "PUT", body: profile, responseType: UserProfile.self)
    }
    
    func followUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        let url = baseURL.appendingPathComponent("profiles/\(userId)/follow")
        return performRequest(url: url, method: "POST", responseType: UserProfile.self)
    }
    
    func unfollowUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        let url = baseURL.appendingPathComponent("profiles/\(userId)/unfollow")
        return performRequest(url: url, method: "POST", responseType: UserProfile.self)
    }
    
    func blockUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        let url = baseURL.appendingPathComponent("profiles/\(userId)/block")
        return performRequest(url: url, method: "POST", responseType: UserProfile.self)
    }
    
    func unblockUser(userId: UUID) -> AnyPublisher<UserProfile, Error> {
        let url = baseURL.appendingPathComponent("profiles/\(userId)/unblock")
        return performRequest(url: url, method: "POST", responseType: UserProfile.self)
    }
    
    // MARK: - Private Methods
    
    private func performRequest<T: Codable>(
        url: URL,
        method: String = "GET",
        body: T? = nil,
        responseType: T.Type
    ) -> AnyPublisher<T, Error> {
        var request = URLRequest(url: url)
        request.httpMethod = method
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        
        if let body = body {
            do {
                request.httpBody = try JSONEncoder().encode(body)
            } catch {
                return Fail(error: error)
                    .eraseToAnyPublisher()
            }
        }
        
        return session.dataTaskPublisher(for: request)
            .map(\.data)
            .decode(type: responseType, decoder: JSONDecoder())
            .eraseToAnyPublisher()
    }
}

// MARK: - Cache Layer

/**
 * Cache service protocol defining caching operations
 * 
 * This protocol demonstrates proper cache layer abstraction
 * for data persistence and performance optimization
 */
protocol CacheServiceProtocol {
    func getUser(id: UUID) -> User?
    func cacheUser(_ user: User)
    func removeUser(id: UUID)
    func getProfile(userId: UUID) -> UserProfile?
    func cacheProfile(_ profile: UserProfile)
    func removeProfile(userId: UUID)
}

/**
 * Cache service implementation
 * 
 * This class demonstrates proper cache service implementation
 * with in-memory caching and persistence
 */
class CacheService: CacheServiceProtocol {
    
    private var userCache: [UUID: User] = [:]
    private var profileCache: [UUID: UserProfile] = [:]
    private let cacheQueue = DispatchQueue(label: "com.company.cache", attributes: .concurrent)
    
    func getUser(id: UUID) -> User? {
        return cacheQueue.sync {
            return userCache[id]
        }
    }
    
    func cacheUser(_ user: User) {
        cacheQueue.async(flags: .barrier) {
            self.userCache[user.id] = user
        }
    }
    
    func removeUser(id: UUID) {
        cacheQueue.async(flags: .barrier) {
            self.userCache.removeValue(forKey: id)
        }
    }
    
    func getProfile(userId: UUID) -> UserProfile? {
        return cacheQueue.sync {
            return profileCache[userId]
        }
    }
    
    func cacheProfile(_ profile: UserProfile) {
        cacheQueue.async(flags: .barrier) {
            self.profileCache[profile.user.id] = profile
        }
    }
    
    func removeProfile(userId: UUID) {
        cacheQueue.async(flags: .barrier) {
            self.profileCache.removeValue(forKey: userId)
        }
    }
}

// MARK: - Dependency Injection

/**
 * Dependency injection container for MVVM architecture
 * 
 * This class demonstrates proper dependency injection
 * for MVVM architecture with service registration
 */
class DIContainer {
    
    private var services: [String: Any] = [:]
    
    func register<T>(_ type: T.Type, factory: @escaping () -> T) {
        let key = String(describing: type)
        services[key] = factory
    }
    
    func resolve<T>(_ type: T.Type) -> T {
        let key = String(describing: type)
        guard let factory = services[key] as? () -> T else {
            fatalError("Service \(type) not registered")
        }
        return factory()
    }
    
    func setup() {
        // Register services
        register(NetworkServiceProtocol.self) {
            NetworkService(baseURL: URL(string: "https://api.example.com")!)
        }
        
        register(CacheServiceProtocol.self) {
            CacheService()
        }
        
        register(UserServiceProtocol.self) {
            UserService(
                networkService: self.resolve(NetworkServiceProtocol.self),
                cacheService: self.resolve(CacheServiceProtocol.self)
            )
        }
        
        register(ProfileServiceProtocol.self) {
            ProfileService(
                networkService: self.resolve(NetworkServiceProtocol.self),
                cacheService: self.resolve(CacheServiceProtocol.self)
            )
        }
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use the MVVM architecture
 * 
 * This function shows practical usage of all the MVVM components
 */
func demonstrateMVVM() {
    print("=== MVVM Architecture Demonstration ===\n")
    
    // Setup dependency injection
    let container = DIContainer()
    container.setup()
    
    // Create ViewModel with dependencies
    let userId = UUID()
    let userService = container.resolve(UserServiceProtocol.self)
    let profileService = container.resolve(ProfileServiceProtocol.self)
    
    let viewModel = UserProfileViewModel(
        userId: userId,
        userService: userService,
        profileService: profileService
    )
    
    // Demonstrate ViewModel usage
    print("--- ViewModel Usage ---")
    print("Initial state: \(viewModel.state)")
    print("Is loading: \(viewModel.isLoading)")
    print("Error: \(viewModel.error?.localizedDescription ?? "None")")
    
    // Load profile
    viewModel.handleAction(.loadProfile(userId: userId))
    print("After load action - State: \(viewModel.state)")
    
    // Follow user
    viewModel.handleAction(.followUser)
    print("After follow action - State: \(viewModel.state)")
    
    // Refresh profile
    viewModel.handleAction(.refreshProfile)
    print("After refresh action - State: \(viewModel.state)")
    
    // Demonstrate service layer
    print("\n--- Service Layer ---")
    let networkService = container.resolve(NetworkServiceProtocol.self)
    let cacheService = container.resolve(CacheServiceProtocol.self)
    
    print("Network service: \(type(of: networkService))")
    print("Cache service: \(type(of: cacheService))")
    
    // Demonstrate model layer
    print("\n--- Model Layer ---")
    let user = User(
        username: "johndoe",
        email: "john@example.com",
        firstName: "John",
        lastName: "Doe"
    )
    
    let profile = UserProfile(
        user: user,
        bio: "Software Engineer",
        location: "San Francisco, CA",
        website: "https://johndoe.com",
        followersCount: 1000,
        followingCount: 500,
        postsCount: 50
    )
    
    print("User: \(user.fullName) (\(user.displayName))")
    print("Profile: \(profile.user.fullName) - \(profile.followersCount) followers")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateMVVM()
