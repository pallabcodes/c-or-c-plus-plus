/*
 * Swift Fundamentals: Functions & Closures
 * 
 * This file demonstrates production-grade function and closure patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master function syntax and parameter patterns
 * - Understand closure capture semantics and memory management
 * - Implement higher-order functions and functional programming
 * - Apply advanced closure patterns for reactive programming
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Function Fundamentals

/**
 * Demonstrates various function patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic function syntax and parameters
 * - Function overloading and default parameters
 * - Inout parameters and reference passing
 * - Variadic parameters and function flexibility
 */
class FunctionFundamentals {
    
    // MARK: - Basic Function Patterns
    
    /**
     * Basic function with parameters and return type
     * 
     * - Parameters:
     *   - a: First integer
     *   - b: Second integer
     * - Returns: Sum of the two integers
     */
    func add(_ a: Int, _ b: Int) -> Int {
        return a + b
    }
    
    /**
     * Function with external parameter names
     * 
     * - Parameters:
     *   - firstName: User's first name
     *   - lastName: User's last name
     * - Returns: Full name string
     */
    func createFullName(firstName: String, lastName: String) -> String {
        return "\(firstName) \(lastName)"
    }
    
    /**
     * Function with default parameters
     * 
     * - Parameters:
     *   - message: Message to log
     *   - level: Log level (default: .info)
     *   - timestamp: Whether to include timestamp (default: true)
     * - Returns: Formatted log message
     */
    func logMessage(_ message: String, level: LogLevel = .info, timestamp: Bool = true) -> String {
        let timestampString = timestamp ? "[\(Date())] " : ""
        return "\(timestampString)[\(level.rawValue)] \(message)"
    }
    
    /**
     * Function with inout parameters for reference modification
     * 
     * - Parameter numbers: Array of numbers to sort in place
     */
    func sortNumbers(_ numbers: inout [Int]) {
        numbers.sort()
    }
    
    /**
     * Function with variadic parameters
     * 
     * - Parameter numbers: Variable number of integers
     * - Returns: Sum of all numbers
     */
    func sumNumbers(_ numbers: Int...) -> Int {
        return numbers.reduce(0, +)
    }
    
    /**
     * Function with multiple return values using tuple
     * 
     * - Parameter numbers: Array of numbers
     * - Returns: Tuple containing min, max, and average
     */
    func analyzeNumbers(_ numbers: [Int]) -> (min: Int, max: Int, average: Double) {
        guard !numbers.isEmpty else {
            return (min: 0, max: 0, average: 0.0)
        }
        
        let min = numbers.min() ?? 0
        let max = numbers.max() ?? 0
        let average = Double(numbers.reduce(0, +)) / Double(numbers.count)
        
        return (min: min, max: max, average: average)
    }
    
    /**
     * Function with optional return type
     * 
     * - Parameter numbers: Array of numbers to search
     * - Parameter target: Target number to find
     * - Returns: Index of target number, or nil if not found
     */
    func findIndex(_ numbers: [Int], target: Int) -> Int? {
        for (index, number) in numbers.enumerated() {
            if number == target {
                return index
            }
        }
        return nil
    }
    
    /**
     * Function with throwing error handling
     * 
     * - Parameter number: Number to validate
     * - Throws: ValidationError if number is invalid
     * - Returns: Validated number
     */
    func validateNumber(_ number: Int) throws -> Int {
        if number < 0 {
            throw ValidationError.negativeNumber
        }
        if number > 1000 {
            throw ValidationError.numberTooLarge
        }
        return number
    }
    
    /**
     * Function with async/await for asynchronous operations
     * 
     * - Parameter delay: Delay in seconds
     * - Returns: Current timestamp after delay
     */
    func delayedOperation(delay: TimeInterval) async -> Date {
        try? await Task.sleep(nanoseconds: UInt64(delay * 1_000_000_000))
        return Date()
    }
    
    // MARK: - Function Overloading
    
    /**
     * Overloaded function for different parameter types
     * 
     * - Parameter value: String value to process
     * - Returns: Processed string
     */
    func processValue(_ value: String) -> String {
        return value.uppercased()
    }
    
    /**
     * Overloaded function for different parameter types
     * 
     * - Parameter value: Integer value to process
     * - Returns: Processed integer as string
     */
    func processValue(_ value: Int) -> String {
        return "Number: \(value)"
    }
    
    /**
     * Overloaded function for different parameter types
     * 
     * - Parameter value: Double value to process
     * - Returns: Processed double as string
     */
    func processValue(_ value: Double) -> String {
        return String(format: "Double: %.2f", value)
    }
}

// MARK: - Supporting Types

/**
 * Log level enumeration
 */
enum LogLevel: String, CaseIterable {
    case debug = "DEBUG"
    case info = "INFO"
    case warning = "WARNING"
    case error = "ERROR"
    case critical = "CRITICAL"
}

/**
 * Validation error enumeration
 */
enum ValidationError: Error, LocalizedError {
    case negativeNumber
    case numberTooLarge
    case invalidFormat
    
    var errorDescription: String? {
        switch self {
        case .negativeNumber:
            return "Number cannot be negative"
        case .numberTooLarge:
            return "Number cannot be greater than 1000"
        case .invalidFormat:
            return "Invalid number format"
        }
    }
}

// MARK: - Closure Fundamentals

/**
 * Demonstrates various closure patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic closure syntax and capture semantics
 * - Escaping vs non-escaping closures
 * - Capture lists and memory management
 * - Higher-order functions and functional programming
 */
class ClosureFundamentals {
    
    // MARK: - Basic Closure Patterns
    
    /**
     * Demonstrates basic closure syntax
     * 
     * - Parameter numbers: Array of numbers to process
     * - Returns: Array of squared numbers
     */
    func squareNumbers(_ numbers: [Int]) -> [Int] {
        return numbers.map { number in
            return number * number
        }
    }
    
    /**
     * Demonstrates closure with shorthand syntax
     * 
     * - Parameter numbers: Array of numbers to filter
     * - Returns: Array of even numbers
     */
    func filterEvenNumbers(_ numbers: [Int]) -> [Int] {
        return numbers.filter { $0 % 2 == 0 }
    }
    
    /**
     * Demonstrates closure with reduce operation
     * 
     * - Parameter numbers: Array of numbers to sum
     * - Returns: Sum of all numbers
     */
    func sumNumbers(_ numbers: [Int]) -> Int {
        return numbers.reduce(0) { result, number in
            return result + number
        }
    }
    
    /**
     * Demonstrates closure with multiple parameters
     * 
     * - Parameter numbers: Array of numbers to process
     * - Returns: Array of processed numbers
     */
    func processNumbers(_ numbers: [Int]) -> [Int] {
        return numbers.map { number in
            if number % 2 == 0 {
                return number * 2
            } else {
                return number * 3
            }
        }
    }
    
    // MARK: - Escaping vs Non-Escaping Closures
    
    /**
     * Demonstrates non-escaping closure (default)
     * 
     * - Parameter numbers: Array of numbers to process
     * - Parameter transform: Transformation closure
     * - Returns: Array of transformed numbers
     */
    func transformNumbers(_ numbers: [Int], transform: (Int) -> Int) -> [Int] {
        return numbers.map(transform)
    }
    
    /**
     * Demonstrates escaping closure for asynchronous operations
     * 
     * - Parameter delay: Delay in seconds
     * - Parameter completion: Completion closure to call after delay
     */
    func delayedOperation(delay: TimeInterval, completion: @escaping (Date) -> Void) {
        DispatchQueue.global(qos: .userInitiated).asyncAfter(deadline: .now() + delay) {
            let result = Date()
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    /**
     * Demonstrates escaping closure with error handling
     * 
     * - Parameter url: URL to fetch data from
     * - Parameter completion: Completion closure with result or error
     */
    func fetchData(from url: URL, completion: @escaping (Result<Data, Error>) -> Void) {
        URLSession.shared.dataTask(with: url) { data, response, error in
            if let error = error {
                completion(.failure(error))
                return
            }
            
            guard let data = data else {
                completion(.failure(NetworkError.noData))
                return
            }
            
            completion(.success(data))
        }.resume()
    }
    
    // MARK: - Capture Lists and Memory Management
    
    /**
     * Demonstrates weak capture to avoid retain cycles
     * 
     * - Parameter completion: Completion closure
     */
    func performOperation(completion: @escaping (String) -> Void) {
        // Simulate async operation
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            guard let self = self else { return }
            
            let result = self.processData()
            
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    /**
     * Demonstrates unowned capture for guaranteed lifetime
     * 
     * - Parameter completion: Completion closure
     */
    func performGuaranteedOperation(completion: @escaping (String) -> Void) {
        // Simulate async operation
        DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
            let result = self.processData()
            
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    /**
     * Helper method for data processing
     * 
     * - Returns: Processed data string
     */
    private func processData() -> String {
        return "Processed data at \(Date())"
    }
    
    // MARK: - Higher-Order Functions
    
    /**
     * Demonstrates custom higher-order function
     * 
     * - Parameters:
     *   - items: Array of items to process
     *   - predicate: Predicate closure to filter items
     *   - transform: Transformation closure to modify items
     * - Returns: Array of filtered and transformed items
     */
    func filterAndTransform<T, U>(
        _ items: [T],
        predicate: (T) -> Bool,
        transform: (T) -> U
    ) -> [U] {
        return items
            .filter(predicate)
            .map(transform)
    }
    
    /**
     * Demonstrates function composition
     * 
     * - Parameter numbers: Array of numbers to process
     * - Returns: Array of processed numbers
     */
    func composeOperations(_ numbers: [Int]) -> [Int] {
        let square: (Int) -> Int = { $0 * $0 }
        let double: (Int) -> Int = { $0 * 2 }
        let filterEven: (Int) -> Bool = { $0 % 2 == 0 }
        
        return numbers
            .filter(filterEven)
            .map(square)
            .map(double)
    }
    
    /**
     * Demonstrates currying for partial function application
     * 
     * - Parameter multiplier: Multiplier value
     * - Returns: Function that multiplies by the given value
     */
    func createMultiplier(_ multiplier: Int) -> (Int) -> Int {
        return { number in
            return number * multiplier
        }
    }
    
    /**
     * Demonstrates function that returns a closure
     * 
     * - Parameter threshold: Threshold value for comparison
     * - Returns: Closure that checks if value is above threshold
     */
    func createThresholdChecker(_ threshold: Int) -> (Int) -> Bool {
        return { value in
            return value > threshold
        }
    }
}

// MARK: - Supporting Types

/**
 * Network error enumeration
 */
enum NetworkError: Error, LocalizedError {
    case noData
    case invalidURL
    case networkUnavailable
    
    var errorDescription: String? {
        switch self {
        case .noData:
            return "No data received"
        case .invalidURL:
            return "Invalid URL"
        case .networkUnavailable:
            return "Network unavailable"
        }
    }
}

// MARK: - Functional Programming Patterns

/**
 * Demonstrates advanced functional programming patterns
 * 
 * This class covers:
 * - Immutable data structures
 * - Pure functions and side effects
 * - Function composition and chaining
 * - Monadic operations and error handling
 */
class FunctionalProgramming {
    
    // MARK: - Immutable Data Structures
    
    /**
     * Immutable user data structure
     */
    struct User {
        let id: UUID
        let name: String
        let email: String
        let age: Int
        
        init(id: UUID = UUID(), name: String, email: String, age: Int) {
            self.id = id
            self.name = name
            self.email = email
            self.age = age
        }
        
        /**
         * Creates a new user with updated name
         * 
         * - Parameter newName: New name for the user
         * - Returns: New user instance with updated name
         */
        func updatingName(_ newName: String) -> User {
            return User(id: id, name: newName, email: email, age: age)
        }
        
        /**
         * Creates a new user with updated email
         * 
         * - Parameter newEmail: New email for the user
         * - Returns: New user instance with updated email
         */
        func updatingEmail(_ newEmail: String) -> User {
            return User(id: id, name: name, email: newEmail, age: age)
        }
    }
    
    // MARK: - Pure Functions
    
    /**
     * Pure function that calculates age category
     * 
     * - Parameter age: User's age
     * - Returns: Age category string
     */
    func getAgeCategory(_ age: Int) -> String {
        switch age {
        case 0...12:
            return "Child"
        case 13...19:
            return "Teenager"
        case 20...59:
            return "Adult"
        case 60...:
            return "Senior"
        default:
            return "Unknown"
        }
    }
    
    /**
     * Pure function that validates email format
     * 
     * - Parameter email: Email string to validate
     * - Returns: True if email is valid
     */
    func isValidEmail(_ email: String) -> Bool {
        let emailRegex = "^[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"
        let emailPredicate = NSPredicate(format: "SELF MATCHES %@", emailRegex)
        return emailPredicate.evaluate(with: email)
    }
    
    /**
     * Pure function that formats user display name
     * 
     * - Parameter user: User to format
     * - Returns: Formatted display name
     */
    func formatDisplayName(for user: User) -> String {
        let ageCategory = getAgeCategory(user.age)
        return "\(user.name) (\(ageCategory))"
    }
    
    // MARK: - Function Composition
    
    /**
     * Demonstrates function composition with chaining
     * 
     * - Parameter users: Array of users to process
     * - Returns: Array of formatted display names
     */
    func processUsers(_ users: [User]) -> [String] {
        return users
            .filter { isValidEmail($0.email) }
            .map { formatDisplayName(for: $0) }
            .sorted()
    }
    
    /**
     * Demonstrates function composition with custom operators
     * 
     * - Parameter users: Array of users to process
     * - Returns: Array of processed user names
     */
    func processUsersWithComposition(_ users: [User]) -> [String] {
        let filterValid = { (users: [User]) in users.filter { isValidEmail($0.email) } }
        let formatNames = { (users: [User]) in users.map { formatDisplayName(for: $0) } }
        let sortNames = { (names: [String]) in names.sorted() }
        
        return (filterValid >>> formatNames >>> sortNames)(users)
    }
    
    // MARK: - Monadic Operations
    
    /**
     * Demonstrates monadic operations with Result type
     * 
     * - Parameter user: User to validate
     * - Returns: Result with validated user or error
     */
    func validateUser(_ user: User) -> Result<User, ValidationError> {
        guard !user.name.isEmpty else {
            return .failure(.invalidFormat)
        }
        
        guard isValidEmail(user.email) else {
            return .failure(.invalidFormat)
        }
        
        guard user.age >= 0 else {
            return .failure(.negativeNumber)
        }
        
        return .success(user)
    }
    
    /**
     * Demonstrates chaining of monadic operations
     * 
     * - Parameter users: Array of users to validate
     * - Returns: Array of valid users
     */
    func validateUsers(_ users: [User]) -> [User] {
        return users
            .compactMap { user in
                switch validateUser(user) {
                case .success(let validUser):
                    return validUser
                case .failure:
                    return nil
                }
            }
    }
}

// MARK: - Custom Operators

/**
 * Custom operator for function composition
 * 
 * - Parameters:
 *   - f: First function
 *   - g: Second function
 * - Returns: Composed function
 */
infix operator >>>: CompositionPrecedence

precedencegroup CompositionPrecedence {
    associativity: left
    higherThan: AssignmentPrecedence
}

func >>> <A, B, C>(f: @escaping (A) -> B, g: @escaping (B) -> C) -> (A) -> C {
    return { x in g(f(x)) }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the function and closure patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateFunctionsAndClosures() {
    print("=== Swift Functions & Closures Demonstration ===\n")
    
    // Function Fundamentals
    let functionExample = FunctionFundamentals()
    
    print("Add function: \(functionExample.add(5, 3))")
    print("Full name: \(functionExample.createFullName(firstName: "John", lastName: "Doe"))")
    print("Log message: \(functionExample.logMessage("Test message", level: .info))")
    
    var numbers = [3, 1, 4, 1, 5]
    functionExample.sortNumbers(&numbers)
    print("Sorted numbers: \(numbers)")
    
    print("Sum of numbers: \(functionExample.sumNumbers(1, 2, 3, 4, 5))")
    
    let analysis = functionExample.analyzeNumbers([1, 2, 3, 4, 5])
    print("Analysis: min=\(analysis.min), max=\(analysis.max), avg=\(analysis.average)")
    
    print("Find index: \(functionExample.findIndex([1, 2, 3, 4, 5], target: 3) ?? -1)")
    
    do {
        let validated = try functionExample.validateNumber(42)
        print("Validated number: \(validated)")
    } catch {
        print("Validation error: \(error.localizedDescription)")
    }
    
    // Closure Fundamentals
    let closureExample = ClosureFundamentals()
    
    print("Squared numbers: \(closureExample.squareNumbers([1, 2, 3, 4, 5]))")
    print("Even numbers: \(closureExample.filterEvenNumbers([1, 2, 3, 4, 5]))")
    print("Sum of numbers: \(closureExample.sumNumbers([1, 2, 3, 4, 5]))")
    print("Processed numbers: \(closureExample.processNumbers([1, 2, 3, 4, 5]))")
    
    // Escaping closures
    closureExample.delayedOperation(delay: 1.0) { result in
        print("Delayed operation result: \(result)")
    }
    
    // Higher-order functions
    let items = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    let filteredAndTransformed = closureExample.filterAndTransform(
        items,
        predicate: { $0 % 2 == 0 },
        transform: { $0 * $0 }
    )
    print("Filtered and transformed: \(filteredAndTransformed)")
    
    print("Composed operations: \(closureExample.composeOperations([1, 2, 3, 4, 5]))")
    
    let multiplier = closureExample.createMultiplier(5)
    print("Multiplier result: \(multiplier(10))")
    
    let thresholdChecker = closureExample.createThresholdChecker(5)
    print("Threshold check: \(thresholdChecker(7))")
    
    // Functional Programming
    let functionalExample = FunctionalProgramming()
    
    let users = [
        FunctionalProgramming.User(name: "Alice", email: "alice@example.com", age: 25),
        FunctionalProgramming.User(name: "Bob", email: "bob@example.com", age: 30),
        FunctionalProgramming.User(name: "Charlie", email: "invalid-email", age: 35)
    ]
    
    print("Processed users: \(functionalExample.processUsers(users))")
    print("Processed users with composition: \(functionalExample.processUsersWithComposition(users))")
    
    let validUsers = functionalExample.validateUsers(users)
    print("Valid users: \(validUsers.count)")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateFunctionsAndClosures()
