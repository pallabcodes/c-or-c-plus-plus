/*
 * iOS Frameworks: Core Data
 * 
 * This file demonstrates production-grade Core Data patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master Core Data modeling and relationships
 * - Understand data migration and schema changes
 * - Implement proper performance optimization
 * - Apply background processing and concurrency
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import CoreData
import Combine

// MARK: - Core Data Stack

/**
 * Production-grade Core Data stack
 * 
 * This class demonstrates proper Core Data stack implementation
 * with performance optimization and error handling
 */
class CoreDataStack {
    
    // MARK: - Singleton
    
    static let shared = CoreDataStack()
    
    // MARK: - Properties
    
    private let modelName: String
    private let storeURL: URL
    
    lazy var persistentContainer: NSPersistentContainer = {
        let container = NSPersistentContainer(name: modelName)
        
        // Configure store description
        let storeDescription = container.persistentStoreDescriptions.first
        storeDescription?.url = storeURL
        storeDescription?.shouldMigrateStoreAutomatically = true
        storeDescription?.shouldInferMappingModelAutomatically = true
        
        // Performance optimizations
        storeDescription?.setOption(true as NSNumber, forKey: NSPersistentHistoryTrackingKey)
        storeDescription?.setOption(true as NSNumber, forKey: NSPersistentStoreRemoteChangeNotificationPostOptionKey)
        
        container.loadPersistentStores { [weak self] storeDescription, error in
            if let error = error as NSError? {
                self?.handleStoreLoadingError(error)
            }
        }
        
        // Configure view context
        container.viewContext.automaticallyMergesChangesFromParent = true
        container.viewContext.mergePolicy = NSMergeByPropertyObjectTrumpMergePolicy
        
        return container
    }()
    
    // MARK: - Computed Properties
    
    var viewContext: NSManagedObjectContext {
        return persistentContainer.viewContext
    }
    
    var backgroundContext: NSManagedObjectContext {
        return persistentContainer.newBackgroundContext()
    }
    
    // MARK: - Initialization
    
    private init(modelName: String = "DataModel") {
        self.modelName = modelName
        
        // Create store URL
        let documentsDirectory = FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first!
        self.storeURL = documentsDirectory.appendingPathComponent("\(modelName).sqlite")
    }
    
    // MARK: - Public Methods
    
    func saveContext() {
        let context = persistentContainer.viewContext
        
        if context.hasChanges {
            do {
                try context.save()
            } catch {
                handleSaveError(error)
            }
        }
    }
    
    func saveBackgroundContext(_ context: NSManagedObjectContext) {
        if context.hasChanges {
            do {
                try context.save()
            } catch {
                handleSaveError(error)
            }
        }
    }
    
    func performBackgroundTask<T>(_ block: @escaping (NSManagedObjectContext) -> T) -> AnyPublisher<T, Error> {
        return Future<T, Error> { promise in
            self.persistentContainer.performBackgroundTask { context in
                do {
                    let result = block(context)
                    try context.save()
                    promise(.success(result))
                } catch {
                    promise(.failure(error))
                }
            }
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func handleStoreLoadingError(_ error: NSError) {
        print("Core Data error: \(error), \(error.userInfo)")
        
        // In production, you would handle this more gracefully
        // For example, by attempting to migrate or reset the store
    }
    
    private func handleSaveError(_ error: Error) {
        print("Core Data save error: \(error)")
        
        // In production, you would handle this more gracefully
        // For example, by logging to a crash reporting service
    }
}

// MARK: - Data Models

/**
 * User entity extension
 * 
 * This extension demonstrates proper Core Data entity management
 * with computed properties and business logic
 */
extension UserEntity {
    
    // MARK: - Computed Properties
    
    var fullName: String {
        return "\(firstName ?? "") \(lastName ?? "")"
    }
    
    var isActive: Bool {
        return status == "active"
    }
    
    var formattedCreatedAt: String {
        guard let createdAt = createdAt else { return "" }
        let formatter = DateFormatter()
        formatter.dateStyle = .medium
        formatter.timeStyle = .short
        return formatter.string(from: createdAt)
    }
    
    // MARK: - Business Logic
    
    func activate() {
        status = "active"
        updatedAt = Date()
    }
    
    func deactivate() {
        status = "inactive"
        updatedAt = Date()
    }
    
    func updateProfile(firstName: String, lastName: String, email: String) {
        self.firstName = firstName
        self.lastName = lastName
        self.email = email
        self.updatedAt = Date()
    }
}

/**
 * Post entity extension
 * 
 * This extension demonstrates proper Core Data entity management
 * with relationships and business logic
 */
extension PostEntity {
    
    // MARK: - Computed Properties
    
    var isPublished: Bool {
        return status == "published"
    }
    
    var formattedCreatedAt: String {
        guard let createdAt = createdAt else { return "" }
        let formatter = DateFormatter()
        formatter.dateStyle = .medium
        formatter.timeStyle = .short
        return formatter.string(from: createdAt)
    }
    
    // MARK: - Business Logic
    
    func publish() {
        status = "published"
        publishedAt = Date()
        updatedAt = Date()
    }
    
    func unpublish() {
        status = "draft"
        publishedAt = nil
        updatedAt = Date()
    }
    
    func addComment(_ content: String, author: UserEntity) {
        let comment = CommentEntity(context: managedObjectContext!)
        comment.id = UUID()
        comment.content = content
        comment.author = author
        comment.post = self
        comment.createdAt = Date()
        
        addToComments(comment)
    }
}

// MARK: - Data Manager

/**
 * Core Data manager for business logic
 * 
 * This class demonstrates proper data management patterns
 * with performance optimization and error handling
 */
class CoreDataManager: ObservableObject {
    
    // MARK: - Published Properties
    
    @Published var users: [UserEntity] = []
    @Published var posts: [PostEntity] = []
    @Published var isLoading = false
    @Published var error: Error?
    
    // MARK: - Private Properties
    
    private let coreDataStack: CoreDataStack
    private var cancellables = Set<AnyCancellable>()
    
    // MARK: - Initialization
    
    init(coreDataStack: CoreDataStack = .shared) {
        self.coreDataStack = coreDataStack
        setupBindings()
    }
    
    // MARK: - Setup
    
    private func setupBindings() {
        // Observe context changes
        NotificationCenter.default
            .publisher(for: .NSManagedObjectContextDidSave)
            .sink { [weak self] _ in
                self?.refreshData()
            }
            .store(in: &cancellables)
    }
    
    // MARK: - User Management
    
    func createUser(
        firstName: String,
        lastName: String,
        email: String,
        avatarURL: String? = nil
    ) -> AnyPublisher<UserEntity, Error> {
        return coreDataStack.performBackgroundTask { context in
            let user = UserEntity(context: context)
            user.id = UUID()
            user.firstName = firstName
            user.lastName = lastName
            user.email = email
            user.avatarURL = avatarURL
            user.status = "active"
            user.createdAt = Date()
            user.updatedAt = Date()
            
            return user
        }
    }
    
    func updateUser(
        _ user: UserEntity,
        firstName: String,
        lastName: String,
        email: String
    ) -> AnyPublisher<UserEntity, Error> {
        return coreDataStack.performBackgroundTask { context in
            let userInContext = context.object(with: user.objectID) as! UserEntity
            userInContext.updateProfile(firstName: firstName, lastName: lastName, email: email)
            return userInContext
        }
    }
    
    func deleteUser(_ user: UserEntity) -> AnyPublisher<Void, Error> {
        return coreDataStack.performBackgroundTask { context in
            let userInContext = context.object(with: user.objectID) as! UserEntity
            context.delete(userInContext)
        }
    }
    
    func fetchUsers() -> AnyPublisher<[UserEntity], Error> {
        return coreDataStack.performBackgroundTask { context in
            let request: NSFetchRequest<UserEntity> = UserEntity.fetchRequest()
            request.sortDescriptors = [NSSortDescriptor(keyPath: \UserEntity.createdAt, ascending: false)]
            
            return try context.fetch(request)
        }
    }
    
    func searchUsers(_ query: String) -> AnyPublisher<[UserEntity], Error> {
        return coreDataStack.performBackgroundTask { context in
            let request: NSFetchRequest<UserEntity> = UserEntity.fetchRequest()
            request.predicate = NSPredicate(
                format: "firstName CONTAINS[cd] %@ OR lastName CONTAINS[cd] %@ OR email CONTAINS[cd] %@",
                query, query, query
            )
            request.sortDescriptors = [NSSortDescriptor(keyPath: \UserEntity.createdAt, ascending: false)]
            
            return try context.fetch(request)
        }
    }
    
    // MARK: - Post Management
    
    func createPost(
        title: String,
        content: String,
        author: UserEntity,
        imageURL: String? = nil
    ) -> AnyPublisher<PostEntity, Error> {
        return coreDataStack.performBackgroundTask { context in
            let post = PostEntity(context: context)
            post.id = UUID()
            post.title = title
            post.content = content
            post.imageURL = imageURL
            post.status = "draft"
            post.author = context.object(with: author.objectID) as? UserEntity
            post.createdAt = Date()
            post.updatedAt = Date()
            
            return post
        }
    }
    
    func updatePost(
        _ post: PostEntity,
        title: String,
        content: String
    ) -> AnyPublisher<PostEntity, Error> {
        return coreDataStack.performBackgroundTask { context in
            let postInContext = context.object(with: post.objectID) as! PostEntity
            postInContext.title = title
            postInContext.content = content
            postInContext.updatedAt = Date()
            return postInContext
        }
    }
    
    func deletePost(_ post: PostEntity) -> AnyPublisher<Void, Error> {
        return coreDataStack.performBackgroundTask { context in
            let postInContext = context.object(with: post.objectID) as! PostEntity
            context.delete(postInContext)
        }
    }
    
    func fetchPosts() -> AnyPublisher<[PostEntity], Error> {
        return coreDataStack.performBackgroundTask { context in
            let request: NSFetchRequest<PostEntity> = PostEntity.fetchRequest()
            request.sortDescriptors = [NSSortDescriptor(keyPath: \PostEntity.createdAt, ascending: false)]
            
            return try context.fetch(request)
        }
    }
    
    func fetchPostsByAuthor(_ author: UserEntity) -> AnyPublisher<[PostEntity], Error> {
        return coreDataStack.performBackgroundTask { context in
            let request: NSFetchRequest<PostEntity> = PostEntity.fetchRequest()
            request.predicate = NSPredicate(format: "author == %@", author)
            request.sortDescriptors = [NSSortDescriptor(keyPath: \PostEntity.createdAt, ascending: false)]
            
            return try context.fetch(request)
        }
    }
    
    // MARK: - Data Refresh
    
    func refreshData() {
        isLoading = true
        error = nil
        
        Publishers.Zip(fetchUsers(), fetchPosts())
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
}

// MARK: - Data Migration

/**
 * Core Data migration manager
 * 
 * This class demonstrates proper data migration handling
 * with version management and error recovery
 */
class CoreDataMigrationManager {
    
    // MARK: - Properties
    
    private let coreDataStack: CoreDataStack
    
    // MARK: - Initialization
    
    init(coreDataStack: CoreDataStack = .shared) {
        self.coreDataStack = coreDataStack
    }
    
    // MARK: - Migration Methods
    
    func migrateIfNeeded() -> AnyPublisher<Bool, Error> {
        return Future<Bool, Error> { promise in
            do {
                let migrationNeeded = try self.checkMigrationNeeded()
                if migrationNeeded {
                    try self.performMigration()
                }
                promise(.success(migrationNeeded))
            } catch {
                promise(.failure(error))
            }
        }
        .eraseToAnyPublisher()
    }
    
    private func checkMigrationNeeded() throws -> Bool {
        // Check if migration is needed
        // This is a simplified implementation
        // In production, you would check the store metadata
        return false
    }
    
    private func performMigration() throws {
        // Perform data migration
        // This is a simplified implementation
        // In production, you would use NSMigrationManager
        print("Performing data migration...")
    }
}

// MARK: - Performance Optimization

/**
 * Core Data performance utilities
 * 
 * This class demonstrates performance optimization techniques
 * for Core Data operations
 */
class CoreDataPerformanceOptimizer {
    
    // MARK: - Batch Operations
    
    static func batchInsert<T: NSManagedObject>(
        entities: [T.Type],
        count: Int,
        context: NSManagedObjectContext,
        block: @escaping (NSManagedObjectContext, Int) -> Void
    ) -> AnyPublisher<Void, Error> {
        return Future<Void, Error> { promise in
            do {
                let batchInsert = NSBatchInsertRequest(entity: T.entity()) { managedObject, index in
                    block(context, index)
                }
                batchInsert.resultType = .objectIDs
                
                let result = try context.execute(batchInsert) as? NSBatchInsertResult
                let objectIDs = result?.result as? [NSManagedObjectID] ?? []
                
                // Merge changes to view context
                NSManagedObjectContext.mergeChanges(fromRemoteContextSave: [NSInsertedObjectsKey: objectIDs], into: [context])
                
                promise(.success(()))
            } catch {
                promise(.failure(error))
            }
        }
        .eraseToAnyPublisher()
    }
    
    static func batchUpdate(
        entityName: String,
        predicate: NSPredicate,
        propertiesToUpdate: [String: Any],
        context: NSManagedObjectContext
    ) -> AnyPublisher<Void, Error> {
        return Future<Void, Error> { promise in
            do {
                let batchUpdate = NSBatchUpdateRequest(entityName: entityName)
                batchUpdate.predicate = predicate
                batchUpdate.propertiesToUpdate = propertiesToUpdate
                batchUpdate.resultType = .updatedObjectIDsResultType
                
                let result = try context.execute(batchUpdate) as? NSBatchUpdateResult
                let objectIDs = result?.result as? [NSManagedObjectID] ?? []
                
                // Merge changes to view context
                NSManagedObjectContext.mergeChanges(fromRemoteContextSave: [NSUpdatedObjectsKey: objectIDs], into: [context])
                
                promise(.success(()))
            } catch {
                promise(.failure(error))
            }
        }
        .eraseToAnyPublisher()
    }
    
    static func batchDelete(
        entityName: String,
        predicate: NSPredicate,
        context: NSManagedObjectContext
    ) -> AnyPublisher<Void, Error> {
        return Future<Void, Error> { promise in
            do {
                let batchDelete = NSBatchDeleteRequest(fetchRequest: NSFetchRequest<NSFetchRequestResult>(entityName: entityName))
                batchDelete.predicate = predicate
                batchDelete.resultType = .resultTypeObjectIDs
                
                let result = try context.execute(batchDelete) as? NSBatchDeleteResult
                let objectIDs = result?.result as? [NSManagedObjectID] ?? []
                
                // Merge changes to view context
                NSManagedObjectContext.mergeChanges(fromRemoteContextSave: [NSDeletedObjectsKey: objectIDs], into: [context])
                
                promise(.success(()))
            } catch {
                promise(.failure(error))
            }
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Fetch Optimization
    
    static func optimizedFetch<T: NSManagedObject>(
        entity: T.Type,
        predicate: NSPredicate? = nil,
        sortDescriptors: [NSSortDescriptor] = [],
        limit: Int? = nil,
        context: NSManagedObjectContext
    ) -> AnyPublisher<[T], Error> {
        return Future<[T], Error> { promise in
            do {
                let request = NSFetchRequest<T>(entityName: String(describing: entity))
                request.predicate = predicate
                request.sortDescriptors = sortDescriptors
                request.fetchLimit = limit ?? 0
                request.returnsObjectsAsFaults = false
                request.relationshipKeyPathsForPrefetching = ["author", "comments"]
                
                let results = try context.fetch(request)
                promise(.success(results))
            } catch {
                promise(.failure(error))
            }
        }
        .eraseToAnyPublisher()
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use Core Data patterns
 * 
 * This function shows practical usage of all the Core Data components
 */
func demonstrateCoreData() {
    print("=== Core Data Demonstration ===\n")
    
    // Create Core Data stack
    let coreDataStack = CoreDataStack.shared
    print("--- Core Data Stack ---")
    print("Persistent container: \(type(of: coreDataStack.persistentContainer))")
    print("View context: \(type(of: coreDataStack.viewContext))")
    print("Background context: \(type(of: coreDataStack.backgroundContext))")
    
    // Create data manager
    let dataManager = CoreDataManager(coreDataStack: coreDataStack)
    print("\n--- Data Manager ---")
    print("Data manager: \(type(of: dataManager))")
    print("Published properties: users, posts, isLoading, error")
    
    // Demonstrate user operations
    print("\n--- User Operations ---")
    print("Create user: Available")
    print("Update user: Available")
    print("Delete user: Available")
    print("Fetch users: Available")
    print("Search users: Available")
    
    // Demonstrate post operations
    print("\n--- Post Operations ---")
    print("Create post: Available")
    print("Update post: Available")
    print("Delete post: Available")
    print("Fetch posts: Available")
    print("Fetch posts by author: Available")
    
    // Demonstrate migration
    let migrationManager = CoreDataMigrationManager(coreDataStack: coreDataStack)
    print("\n--- Data Migration ---")
    print("Migration manager: \(type(of: migrationManager))")
    print("Migration check: Available")
    print("Migration execution: Available")
    
    // Demonstrate performance optimization
    print("\n--- Performance Optimization ---")
    print("Batch insert: Available")
    print("Batch update: Available")
    print("Batch delete: Available")
    print("Optimized fetch: Available")
    
    // Demonstrate entity extensions
    print("\n--- Entity Extensions ---")
    print("User entity: Computed properties, business logic")
    print("Post entity: Computed properties, business logic")
    print("Relationships: Author, comments")
    
    // Demonstrate background processing
    print("\n--- Background Processing ---")
    print("Background context: Available")
    print("Background tasks: Available")
    print("Context merging: Available")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateCoreData()
