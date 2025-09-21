/*
 * Swift Testing: Integration Testing
 * 
 * This file demonstrates production-grade integration testing strategies in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master API testing and network layer integration
 * - Understand database testing and persistence layer testing
 * - Implement proper component integration testing
 * - Apply end-to-end testing strategies
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import XCTest
import Foundation
import CoreData

// MARK: - API Integration Testing

/**
 * API integration test manager
 * 
 * This class demonstrates proper API integration testing
 * with real network calls and validation
 */
class APIIntegrationTestManager {
    
    // MARK: - Properties
    
    private let session: URLSession
    private let baseURL: URL
    private let timeout: TimeInterval
    
    // MARK: - Initialization
    
    init(baseURL: URL, timeout: TimeInterval = 30.0) {
        self.baseURL = baseURL
        self.timeout = timeout
        
        let config = URLSessionConfiguration.default
        config.timeoutIntervalForRequest = timeout
        config.timeoutIntervalForResource = timeout
        self.session = URLSession(configuration: config)
    }
    
    // MARK: - Public Methods
    
    /**
     * Test API endpoint
     * 
     * This method demonstrates proper API endpoint testing
     * with comprehensive validation
     */
    func testAPIEndpoint<T: Codable>(
        path: String,
        method: HTTPMethod = .GET,
        expectedResponseType: T.Type,
        expectedStatusCode: Int = 200,
        headers: [String: String] = [:],
        body: Data? = nil
    ) -> AnyPublisher<APITestResult<T>, Error> {
        return createRequest(path: path, method: method, headers: headers, body: body)
            .flatMap { request in
                self.performRequest(request, expectedResponseType: expectedResponseType, expectedStatusCode: expectedStatusCode)
            }
            .eraseToAnyPublisher()
    }
    
    /**
     * Test API authentication
     * 
     * This method demonstrates proper API authentication testing
     * with token validation
     */
    func testAPIAuthentication(
        username: String,
        password: String
    ) -> AnyPublisher<AuthenticationResult, Error> {
        let loginData = ["username": username, "password": password]
        let jsonData = try! JSONEncoder().encode(loginData)
        
        return testAPIEndpoint(
            path: "/auth/login",
            method: .POST,
            expectedResponseType: AuthenticationResponse.self,
            body: jsonData
        )
        .map { result in
            AuthenticationResult(
                success: result.success,
                token: result.data?.token,
                user: result.data?.user
            )
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func createRequest(
        path: String,
        method: HTTPMethod,
        headers: [String: String],
        body: Data?
    ) -> AnyPublisher<URLRequest, Error> {
        return Future<URLRequest, Error> { promise in
            guard let url = URL(string: path, relativeTo: self.baseURL) else {
                promise(.failure(APITestError.invalidURL))
                return
            }
            
            var request = URLRequest(url: url)
            request.httpMethod = method.rawValue
            request.httpBody = body
            
            for (key, value) in headers {
                request.setValue(value, forHTTPHeaderField: key)
            }
            
            promise(.success(request))
        }
        .eraseToAnyPublisher()
    }
    
    private func performRequest<T: Codable>(
        _ request: URLRequest,
        expectedResponseType: T.Type,
        expectedStatusCode: Int
    ) -> AnyPublisher<APITestResult<T>, Error> {
        return session.dataTaskPublisher(for: request)
            .map { data, response in
                let httpResponse = response as! HTTPURLResponse
                let statusCode = httpResponse.statusCode
                
                let success = statusCode == expectedStatusCode
                var decodedData: T?
                var error: Error?
                
                if success {
                    do {
                        decodedData = try JSONDecoder().decode(T.self, from: data)
                    } catch {
                        error = error
                    }
                } else {
                    error = APITestError.unexpectedStatusCode(statusCode)
                }
                
                return APITestResult(
                    success: success,
                    statusCode: statusCode,
                    data: decodedData,
                    error: error
                )
            }
            .eraseToAnyPublisher()
    }
}

// MARK: - Database Integration Testing

/**
 * Core Data integration test manager
 * 
 * This class demonstrates proper Core Data integration testing
 * with in-memory store and validation
 */
class CoreDataIntegrationTestManager {
    
    // MARK: - Properties
    
    private var persistentContainer: NSPersistentContainer!
    private var context: NSManagedObjectContext!
    
    // MARK: - Setup Methods
    
    /**
     * Setup in-memory Core Data stack
     * 
     * This method demonstrates proper Core Data test setup
     * with in-memory store configuration
     */
    func setupInMemoryCoreDataStack() throws {
        persistentContainer = NSPersistentContainer(name: "TestDataModel")
        
        let description = NSPersistentStoreDescription()
        description.type = NSInMemoryStoreType
        description.shouldAddStoreAsynchronously = false
        
        persistentContainer.persistentStoreDescriptions = [description]
        
        let expectation = XCTestExpectation(description: "Core Data stack setup")
        var setupError: Error?
        
        persistentContainer.loadPersistentStores { _, error in
            setupError = error
            expectation.fulfill()
        }
        
        wait(for: [expectation], timeout: 5.0)
        
        if let error = setupError {
            throw error
        }
        
        context = persistentContainer.viewContext
        context.automaticallyMergesChangesFromParent = true
    }
    
    /**
     * Cleanup Core Data stack
     * 
     * This method demonstrates proper Core Data test cleanup
     * with resource management
     */
    func cleanupCoreDataStack() {
        context = nil
        persistentContainer = nil
    }
    
    // MARK: - Test Methods
    
    /**
     * Test entity creation
     * 
     * This method demonstrates proper entity creation testing
     * with Core Data validation
     */
    func testEntityCreation<T: NSManagedObject>(
        entityType: T.Type,
        properties: [String: Any]
    ) throws -> T {
        let entity = NSEntityDescription.entity(forEntityName: String(describing: entityType), in: context)!
        let object = T(entity: entity, insertInto: context)
        
        for (key, value) in properties {
            object.setValue(value, forKey: key)
        }
        
        try context.save()
        
        return object
    }
    
    /**
     * Test entity fetching
     * 
     * This method demonstrates proper entity fetching testing
     * with Core Data queries
     */
    func testEntityFetching<T: NSManagedObject>(
        entityType: T.Type,
        predicate: NSPredicate? = nil,
        sortDescriptors: [NSSortDescriptor] = []
    ) throws -> [T] {
        let request = NSFetchRequest<T>(entityName: String(describing: entityType))
        request.predicate = predicate
        request.sortDescriptors = sortDescriptors
        
        return try context.fetch(request)
    }
    
    /**
     * Test entity deletion
     * 
     * This method demonstrates proper entity deletion testing
     * with Core Data validation
     */
    func testEntityDeletion<T: NSManagedObject>(_ object: T) throws {
        context.delete(object)
        try context.save()
    }
    
    /**
     * Test entity relationships
     * 
     * This method demonstrates proper relationship testing
     * with Core Data validation
     */
    func testEntityRelationships<T: NSManagedObject, U: NSManagedObject>(
        parentType: T.Type,
        childType: U.Type,
        relationshipName: String,
        parentProperties: [String: Any],
        childProperties: [String: Any]
    ) throws -> (parent: T, child: U) {
        let parent = try testEntityCreation(entityType: parentType, properties: parentProperties)
        let child = try testEntityCreation(entityType: childType, properties: childProperties)
        
        parent.setValue(child, forKey: relationshipName)
        try context.save()
        
        return (parent: parent, child: child)
    }
}

// MARK: - Component Integration Testing

/**
 * Component integration test manager
 * 
 * This class demonstrates proper component integration testing
 * with real component interactions
 */
class ComponentIntegrationTestManager {
    
    // MARK: - Properties
    
    private var components: [String: Any] = [:]
    
    // MARK: - Public Methods
    
    /**
     * Register component
     * 
     * This method demonstrates proper component registration
     * for integration testing
     */
    func registerComponent<T>(_ component: T, forKey key: String) {
        components[key] = component
    }
    
    /**
     * Get component
     * 
     * This method demonstrates proper component retrieval
     * for integration testing
     */
    func getComponent<T>(forKey key: String, as type: T.Type) -> T? {
        return components[key] as? T
    }
    
    /**
     * Test component interaction
     * 
     * This method demonstrates proper component interaction testing
     * with real component communication
     */
    func testComponentInteraction<T, U>(
        fromComponentKey: String,
        toComponentKey: String,
        input: T,
        expectedOutput: U,
        interaction: (T) -> U
    ) throws {
        let result = interaction(input)
        
        if let expected = expectedOutput as? U, let actual = result as? U {
            XCTAssertEqual(actual, expected, "Component interaction should produce expected output")
        } else {
            throw ComponentIntegrationTestError.unexpectedOutput
        }
    }
    
    /**
     * Test component lifecycle
     * 
     * This method demonstrates proper component lifecycle testing
     * with initialization and cleanup
     */
    func testComponentLifecycle<T: ComponentProtocol>(
        componentType: T.Type,
        configuration: T.Configuration
    ) throws -> T {
        let component = try componentType.initialize(with: configuration)
        
        // Test component functionality
        try component.testFunctionality()
        
        // Test component cleanup
        try component.cleanup()
        
        return component
    }
}

// MARK: - End-to-End Testing

/**
 * End-to-end test manager
 * 
 * This class demonstrates proper end-to-end testing
 * with complete application flow testing
 */
class EndToEndTestManager {
    
    // MARK: - Properties
    
    private let apiManager: APIIntegrationTestManager
    private let databaseManager: CoreDataIntegrationTestManager
    private let componentManager: ComponentIntegrationTestManager
    
    // MARK: - Initialization
    
    init(
        apiManager: APIIntegrationTestManager,
        databaseManager: CoreDataIntegrationTestManager,
        componentManager: ComponentIntegrationTestManager
    ) {
        self.apiManager = apiManager
        self.databaseManager = databaseManager
        self.componentManager = componentManager
    }
    
    // MARK: - Public Methods
    
    /**
     * Test complete user flow
     * 
     * This method demonstrates proper end-to-end testing
     * with complete user journey validation
     */
    func testCompleteUserFlow() -> AnyPublisher<UserFlowTestResult, Error> {
        return Future<UserFlowTestResult, Error> { promise in
            // Step 1: User registration
            self.testUserRegistration()
                .flatMap { registrationResult in
                    // Step 2: User login
                    self.testUserLogin(credentials: registrationResult.credentials)
                }
                .flatMap { loginResult in
                    // Step 3: User data fetching
                    self.testUserDataFetching(token: loginResult.token)
                }
                .flatMap { userData in
                    // Step 4: User data updating
                    self.testUserDataUpdating(user: userData, token: loginResult.token)
                }
                .flatMap { updatedUser in
                    // Step 5: User data persistence
                    self.testUserDataPersistence(user: updatedUser)
                }
                .sink(
                    receiveCompletion: { completion in
                        if case .failure(let error) = completion {
                            promise(.failure(error))
                        }
                    },
                    receiveValue: { result in
                        promise(.success(result))
                    }
                )
                .store(in: &self.cancellables)
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func testUserRegistration() -> AnyPublisher<RegistrationResult, Error> {
        let registrationData = [
            "username": "testuser",
            "email": "test@example.com",
            "password": "testpassword"
        ]
        let jsonData = try! JSONEncoder().encode(registrationData)
        
        return apiManager.testAPIEndpoint(
            path: "/users/register",
            method: .POST,
            expectedResponseType: RegistrationResponse.self,
            body: jsonData
        )
        .map { result in
            RegistrationResult(
                success: result.success,
                credentials: UserCredentials(
                    username: "testuser",
                    password: "testpassword"
                )
            )
        }
        .eraseToAnyPublisher()
    }
    
    private func testUserLogin(credentials: UserCredentials) -> AnyPublisher<LoginResult, Error> {
        return apiManager.testAPIAuthentication(
            username: credentials.username,
            password: credentials.password
        )
        .map { result in
            LoginResult(
                success: result.success,
                token: result.token ?? ""
            )
        }
        .eraseToAnyPublisher()
    }
    
    private func testUserDataFetching(token: String) -> AnyPublisher<User, Error> {
        let headers = ["Authorization": "Bearer \(token)"]
        
        return apiManager.testAPIEndpoint(
            path: "/users/profile",
            method: .GET,
            expectedResponseType: User.self,
            headers: headers
        )
        .map { result in
            result.data!
        }
        .eraseToAnyPublisher()
    }
    
    private func testUserDataUpdating(user: User, token: String) -> AnyPublisher<User, Error> {
        let headers = ["Authorization": "Bearer \(token)"]
        let updatedUser = User(
            id: user.id,
            name: "Updated \(user.name)",
            email: user.email
        )
        let jsonData = try! JSONEncoder().encode(updatedUser)
        
        return apiManager.testAPIEndpoint(
            path: "/users/profile",
            method: .PUT,
            expectedResponseType: User.self,
            headers: headers,
            body: jsonData
        )
        .map { result in
            result.data!
        }
        .eraseToAnyPublisher()
    }
    
    private func testUserDataPersistence(user: User) -> AnyPublisher<UserFlowTestResult, Error> {
        return Future<UserFlowTestResult, Error> { promise in
            do {
                // Save user to database
                try self.databaseManager.testEntityCreation(
                    entityType: UserEntity.self,
                    properties: [
                        "id": user.id,
                        "name": user.name,
                        "email": user.email
                    ]
                )
                
                // Verify user was saved
                let savedUsers = try self.databaseManager.testEntityFetching(
                    entityType: UserEntity.self,
                    predicate: NSPredicate(format: "id == %d", user.id)
                )
                
                if savedUsers.count == 1 {
                    promise(.success(UserFlowTestResult(success: true, message: "User flow completed successfully")))
                } else {
                    promise(.failure(EndToEndTestError.persistenceFailed))
                }
            } catch {
                promise(.failure(error))
            }
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Properties
    
    private var cancellables = Set<AnyCancellable>()
}

// MARK: - Supporting Types

/**
 * HTTP method enumeration
 * 
 * This enum demonstrates proper HTTP method modeling
 * for API testing
 */
enum HTTPMethod: String {
    case GET = "GET"
    case POST = "POST"
    case PUT = "PUT"
    case DELETE = "DELETE"
    case PATCH = "PATCH"
}

/**
 * API test result
 * 
 * This struct demonstrates proper API test result modeling
 * for integration testing
 */
struct APITestResult<T> {
    let success: Bool
    let statusCode: Int
    let data: T?
    let error: Error?
}

/**
 * Authentication result
 * 
 * This struct demonstrates proper authentication result modeling
 * for API testing
 */
struct AuthenticationResult {
    let success: Bool
    let token: String?
    let user: User?
}

/**
 * Authentication response
 * 
 * This struct demonstrates proper authentication response modeling
 * for API testing
 */
struct AuthenticationResponse: Codable {
    let token: String
    let user: User
}

/**
 * Registration result
 * 
 * This struct demonstrates proper registration result modeling
 * for end-to-end testing
 */
struct RegistrationResult {
    let success: Bool
    let credentials: UserCredentials
}

/**
 * Registration response
 * 
 * This struct demonstrates proper registration response modeling
 * for API testing
 */
struct RegistrationResponse: Codable {
    let success: Bool
    let message: String
}

/**
 * User credentials
 * 
 * This struct demonstrates proper user credentials modeling
 * for testing
 */
struct UserCredentials {
    let username: String
    let password: String
}

/**
 * Login result
 * 
 * This struct demonstrates proper login result modeling
 * for end-to-end testing
 */
struct LoginResult {
    let success: Bool
    let token: String
}

/**
 * User flow test result
 * 
 * This struct demonstrates proper user flow test result modeling
 * for end-to-end testing
 */
struct UserFlowTestResult {
    let success: Bool
    let message: String
}

/**
 * Component protocol
 * 
 * This protocol demonstrates proper component protocol design
 * for integration testing
 */
protocol ComponentProtocol {
    associatedtype Configuration
    
    static func initialize(with configuration: Configuration) throws -> Self
    func testFunctionality() throws
    func cleanup() throws
}

/**
 * User entity
 * 
 * This class demonstrates proper Core Data entity modeling
 * for database testing
 */
class UserEntity: NSManagedObject {
    @NSManaged var id: Int32
    @NSManaged var name: String
    @NSManaged var email: String
}

/**
 * API test error types
 * 
 * This enum demonstrates proper error modeling
 * for API testing
 */
enum APITestError: Error, LocalizedError {
    case invalidURL
    case unexpectedStatusCode(Int)
    case networkError(Error)
    
    var errorDescription: String? {
        switch self {
        case .invalidURL:
            return "Invalid URL provided"
        case .unexpectedStatusCode(let code):
            return "Unexpected status code: \(code)"
        case .networkError(let error):
            return "Network error: \(error.localizedDescription)"
        }
    }
}

/**
 * Component integration test error types
 * 
 * This enum demonstrates proper error modeling
 * for component integration testing
 */
enum ComponentIntegrationTestError: Error, LocalizedError {
    case unexpectedOutput
    case componentNotFound
    case interactionFailed
    
    var errorDescription: String? {
        switch self {
        case .unexpectedOutput:
            return "Component interaction produced unexpected output"
        case .componentNotFound:
            return "Component not found"
        case .interactionFailed:
            return "Component interaction failed"
        }
    }
}

/**
 * End-to-end test error types
 * 
 * This enum demonstrates proper error modeling
 * for end-to-end testing
 */
enum EndToEndTestError: Error, LocalizedError {
    case persistenceFailed
    case flowInterrupted
    case validationFailed
    
    var errorDescription: String? {
        switch self {
        case .persistenceFailed:
            return "Data persistence failed"
        case .flowInterrupted:
            return "User flow was interrupted"
        case .validationFailed:
            return "Flow validation failed"
        }
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use integration testing strategies
 * 
 * This function shows practical usage of all the integration testing components
 */
func demonstrateIntegrationTesting() {
    print("=== Integration Testing Demonstration ===\n")
    
    // API Integration Testing
    let apiManager = APIIntegrationTestManager(
        baseURL: URL(string: "https://api.example.com")!,
        timeout: 30.0
    )
    print("--- API Integration Testing ---")
    print("API Manager: \(type(of: apiManager))")
    print("Features: Real network calls, response validation, authentication testing")
    
    // Database Integration Testing
    let databaseManager = CoreDataIntegrationTestManager()
    print("\n--- Database Integration Testing ---")
    print("Database Manager: \(type(of: databaseManager))")
    print("Features: In-memory store, entity testing, relationship validation")
    
    // Component Integration Testing
    let componentManager = ComponentIntegrationTestManager()
    print("\n--- Component Integration Testing ---")
    print("Component Manager: \(type(of: componentManager))")
    print("Features: Component registration, interaction testing, lifecycle testing")
    
    // End-to-End Testing
    let endToEndManager = EndToEndTestManager(
        apiManager: apiManager,
        databaseManager: databaseManager,
        componentManager: componentManager
    )
    print("\n--- End-to-End Testing ---")
    print("End-to-End Manager: \(type(of: endToEndManager))")
    print("Features: Complete user flow testing, full application validation")
    
    // Demonstrate testing techniques
    print("\n--- Testing Techniques ---")
    print("API Testing: Network layer integration, response validation")
    print("Database Testing: Persistence layer testing, data validation")
    print("Component Testing: Component interaction, lifecycle testing")
    print("End-to-End Testing: Complete flow validation, user journey testing")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Test real integrations, not just mocks")
    print("2. Use test databases and environments")
    print("3. Test complete user flows")
    print("4. Validate data persistence")
    print("5. Test error conditions and edge cases")
    print("6. Use appropriate test data")
    print("7. Clean up after tests")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateIntegrationTesting()
