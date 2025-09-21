/*
 * Swift Testing: Unit Testing
 * 
 * This file demonstrates production-grade unit testing strategies in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master Test-Driven Development (TDD) principles
 * - Understand mocking and stubbing techniques
 * - Implement proper test coverage and organization
 * - Apply advanced testing patterns and best practices
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import XCTest
import Foundation

// MARK: - Test-Driven Development Example

/**
 * Example of TDD implementation
 * 
 * This class demonstrates proper TDD practices
 * with red-green-refactor cycle
 */
class Calculator {
    
    // MARK: - Properties
    
    private var history: [String] = []
    
    // MARK: - Public Methods
    
    /**
     * Add two numbers
     * 
     * This method demonstrates proper TDD implementation
     * with test-first development
     */
    func add(_ a: Double, _ b: Double) -> Double {
        let result = a + b
        history.append("\(a) + \(b) = \(result)")
        return result
    }
    
    /**
     * Subtract two numbers
     * 
     * This method demonstrates proper TDD implementation
     * with test-first development
     */
    func subtract(_ a: Double, _ b: Double) -> Double {
        let result = a - b
        history.append("\(a) - \(b) = \(result)")
        return result
    }
    
    /**
     * Multiply two numbers
     * 
     * This method demonstrates proper TDD implementation
     * with test-first development
     */
    func multiply(_ a: Double, _ b: Double) -> Double {
        let result = a * b
        history.append("\(a) * \(b) = \(result)")
        return result
    }
    
    /**
     * Divide two numbers
     * 
     * This method demonstrates proper TDD implementation
     * with test-first development
     */
    func divide(_ a: Double, _ b: Double) throws -> Double {
        guard b != 0 else {
            throw CalculatorError.divisionByZero
        }
        
        let result = a / b
        history.append("\(a) / \(b) = \(result)")
        return result
    }
    
    /**
     * Get calculation history
     * 
     * This method demonstrates proper TDD implementation
     * with test-first development
     */
    func getHistory() -> [String] {
        return history
    }
    
    /**
     * Clear calculation history
     * 
     * This method demonstrates proper TDD implementation
     * with test-first development
     */
    func clearHistory() {
        history.removeAll()
    }
}

// MARK: - Calculator Error

/**
 * Calculator error types
 * 
 * This enum demonstrates proper error modeling
 * for unit testing
 */
enum CalculatorError: Error, Equatable {
    case divisionByZero
    case invalidInput
}

// MARK: - Unit Tests

/**
 * Calculator unit tests
 * 
 * This class demonstrates production-grade unit testing
 * with comprehensive test coverage
 */
class CalculatorTests: XCTestCase {
    
    // MARK: - Properties
    
    var calculator: Calculator!
    
    // MARK: - Setup and Teardown
    
    override func setUp() {
        super.setUp()
        calculator = Calculator()
    }
    
    override func tearDown() {
        calculator = nil
        super.tearDown()
    }
    
    // MARK: - Addition Tests
    
    func testAddPositiveNumbers() {
        // Given
        let a = 5.0
        let b = 3.0
        
        // When
        let result = calculator.add(a, b)
        
        // Then
        XCTAssertEqual(result, 8.0, "Addition of positive numbers should be correct")
    }
    
    func testAddNegativeNumbers() {
        // Given
        let a = -5.0
        let b = -3.0
        
        // When
        let result = calculator.add(a, b)
        
        // Then
        XCTAssertEqual(result, -8.0, "Addition of negative numbers should be correct")
    }
    
    func testAddZero() {
        // Given
        let a = 5.0
        let b = 0.0
        
        // When
        let result = calculator.add(a, b)
        
        // Then
        XCTAssertEqual(result, 5.0, "Addition with zero should return the other number")
    }
    
    // MARK: - Subtraction Tests
    
    func testSubtractPositiveNumbers() {
        // Given
        let a = 5.0
        let b = 3.0
        
        // When
        let result = calculator.subtract(a, b)
        
        // Then
        XCTAssertEqual(result, 2.0, "Subtraction of positive numbers should be correct")
    }
    
    func testSubtractNegativeNumbers() {
        // Given
        let a = -5.0
        let b = -3.0
        
        // When
        let result = calculator.subtract(a, b)
        
        // Then
        XCTAssertEqual(result, -2.0, "Subtraction of negative numbers should be correct")
    }
    
    func testSubtractZero() {
        // Given
        let a = 5.0
        let b = 0.0
        
        // When
        let result = calculator.subtract(a, b)
        
        // Then
        XCTAssertEqual(result, 5.0, "Subtraction with zero should return the other number")
    }
    
    // MARK: - Multiplication Tests
    
    func testMultiplyPositiveNumbers() {
        // Given
        let a = 5.0
        let b = 3.0
        
        // When
        let result = calculator.multiply(a, b)
        
        // Then
        XCTAssertEqual(result, 15.0, "Multiplication of positive numbers should be correct")
    }
    
    func testMultiplyNegativeNumbers() {
        // Given
        let a = -5.0
        let b = -3.0
        
        // When
        let result = calculator.multiply(a, b)
        
        // Then
        XCTAssertEqual(result, 15.0, "Multiplication of negative numbers should be correct")
    }
    
    func testMultiplyByZero() {
        // Given
        let a = 5.0
        let b = 0.0
        
        // When
        let result = calculator.multiply(a, b)
        
        // Then
        XCTAssertEqual(result, 0.0, "Multiplication by zero should return zero")
    }
    
    // MARK: - Division Tests
    
    func testDividePositiveNumbers() throws {
        // Given
        let a = 15.0
        let b = 3.0
        
        // When
        let result = try calculator.divide(a, b)
        
        // Then
        XCTAssertEqual(result, 5.0, "Division of positive numbers should be correct")
    }
    
    func testDivideNegativeNumbers() throws {
        // Given
        let a = -15.0
        let b = -3.0
        
        // When
        let result = try calculator.divide(a, b)
        
        // Then
        XCTAssertEqual(result, 5.0, "Division of negative numbers should be correct")
    }
    
    func testDivideByZero() {
        // Given
        let a = 5.0
        let b = 0.0
        
        // When & Then
        XCTAssertThrowsError(try calculator.divide(a, b)) { error in
            XCTAssertEqual(error as? CalculatorError, .divisionByZero)
        }
    }
    
    // MARK: - History Tests
    
    func testHistoryRecording() {
        // Given
        let a = 5.0
        let b = 3.0
        
        // When
        calculator.add(a, b)
        calculator.subtract(a, b)
        calculator.multiply(a, b)
        
        // Then
        let history = calculator.getHistory()
        XCTAssertEqual(history.count, 3, "History should record all operations")
        XCTAssertTrue(history.contains("5.0 + 3.0 = 8.0"), "History should contain addition")
        XCTAssertTrue(history.contains("5.0 - 3.0 = 2.0"), "History should contain subtraction")
        XCTAssertTrue(history.contains("5.0 * 3.0 = 15.0"), "History should contain multiplication")
    }
    
    func testClearHistory() {
        // Given
        calculator.add(5.0, 3.0)
        calculator.subtract(5.0, 3.0)
        
        // When
        calculator.clearHistory()
        
        // Then
        let history = calculator.getHistory()
        XCTAssertTrue(history.isEmpty, "History should be empty after clearing")
    }
}

// MARK: - Mocking and Stubbing

/**
 * Network service protocol
 * 
 * This protocol demonstrates proper protocol design
 * for mocking and testing
 */
protocol NetworkServiceProtocol {
    func fetchData(from url: URL) -> AnyPublisher<Data, Error>
    func postData(_ data: Data, to url: URL) -> AnyPublisher<Data, Error>
}

/**
 * Mock network service
 * 
 * This class demonstrates proper mocking implementation
 * for unit testing
 */
class MockNetworkService: NetworkServiceProtocol {
    
    // MARK: - Properties
    
    var fetchDataResult: Result<Data, Error> = .success(Data())
    var postDataResult: Result<Data, Error> = .success(Data())
    var fetchDataCallCount = 0
    var postDataCallCount = 0
    var lastFetchURL: URL?
    var lastPostURL: URL?
    var lastPostData: Data?
    
    // MARK: - NetworkServiceProtocol
    
    func fetchData(from url: URL) -> AnyPublisher<Data, Error> {
        fetchDataCallCount += 1
        lastFetchURL = url
        
        return Future<Data, Error> { promise in
            promise(self.fetchDataResult)
        }
        .eraseToAnyPublisher()
    }
    
    func postData(_ data: Data, to url: URL) -> AnyPublisher<Data, Error> {
        postDataCallCount += 1
        lastPostURL = url
        lastPostData = data
        
        return Future<Data, Error> { promise in
            promise(self.postDataResult)
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Mock Methods
    
    func reset() {
        fetchDataResult = .success(Data())
        postDataResult = .success(Data())
        fetchDataCallCount = 0
        postDataCallCount = 0
        lastFetchURL = nil
        lastPostURL = nil
        lastPostData = nil
    }
}

/**
 * Data manager with network dependency
 * 
 * This class demonstrates proper dependency injection
 * for unit testing
 */
class DataManager {
    
    // MARK: - Properties
    
    private let networkService: NetworkServiceProtocol
    
    // MARK: - Initialization
    
    init(networkService: NetworkServiceProtocol) {
        self.networkService = networkService
    }
    
    // MARK: - Public Methods
    
    func fetchUserData(from url: URL) -> AnyPublisher<User, Error> {
        return networkService.fetchData(from: url)
            .decode(type: User.self, decoder: JSONDecoder())
            .eraseToAnyPublisher()
    }
    
    func updateUserData(_ user: User, to url: URL) -> AnyPublisher<User, Error> {
        let data = try! JSONEncoder().encode(user)
        return networkService.postData(data, to: url)
            .decode(type: User.self, decoder: JSONDecoder())
            .eraseToAnyPublisher()
    }
}

/**
 * User model
 * 
 * This struct demonstrates proper model design
 * for unit testing
 */
struct User: Codable, Equatable {
    let id: Int
    let name: String
    let email: String
}

/**
 * Data manager unit tests
 * 
 * This class demonstrates proper unit testing
 * with mocking and dependency injection
 */
class DataManagerTests: XCTestCase {
    
    // MARK: - Properties
    
    var dataManager: DataManager!
    var mockNetworkService: MockNetworkService!
    
    // MARK: - Setup and Teardown
    
    override func setUp() {
        super.setUp()
        mockNetworkService = MockNetworkService()
        dataManager = DataManager(networkService: mockNetworkService)
    }
    
    override func tearDown() {
        dataManager = nil
        mockNetworkService = nil
        super.tearDown()
    }
    
    // MARK: - Fetch User Data Tests
    
    func testFetchUserDataSuccess() {
        // Given
        let url = URL(string: "https://api.example.com/users/1")!
        let userData = """
        {
            "id": 1,
            "name": "John Doe",
            "email": "john@example.com"
        }
        """.data(using: .utf8)!
        mockNetworkService.fetchDataResult = .success(userData)
        
        // When
        let expectation = XCTestExpectation(description: "Fetch user data")
        var result: User?
        var error: Error?
        
        dataManager.fetchUserData(from: url)
            .sink(
                receiveCompletion: { completion in
                    if case .failure(let err) = completion {
                        error = err
                    }
                    expectation.fulfill()
                },
                receiveValue: { user in
                    result = user
                }
            )
            .store(in: &cancellables)
        
        // Then
        wait(for: [expectation], timeout: 1.0)
        XCTAssertNil(error, "Should not have error")
        XCTAssertNotNil(result, "Should have user data")
        XCTAssertEqual(result?.id, 1, "User ID should be correct")
        XCTAssertEqual(result?.name, "John Doe", "User name should be correct")
        XCTAssertEqual(result?.email, "john@example.com", "User email should be correct")
        XCTAssertEqual(mockNetworkService.fetchDataCallCount, 1, "Should call fetch data once")
        XCTAssertEqual(mockNetworkService.lastFetchURL, url, "Should use correct URL")
    }
    
    func testFetchUserDataFailure() {
        // Given
        let url = URL(string: "https://api.example.com/users/1")!
        let networkError = NSError(domain: "NetworkError", code: 404, userInfo: nil)
        mockNetworkService.fetchDataResult = .failure(networkError)
        
        // When
        let expectation = XCTestExpectation(description: "Fetch user data failure")
        var result: User?
        var error: Error?
        
        dataManager.fetchUserData(from: url)
            .sink(
                receiveCompletion: { completion in
                    if case .failure(let err) = completion {
                        error = err
                    }
                    expectation.fulfill()
                },
                receiveValue: { user in
                    result = user
                }
            )
            .store(in: &cancellables)
        
        // Then
        wait(for: [expectation], timeout: 1.0)
        XCTAssertNotNil(error, "Should have error")
        XCTAssertNil(result, "Should not have user data")
        XCTAssertEqual(mockNetworkService.fetchDataCallCount, 1, "Should call fetch data once")
    }
    
    // MARK: - Update User Data Tests
    
    func testUpdateUserDataSuccess() {
        // Given
        let url = URL(string: "https://api.example.com/users/1")!
        let user = User(id: 1, name: "John Doe", email: "john@example.com")
        let updatedUserData = """
        {
            "id": 1,
            "name": "John Doe Updated",
            "email": "john.updated@example.com"
        }
        """.data(using: .utf8)!
        mockNetworkService.postDataResult = .success(updatedUserData)
        
        // When
        let expectation = XCTestExpectation(description: "Update user data")
        var result: User?
        var error: Error?
        
        dataManager.updateUserData(user, to: url)
            .sink(
                receiveCompletion: { completion in
                    if case .failure(let err) = completion {
                        error = err
                    }
                    expectation.fulfill()
                },
                receiveValue: { updatedUser in
                    result = updatedUser
                }
            )
            .store(in: &cancellables)
        
        // Then
        wait(for: [expectation], timeout: 1.0)
        XCTAssertNil(error, "Should not have error")
        XCTAssertNotNil(result, "Should have updated user data")
        XCTAssertEqual(result?.name, "John Doe Updated", "User name should be updated")
        XCTAssertEqual(mockNetworkService.postDataCallCount, 1, "Should call post data once")
        XCTAssertEqual(mockNetworkService.lastPostURL, url, "Should use correct URL")
    }
    
    // MARK: - Private Properties
    
    private var cancellables = Set<AnyCancellable>()
}

// MARK: - Test Coverage Example

/**
 * Test coverage demonstration
 * 
 * This class demonstrates proper test coverage
 * with comprehensive test cases
 */
class TestCoverageExample {
    
    // MARK: - Properties
    
    private var data: [String] = []
    
    // MARK: - Public Methods
    
    func addItem(_ item: String) {
        guard !item.isEmpty else { return }
        data.append(item)
    }
    
    func removeItem(at index: Int) throws {
        guard index >= 0 && index < data.count else {
            throw TestCoverageError.indexOutOfBounds
        }
        data.remove(at: index)
    }
    
    func getItem(at index: Int) throws -> String {
        guard index >= 0 && index < data.count else {
            throw TestCoverageError.indexOutOfBounds
        }
        return data[index]
    }
    
    func getCount() -> Int {
        return data.count
    }
    
    func clear() {
        data.removeAll()
    }
}

/**
 * Test coverage error types
 * 
 * This enum demonstrates proper error modeling
 * for test coverage
 */
enum TestCoverageError: Error, Equatable {
    case indexOutOfBounds
    case emptyData
}

/**
 * Test coverage unit tests
 * 
 * This class demonstrates comprehensive test coverage
 * with all code paths tested
 */
class TestCoverageExampleTests: XCTestCase {
    
    // MARK: - Properties
    
    var testCoverageExample: TestCoverageExample!
    
    // MARK: - Setup and Teardown
    
    override func setUp() {
        super.setUp()
        testCoverageExample = TestCoverageExample()
    }
    
    override func tearDown() {
        testCoverageExample = nil
        super.tearDown()
    }
    
    // MARK: - Add Item Tests
    
    func testAddItemSuccess() {
        // Given
        let item = "Test Item"
        
        // When
        testCoverageExample.addItem(item)
        
        // Then
        XCTAssertEqual(testCoverageExample.getCount(), 1, "Should have one item")
    }
    
    func testAddEmptyItem() {
        // Given
        let emptyItem = ""
        
        // When
        testCoverageExample.addItem(emptyItem)
        
        // Then
        XCTAssertEqual(testCoverageExample.getCount(), 0, "Should not add empty item")
    }
    
    func testAddMultipleItems() {
        // Given
        let items = ["Item 1", "Item 2", "Item 3"]
        
        // When
        for item in items {
            testCoverageExample.addItem(item)
        }
        
        // Then
        XCTAssertEqual(testCoverageExample.getCount(), 3, "Should have three items")
    }
    
    // MARK: - Remove Item Tests
    
    func testRemoveItemSuccess() throws {
        // Given
        testCoverageExample.addItem("Item 1")
        testCoverageExample.addItem("Item 2")
        
        // When
        try testCoverageExample.removeItem(at: 0)
        
        // Then
        XCTAssertEqual(testCoverageExample.getCount(), 1, "Should have one item after removal")
    }
    
    func testRemoveItemIndexOutOfBounds() {
        // Given
        testCoverageExample.addItem("Item 1")
        
        // When & Then
        XCTAssertThrowsError(try testCoverageExample.removeItem(at: 5)) { error in
            XCTAssertEqual(error as? TestCoverageError, .indexOutOfBounds)
        }
    }
    
    func testRemoveItemNegativeIndex() {
        // Given
        testCoverageExample.addItem("Item 1")
        
        // When & Then
        XCTAssertThrowsError(try testCoverageExample.removeItem(at: -1)) { error in
            XCTAssertEqual(error as? TestCoverageError, .indexOutOfBounds)
        }
    }
    
    // MARK: - Get Item Tests
    
    func testGetItemSuccess() throws {
        // Given
        testCoverageExample.addItem("Item 1")
        testCoverageExample.addItem("Item 2")
        
        // When
        let item = try testCoverageExample.getItem(at: 0)
        
        // Then
        XCTAssertEqual(item, "Item 1", "Should return correct item")
    }
    
    func testGetItemIndexOutOfBounds() {
        // Given
        testCoverageExample.addItem("Item 1")
        
        // When & Then
        XCTAssertThrowsError(try testCoverageExample.getItem(at: 5)) { error in
            XCTAssertEqual(error as? TestCoverageError, .indexOutOfBounds)
        }
    }
    
    // MARK: - Get Count Tests
    
    func testGetCountEmpty() {
        // Given
        // Empty data
        
        // When
        let count = testCoverageExample.getCount()
        
        // Then
        XCTAssertEqual(count, 0, "Should have zero items")
    }
    
    func testGetCountWithItems() {
        // Given
        testCoverageExample.addItem("Item 1")
        testCoverageExample.addItem("Item 2")
        
        // When
        let count = testCoverageExample.getCount()
        
        // Then
        XCTAssertEqual(count, 2, "Should have two items")
    }
    
    // MARK: - Clear Tests
    
    func testClear() {
        // Given
        testCoverageExample.addItem("Item 1")
        testCoverageExample.addItem("Item 2")
        
        // When
        testCoverageExample.clear()
        
        // Then
        XCTAssertEqual(testCoverageExample.getCount(), 0, "Should have zero items after clear")
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use unit testing strategies
 * 
 * This function shows practical usage of all the unit testing components
 */
func demonstrateUnitTesting() {
    print("=== Unit Testing Demonstration ===\n")
    
    // Test-Driven Development
    print("--- Test-Driven Development ---")
    print("Calculator: TDD implementation with comprehensive tests")
    print("Features: Red-green-refactor cycle, test-first development")
    
    // Mocking and Stubbing
    print("\n--- Mocking and Stubbing ---")
    print("MockNetworkService: Proper mocking implementation")
    print("DataManager: Dependency injection and testing")
    print("Features: Test doubles, dependency injection, isolation")
    
    // Test Coverage
    print("\n--- Test Coverage ---")
    print("TestCoverageExample: Comprehensive test coverage")
    print("Features: All code paths tested, edge cases covered")
    
    // Demonstrate testing techniques
    print("\n--- Testing Techniques ---")
    print("Unit Testing: Isolated component testing")
    print("Mocking: Test doubles and dependency injection")
    print("Test Coverage: Comprehensive test coverage")
    print("TDD: Test-driven development practices")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Write tests first (TDD)")
    print("2. Test behavior, not implementation")
    print("3. Use descriptive test names")
    print("4. Mock external dependencies")
    print("5. Test edge cases and error conditions")
    print("6. Maintain high test coverage")
    print("7. Keep tests simple and focused")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateUnitTesting()
