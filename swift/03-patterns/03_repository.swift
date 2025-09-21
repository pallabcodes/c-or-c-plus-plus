/*
 * Design Patterns: Repository Pattern
 * 
 * This file demonstrates production-grade Repository pattern implementation in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master Repository pattern for data access abstraction
 * - Understand caching strategies and data synchronization
 * - Implement proper error handling and state management
 * - Apply dependency injection and testing patterns
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import Combine

// MARK: - Repository Protocol

/**
 * Base repository protocol defining common data access operations
 * 
 * This protocol demonstrates the core Repository pattern interface
 * with proper abstraction and error handling
 */
protocol Repository {
    associatedtype Entity
    associatedtype ID
    
    func getAll() -> AnyPublisher<[Entity], Error>
    func getById(_ id: ID) -> AnyPublisher<Entity?, Error>
    func create(_ entity: Entity) -> AnyPublisher<Entity, Error>
    func update(_ entity: Entity) -> AnyPublisher<Entity, Error>
    func delete(_ id: ID) -> AnyPublisher<Void, Error>
}

// MARK: - User Repository

/**
 * User repository protocol defining user data operations
 * 
 * This protocol demonstrates domain-specific repository interface
 * with proper abstraction and error handling
 */
protocol UserRepositoryProtocol: Repository where Entity == User, ID == UUID {
    func getByUsername(_ username: String) -> AnyPublisher<User?, Error>
    func searchUsers(_ query: String) -> AnyPublisher<[User], Error>
    func getFollowers(_ userId: UUID) -> AnyPublisher<[User], Error>
    func getFollowing(_ userId: UUID) -> AnyPublisher<[User], Error>
}

/**
 * User repository implementation with caching and network layers
 * 
 * This class demonstrates proper repository implementation
 * with multi-level caching and error handling
 */
class UserRepository: UserRepositoryProtocol {
    
    private let networkService: NetworkServiceProtocol
    private let cacheService: CacheServiceProtocol
    private let localDatabase: LocalDatabaseProtocol
    
    init(
        networkService: NetworkServiceProtocol,
        cacheService: CacheServiceProtocol,
        localDatabase: LocalDatabaseProtocol
    ) {
        self.networkService = networkService
        self.cacheService = cacheService
        self.localDatabase = localDatabase
    }
    
    func getAll() -> AnyPublisher<[User], Error> {
        // Try cache first
        if let cachedUsers = cacheService.getUsers() {
            return Just(cachedUsers)
                .setFailureType(to: Error.self)
                .eraseToAnyPublisher()
        }
        
        // Fallback to local database
        return localDatabase.getUsers()
            .flatMap { localUsers in
                if !localUsers.isEmpty {
                    return Just(localUsers)
                        .setFailureType(to: Error.self)
                        .eraseToAnyPublisher()
                } else {
                    // Fetch from network
                    return self.networkService.getUsers()
                        .handleEvents(receiveOutput: { users in
                            self.cacheService.cacheUsers(users)
                            self.localDatabase.saveUsers(users)
                        })
                        .eraseToAnyPublisher()
                }
            }
            .eraseToAnyPublisher()
    }
    
    func getById(_ id: UUID) -> AnyPublisher<User?, Error> {
        // Try cache first
        if let cachedUser = cacheService.getUser(id: id) {
            return Just(cachedUser)
                .setFailureType(to: Error.self)
                .eraseToAnyPublisher()
        }
        
        // Fallback to local database
        return localDatabase.getUser(id: id)
            .flatMap { localUser in
                if let user = localUser {
                    return Just(user)
                        .setFailureType(to: Error.self)
                        .eraseToAnyPublisher()
                } else {
                    // Fetch from network
                    return self.networkService.getUser(id: id)
                        .handleEvents(receiveOutput: { user in
                            if let user = user {
                                self.cacheService.cacheUser(user)
                                self.localDatabase.saveUser(user)
                            }
                        })
                        .eraseToAnyPublisher()
                }
            }
            .eraseToAnyPublisher()
    }
    
    func create(_ entity: User) -> AnyPublisher<User, Error> {
        return networkService.createUser(entity)
            .handleEvents(receiveOutput: { user in
                self.cacheService.cacheUser(user)
                self.localDatabase.saveUser(user)
            })
            .eraseToAnyPublisher()
    }
    
    func update(_ entity: User) -> AnyPublisher<User, Error> {
        return networkService.updateUser(entity)
            .handleEvents(receiveOutput: { user in
                self.cacheService.cacheUser(user)
                self.localDatabase.saveUser(user)
            })
            .eraseToAnyPublisher()
    }
    
    func delete(_ id: UUID) -> AnyPublisher<Void, Error> {
        return networkService.deleteUser(id: id)
            .handleEvents(receiveOutput: { _ in
                self.cacheService.removeUser(id: id)
                self.localDatabase.deleteUser(id: id)
            })
            .eraseToAnyPublisher()
    }
    
    func getByUsername(_ username: String) -> AnyPublisher<User?, Error> {
        return networkService.getUserByUsername(username)
            .handleEvents(receiveOutput: { user in
                if let user = user {
                    self.cacheService.cacheUser(user)
                    self.localDatabase.saveUser(user)
                }
            })
            .eraseToAnyPublisher()
    }
    
    func searchUsers(_ query: String) -> AnyPublisher<[User], Error> {
        return networkService.searchUsers(query)
            .handleEvents(receiveOutput: { users in
                self.cacheService.cacheUsers(users)
                self.localDatabase.saveUsers(users)
            })
            .eraseToAnyPublisher()
    }
    
    func getFollowers(_ userId: UUID) -> AnyPublisher<[User], Error> {
        return networkService.getFollowers(userId: userId)
            .handleEvents(receiveOutput: { users in
                self.cacheService.cacheUsers(users)
                self.localDatabase.saveUsers(users)
            })
            .eraseToAnyPublisher()
    }
    
    func getFollowing(_ userId: UUID) -> AnyPublisher<[User], Error> {
        return networkService.getFollowing(userId: userId)
            .handleEvents(receiveOutput: { users in
                self.cacheService.cacheUsers(users)
                self.localDatabase.saveUsers(users)
            })
            .eraseToAnyPublisher()
    }
}

// MARK: - Post Repository

/**
 * Post repository protocol defining post data operations
 * 
 * This protocol demonstrates domain-specific repository interface
 * for content management and social features
 */
protocol PostRepositoryProtocol: Repository where Entity == Post, ID == UUID {
    func getByUserId(_ userId: UUID) -> AnyPublisher<[Post], Error>
    func getFeed(_ userId: UUID) -> AnyPublisher<[Post], Error>
    func searchPosts(_ query: String) -> AnyPublisher<[Post], Error>
    func likePost(_ postId: UUID) -> AnyPublisher<Post, Error>
    func unlikePost(_ postId: UUID) -> AnyPublisher<Post, Error>
}

/**
 * Post repository implementation with caching and network layers
 * 
 * This class demonstrates proper repository implementation
 * for content management with real-time updates
 */
class PostRepository: PostRepositoryProtocol {
    
    private let networkService: NetworkServiceProtocol
    private let cacheService: CacheServiceProtocol
    private let localDatabase: LocalDatabaseProtocol
    private let realTimeService: RealTimeServiceProtocol
    
    init(
        networkService: NetworkServiceProtocol,
        cacheService: CacheServiceProtocol,
        localDatabase: LocalDatabaseProtocol,
        realTimeService: RealTimeServiceProtocol
    ) {
        self.networkService = networkService
        self.cacheService = cacheService
        self.localDatabase = localDatabase
        self.realTimeService = realTimeService
    }
    
    func getAll() -> AnyPublisher<[Post], Error> {
        // Try cache first
        if let cachedPosts = cacheService.getPosts() {
            return Just(cachedPosts)
                .setFailureType(to: Error.self)
                .eraseToAnyPublisher()
        }
        
        // Fallback to local database
        return localDatabase.getPosts()
            .flatMap { localPosts in
                if !localPosts.isEmpty {
                    return Just(localPosts)
                        .setFailureType(to: Error.self)
                        .eraseToAnyPublisher()
                } else {
                    // Fetch from network
                    return self.networkService.getPosts()
                        .handleEvents(receiveOutput: { posts in
                            self.cacheService.cachePosts(posts)
                            self.localDatabase.savePosts(posts)
                        })
                        .eraseToAnyPublisher()
                }
            }
            .eraseToAnyPublisher()
    }
    
    func getById(_ id: UUID) -> AnyPublisher<Post?, Error> {
        // Try cache first
        if let cachedPost = cacheService.getPost(id: id) {
            return Just(cachedPost)
                .setFailureType(to: Error.self)
                .eraseToAnyPublisher()
        }
        
        // Fallback to local database
        return localDatabase.getPost(id: id)
            .flatMap { localPost in
                if let post = localPost {
                    return Just(post)
                        .setFailureType(to: Error.self)
                        .eraseToAnyPublisher()
                } else {
                    // Fetch from network
                    return self.networkService.getPost(id: id)
                        .handleEvents(receiveOutput: { post in
                            if let post = post {
                                self.cacheService.cachePost(post)
                                self.localDatabase.savePost(post)
                            }
                        })
                        .eraseToAnyPublisher()
                }
            }
            .eraseToAnyPublisher()
    }
    
    func create(_ entity: Post) -> AnyPublisher<Post, Error> {
        return networkService.createPost(entity)
            .handleEvents(receiveOutput: { post in
                self.cacheService.cachePost(post)
                self.localDatabase.savePost(post)
                self.realTimeService.broadcastPost(post)
            })
            .eraseToAnyPublisher()
    }
    
    func update(_ entity: Post) -> AnyPublisher<Post, Error> {
        return networkService.updatePost(entity)
            .handleEvents(receiveOutput: { post in
                self.cacheService.cachePost(post)
                self.localDatabase.savePost(post)
                self.realTimeService.broadcastPostUpdate(post)
            })
            .eraseToAnyPublisher()
    }
    
    func delete(_ id: UUID) -> AnyPublisher<Void, Error> {
        return networkService.deletePost(id: id)
            .handleEvents(receiveOutput: { _ in
                self.cacheService.removePost(id: id)
                self.localDatabase.deletePost(id: id)
                self.realTimeService.broadcastPostDeletion(id)
            })
            .eraseToAnyPublisher()
    }
    
    func getByUserId(_ userId: UUID) -> AnyPublisher<[Post], Error> {
        return networkService.getPostsByUserId(userId)
            .handleEvents(receiveOutput: { posts in
                self.cacheService.cachePosts(posts)
                self.localDatabase.savePosts(posts)
            })
            .eraseToAnyPublisher()
    }
    
    func getFeed(_ userId: UUID) -> AnyPublisher<[Post], Error> {
        return networkService.getFeed(userId: userId)
            .handleEvents(receiveOutput: { posts in
                self.cacheService.cachePosts(posts)
                self.localDatabase.savePosts(posts)
            })
            .eraseToAnyPublisher()
    }
    
    func searchPosts(_ query: String) -> AnyPublisher<[Post], Error> {
        return networkService.searchPosts(query)
            .handleEvents(receiveOutput: { posts in
                self.cacheService.cachePosts(posts)
                self.localDatabase.savePosts(posts)
            })
            .eraseToAnyPublisher()
    }
    
    func likePost(_ postId: UUID) -> AnyPublisher<Post, Error> {
        return networkService.likePost(postId)
            .handleEvents(receiveOutput: { post in
                self.cacheService.cachePost(post)
                self.localDatabase.savePost(post)
                self.realTimeService.broadcastPostUpdate(post)
            })
            .eraseToAnyPublisher()
    }
    
    func unlikePost(_ postId: UUID) -> AnyPublisher<Post, Error> {
        return networkService.unlikePost(postId)
            .handleEvents(receiveOutput: { post in
                self.cacheService.cachePost(post)
                self.localDatabase.savePost(post)
                self.realTimeService.broadcastPostUpdate(post)
            })
            .eraseToAnyPublisher()
    }
}

// MARK: - Supporting Types

/**
 * User model for demonstration
 */
struct User: Codable, Equatable, Identifiable {
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

/**
 * Post model for demonstration
 */
struct Post: Codable, Equatable, Identifiable {
    let id: UUID
    let userId: UUID
    let content: String
    let imageURL: String?
    let likesCount: Int
    let commentsCount: Int
    let isLiked: Bool
    let createdAt: Date
    let updatedAt: Date
}

// MARK: - Service Protocols

protocol NetworkServiceProtocol {
    func getUsers() -> AnyPublisher<[User], Error>
    func getUser(id: UUID) -> AnyPublisher<User?, Error>
    func getUserByUsername(_ username: String) -> AnyPublisher<User?, Error>
    func createUser(_ user: User) -> AnyPublisher<User, Error>
    func updateUser(_ user: User) -> AnyPublisher<User, Error>
    func deleteUser(id: UUID) -> AnyPublisher<Void, Error>
    func searchUsers(_ query: String) -> AnyPublisher<[User], Error>
    func getFollowers(userId: UUID) -> AnyPublisher<[User], Error>
    func getFollowing(userId: UUID) -> AnyPublisher<[User], Error>
    
    func getPosts() -> AnyPublisher<[Post], Error>
    func getPost(id: UUID) -> AnyPublisher<Post?, Error>
    func getPostsByUserId(_ userId: UUID) -> AnyPublisher<[Post], Error>
    func createPost(_ post: Post) -> AnyPublisher<Post, Error>
    func updatePost(_ post: Post) -> AnyPublisher<Post, Error>
    func deletePost(id: UUID) -> AnyPublisher<Void, Error>
    func searchPosts(_ query: String) -> AnyPublisher<[Post], Error>
    func getFeed(userId: UUID) -> AnyPublisher<[Post], Error>
    func likePost(_ postId: UUID) -> AnyPublisher<Post, Error>
    func unlikePost(_ postId: UUID) -> AnyPublisher<Post, Error>
}

protocol CacheServiceProtocol {
    func getUsers() -> [User]?
    func cacheUsers(_ users: [User])
    func getUser(id: UUID) -> User?
    func cacheUser(_ user: User)
    func removeUser(id: UUID)
    
    func getPosts() -> [Post]?
    func cachePosts(_ posts: [Post])
    func getPost(id: UUID) -> Post?
    func cachePost(_ post: Post)
    func removePost(id: UUID)
}

protocol LocalDatabaseProtocol {
    func getUsers() -> AnyPublisher<[User], Error>
    func getUser(id: UUID) -> AnyPublisher<User?, Error>
    func saveUsers(_ users: [User])
    func saveUser(_ user: User)
    func deleteUser(id: UUID)
    
    func getPosts() -> AnyPublisher<[Post], Error>
    func getPost(id: UUID) -> AnyPublisher<Post?, Error>
    func savePosts(_ posts: [Post])
    func savePost(_ post: Post)
    func deletePost(id: UUID)
}

protocol RealTimeServiceProtocol {
    func broadcastPost(_ post: Post)
    func broadcastPostUpdate(_ post: Post)
    func broadcastPostDeletion(_ postId: UUID)
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use the Repository pattern
 * 
 * This function shows practical usage of all the repository components
 */
func demonstrateRepositoryPattern() {
    print("=== Repository Pattern Demonstration ===\n")
    
    // Setup dependencies
    let networkService = NetworkService()
    let cacheService = CacheService()
    let localDatabase = LocalDatabase()
    let realTimeService = RealTimeService()
    
    // Create repositories
    let userRepository = UserRepository(
        networkService: networkService,
        cacheService: cacheService,
        localDatabase: localDatabase
    )
    
    let postRepository = PostRepository(
        networkService: networkService,
        cacheService: cacheService,
        localDatabase: localDatabase,
        realTimeService: realTimeService
    )
    
    print("--- User Repository ---")
    let userId = UUID()
    userRepository.getById(userId)
        .sink(
            receiveCompletion: { completion in
                if case .failure(let error) = completion {
                    print("Error: \(error.localizedDescription)")
                }
            },
            receiveValue: { user in
                print("User: \(user?.fullName ?? "Not found")")
            }
        )
        .store(in: &Set<AnyCancellable>())
    
    print("\n--- Post Repository ---")
    let postId = UUID()
    postRepository.getById(postId)
        .sink(
            receiveCompletion: { completion in
                if case .failure(let error) = completion {
                    print("Error: \(error.localizedDescription)")
                }
            },
            receiveValue: { post in
                print("Post: \(post?.content ?? "Not found")")
            }
        )
        .store(in: &Set<AnyCancellable>())
}

// MARK: - Service Implementations (Placeholder)

class NetworkService: NetworkServiceProtocol {
    func getUsers() -> AnyPublisher<[User], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getUser(id: UUID) -> AnyPublisher<User?, Error> { Just(nil).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getUserByUsername(_ username: String) -> AnyPublisher<User?, Error> { Just(nil).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func createUser(_ user: User) -> AnyPublisher<User, Error> { Just(user).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func updateUser(_ user: User) -> AnyPublisher<User, Error> { Just(user).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func deleteUser(id: UUID) -> AnyPublisher<Void, Error> { Just(()).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func searchUsers(_ query: String) -> AnyPublisher<[User], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getFollowers(userId: UUID) -> AnyPublisher<[User], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getFollowing(userId: UUID) -> AnyPublisher<[User], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    
    func getPosts() -> AnyPublisher<[Post], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getPost(id: UUID) -> AnyPublisher<Post?, Error> { Just(nil).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getPostsByUserId(_ userId: UUID) -> AnyPublisher<[Post], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func createPost(_ post: Post) -> AnyPublisher<Post, Error> { Just(post).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func updatePost(_ post: Post) -> AnyPublisher<Post, Error> { Just(post).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func deletePost(id: UUID) -> AnyPublisher<Void, Error> { Just(()).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func searchPosts(_ query: String) -> AnyPublisher<[Post], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getFeed(userId: UUID) -> AnyPublisher<[Post], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func likePost(_ postId: UUID) -> AnyPublisher<Post, Error> { Just(Post(id: postId, userId: UUID(), content: "", imageURL: nil, likesCount: 0, commentsCount: 0, isLiked: false, createdAt: Date(), updatedAt: Date())).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func unlikePost(_ postId: UUID) -> AnyPublisher<Post, Error> { Just(Post(id: postId, userId: UUID(), content: "", imageURL: nil, likesCount: 0, commentsCount: 0, isLiked: false, createdAt: Date(), updatedAt: Date())).setFailureType(to: Error.self).eraseToAnyPublisher() }
}

class CacheService: CacheServiceProtocol {
    func getUsers() -> [User]? { nil }
    func cacheUsers(_ users: [User]) {}
    func getUser(id: UUID) -> User? { nil }
    func cacheUser(_ user: User) {}
    func removeUser(id: UUID) {}
    
    func getPosts() -> [Post]? { nil }
    func cachePosts(_ posts: [Post]) {}
    func getPost(id: UUID) -> Post? { nil }
    func cachePost(_ post: Post) {}
    func removePost(id: UUID) {}
}

class LocalDatabase: LocalDatabaseProtocol {
    func getUsers() -> AnyPublisher<[User], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getUser(id: UUID) -> AnyPublisher<User?, Error> { Just(nil).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func saveUsers(_ users: [User]) {}
    func saveUser(_ user: User) {}
    func deleteUser(id: UUID) {}
    
    func getPosts() -> AnyPublisher<[Post], Error> { Just([]).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func getPost(id: UUID) -> AnyPublisher<Post?, Error> { Just(nil).setFailureType(to: Error.self).eraseToAnyPublisher() }
    func savePosts(_ posts: [Post]) {}
    func savePost(_ post: Post) {}
    func deletePost(id: UUID) {}
}

class RealTimeService: RealTimeServiceProtocol {
    func broadcastPost(_ post: Post) {}
    func broadcastPostUpdate(_ post: Post) {}
    func broadcastPostDeletion(_ postId: UUID) {}
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateRepositoryPattern()
