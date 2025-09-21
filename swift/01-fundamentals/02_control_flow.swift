/*
 * Swift Fundamentals: Control Flow
 * 
 * This file demonstrates production-grade control flow patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master conditional statements and guard clauses
 * - Understand loop patterns and performance implications
 * - Implement advanced pattern matching with switch statements
 * - Apply control transfer statements effectively
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Conditional Statements

/**
 * Demonstrates various conditional statement patterns used in production iOS apps
 * 
 * This class covers:
 * - if/else statements with proper error handling
 * - guard statements for early returns
 * - ternary operators for concise conditionals
 * - nil coalescing for safe unwrapping
 */
class ConditionalStatements {
    
    // MARK: - User Authentication Example
    
    /**
     * Represents a user authentication result
     * 
     * This enum demonstrates pattern matching with associated values
     */
    enum AuthenticationResult {
        case success(user: User)
        case failure(error: AuthenticationError)
        case requiresTwoFactor(token: String)
    }
    
    /**
     * Represents authentication errors
     */
    enum AuthenticationError: Error, LocalizedError {
        case invalidCredentials
        case accountLocked
        case networkError
        case serverError(String)
        
        var errorDescription: String? {
            switch self {
            case .invalidCredentials:
                return "Invalid username or password"
            case .accountLocked:
                return "Account is locked due to multiple failed attempts"
            case .networkError:
                return "Network connection failed"
            case .serverError(let message):
                return "Server error: \(message)"
            }
        }
    }
    
    /**
     * Represents a user in the system
     */
    struct User {
        let id: UUID
        let username: String
        let email: String
        let isVerified: Bool
        let lastLogin: Date?
        
        init(id: UUID = UUID(), username: String, email: String, isVerified: Bool = false, lastLogin: Date? = nil) {
            self.id = id
            self.username = username
            self.email = email
            self.isVerified = isVerified
            self.lastLogin = lastLogin
        }
    }
    
    /**
     * Demonstrates if/else statements with proper error handling
     * 
     * - Parameters:
     *   - username: The username to authenticate
     *   - password: The password to verify
     *   - completion: Completion handler with authentication result
     */
    func authenticateUser(username: String, password: String, completion: @escaping (AuthenticationResult) -> Void) {
        // Input validation using guard statements
        guard !username.isEmpty else {
            completion(.failure(error: .invalidCredentials))
            return
        }
        
        guard !password.isEmpty else {
            completion(.failure(error: .invalidCredentials))
            return
        }
        
        guard username.count >= 3 else {
            completion(.failure(error: .invalidCredentials))
            return
        }
        
        guard password.count >= 8 else {
            completion(.failure(error: .invalidCredentials))
            return
        }
        
        // Simulate network request
        DispatchQueue.global(qos: .userInitiated).async {
            // Simulate network delay
            Thread.sleep(forTimeInterval: 1.0)
            
            // Simulate authentication logic
            if username == "admin" && password == "password123" {
                let user = User(username: username, email: "\(username)@example.com", isVerified: true)
                DispatchQueue.main.async {
                    completion(.success(user: user))
                }
            } else if username == "2fa_user" && password == "password123" {
                DispatchQueue.main.async {
                    completion(.requiresTwoFactor(token: "2fa_token_123"))
                }
            } else {
                DispatchQueue.main.async {
                    completion(.failure(error: .invalidCredentials))
                }
            }
        }
    }
    
    /**
     * Demonstrates guard statements for early returns and validation
     * 
     * - Parameter user: The user to validate
     * - Returns: True if user is valid, false otherwise
     */
    func validateUser(_ user: User) -> Bool {
        // Guard statements for early returns
        guard !user.username.isEmpty else {
            print("Username cannot be empty")
            return false
        }
        
        guard user.email.contains("@") else {
            print("Email must contain @ symbol")
            return false
        }
        
        guard user.username.count >= 3 else {
            print("Username must be at least 3 characters")
            return false
        }
        
        // All validations passed
        return true
    }
    
    /**
     * Demonstrates ternary operators for concise conditionals
     * 
     * - Parameter user: The user to get display name for
     * - Returns: Formatted display name
     */
    func getDisplayName(for user: User) -> String {
        // Ternary operator for concise conditionals
        let displayName = user.isVerified ? "✓ \(user.username)" : user.username
        let lastLoginText = user.lastLogin != nil ? " (Last login: \(formatDate(user.lastLogin!)))" : " (Never logged in)"
        
        return displayName + lastLoginText
    }
    
    /**
     * Demonstrates nil coalescing for safe unwrapping
     * 
     * - Parameter user: The user to get last login for
     * - Returns: Formatted last login date or default message
     */
    func getLastLoginText(for user: User) -> String {
        // Nil coalescing operator
        let lastLogin = user.lastLogin ?? Date.distantPast
        let timeSinceLogin = Date().timeIntervalSince(lastLogin)
        
        // Ternary operator with nil coalescing
        return timeSinceLogin > 0 ? "Last login: \(formatDate(lastLogin))" : "Never logged in"
    }
    
    /**
     * Helper function to format dates
     * 
     * - Parameter date: The date to format
     * - Returns: Formatted date string
     */
    private func formatDate(_ date: Date) -> String {
        let formatter = DateFormatter()
        formatter.dateStyle = .medium
        formatter.timeStyle = .short
        return formatter.string(from: date)
    }
}

// MARK: - Loop Patterns

/**
 * Demonstrates various loop patterns used in production iOS apps
 * 
 * This class covers:
 * - for-in loops with collections
 * - while loops for conditional iteration
 * - repeat-while loops for guaranteed execution
 * - Performance considerations and optimizations
 */
class LoopPatterns {
    
    /**
     * Demonstrates for-in loops with different collection types
     * 
     * - Parameter numbers: Array of numbers to process
     * - Returns: Array of processed numbers
     */
    func processNumbers(_ numbers: [Int]) -> [Int] {
        var processedNumbers: [Int] = []
        
        // Basic for-in loop
        for number in numbers {
            let processed = number * 2
            processedNumbers.append(processed)
        }
        
        return processedNumbers
    }
    
    /**
     * Demonstrates for-in loops with enumerated indices
     * 
     * - Parameter items: Array of items to process
     * - Returns: Array of processed items with indices
     */
    func processItemsWithIndices<T>(_ items: [T]) -> [(index: Int, item: T)] {
        var processedItems: [(index: Int, item: T)] = []
        
        // Enumerated for-in loop
        for (index, item) in items.enumerated() {
            processedItems.append((index: index, item: item))
        }
        
        return processedItems
    }
    
    /**
     * Demonstrates for-in loops with ranges
     * 
     * - Parameter count: Number of iterations
     * - Returns: Array of generated numbers
     */
    func generateNumbers(count: Int) -> [Int] {
        var numbers: [Int] = []
        
        // Range-based for-in loop
        for i in 0..<count {
            numbers.append(i * i) // Generate squares
        }
        
        return numbers
    }
    
    /**
     * Demonstrates while loops for conditional iteration
     * 
     * - Parameter target: Target value to reach
     * - Returns: Number of iterations required
     */
    func countToTarget(_ target: Int) -> Int {
        var current = 0
        var iterations = 0
        
        // While loop with condition
        while current < target {
            current += 1
            iterations += 1
            
            // Safety check to prevent infinite loops
            if iterations > target * 2 {
                print("Warning: Potential infinite loop detected")
                break
            }
        }
        
        return iterations
    }
    
    /**
     * Demonstrates repeat-while loops for guaranteed execution
     * 
     * - Parameter maxAttempts: Maximum number of attempts
     * - Returns: Success status
     */
    func retryOperation(maxAttempts: Int) -> Bool {
        var attempts = 0
        var success = false
        
        // Repeat-while loop (guaranteed to execute at least once)
        repeat {
            attempts += 1
            print("Attempt \(attempts) of \(maxAttempts)")
            
            // Simulate operation that might fail
            success = simulateOperation()
            
            if !success && attempts < maxAttempts {
                print("Operation failed, retrying...")
                Thread.sleep(forTimeInterval: 1.0) // Simulate delay
            }
        } while !success && attempts < maxAttempts
        
        return success
    }
    
    /**
     * Simulates an operation that might fail
     * 
     * - Returns: True if operation succeeds, false otherwise
     */
    private func simulateOperation() -> Bool {
        // Simulate 70% success rate
        return Double.random(in: 0...1) < 0.7
    }
    
    /**
     * Demonstrates performance-optimized loops
     * 
     * - Parameter largeArray: Large array to process
     * - Returns: Processed array
     */
    func processLargeArray(_ largeArray: [Int]) -> [Int] {
        // Use map for functional approach (more efficient)
        return largeArray.map { $0 * 2 }
    }
    
    /**
     * Demonstrates loops with early termination
     * 
     * - Parameter numbers: Array of numbers to search
     * - Parameter target: Target number to find
     * - Returns: Index of target number, or nil if not found
     */
    func findTarget(_ numbers: [Int], target: Int) -> Int? {
        // Use enumerated for-in with early termination
        for (index, number) in numbers.enumerated() {
            if number == target {
                return index
            }
        }
        
        return nil
    }
}

// MARK: - Pattern Matching with Switch Statements

/**
 * Demonstrates advanced pattern matching with switch statements
 * 
 * This class covers:
 * - Basic switch statements
 * - Pattern matching with associated values
 * - Where clauses for additional conditions
 * - Fallthrough and control transfer
 */
class PatternMatching {
    
    /**
     * Represents different types of network responses
     */
    enum NetworkResponse {
        case success(data: Data)
        case error(code: Int, message: String)
        case timeout
        case noConnection
    }
    
    /**
     * Represents different types of user actions
     */
    enum UserAction {
        case login(username: String, password: String)
        case logout
        case updateProfile(name: String, email: String)
        case deleteAccount(reason: String)
        case viewProfile(userId: UUID)
    }
    
    /**
     * Demonstrates basic switch statements
     * 
     * - Parameter response: Network response to handle
     * - Returns: Human-readable response description
     */
    func handleNetworkResponse(_ response: NetworkResponse) -> String {
        switch response {
        case .success(let data):
            return "Success: Received \(data.count) bytes"
        case .error(let code, let message):
            return "Error \(code): \(message)"
        case .timeout:
            return "Request timed out"
        case .noConnection:
            return "No internet connection"
        }
    }
    
    /**
     * Demonstrates pattern matching with associated values
     * 
     * - Parameter action: User action to process
     * - Returns: Processing result
     */
    func processUserAction(_ action: UserAction) -> String {
        switch action {
        case .login(let username, let password):
            return "Processing login for user: \(username)"
        case .logout:
            return "Processing logout"
        case .updateProfile(let name, let email):
            return "Updating profile: \(name) (\(email))"
        case .deleteAccount(let reason):
            return "Deleting account. Reason: \(reason)"
        case .viewProfile(let userId):
            return "Viewing profile for user: \(userId)"
        }
    }
    
    /**
     * Demonstrates switch statements with where clauses
     * 
     * - Parameter number: Number to categorize
     * - Returns: Category description
     */
    func categorizeNumber(_ number: Int) -> String {
        switch number {
        case let n where n < 0:
            return "Negative number: \(n)"
        case 0:
            return "Zero"
        case 1...10:
            return "Small positive number: \(number)"
        case let n where n > 10 && n < 100:
            return "Medium positive number: \(n)"
        case let n where n >= 100:
            return "Large positive number: \(n)"
        default:
            return "Unknown number: \(number)"
        }
    }
    
    /**
     * Demonstrates switch statements with tuples
     * 
     * - Parameter coordinates: Tuple of x, y coordinates
     * - Returns: Quadrant description
     */
    func getQuadrant(_ coordinates: (x: Int, y: Int)) -> String {
        switch coordinates {
        case (let x, let y) where x > 0 && y > 0:
            return "First quadrant: (\(x), \(y))"
        case (let x, let y) where x < 0 && y > 0:
            return "Second quadrant: (\(x), \(y))"
        case (let x, let y) where x < 0 && y < 0:
            return "Third quadrant: (\(x), \(y))"
        case (let x, let y) where x > 0 && y < 0:
            return "Fourth quadrant: (\(x), \(y))"
        case (0, 0):
            return "Origin: (0, 0)"
        case (let x, 0):
            return "On x-axis: (\(x), 0)"
        case (0, let y):
            return "On y-axis: (0, \(y))"
        default:
            return "Unknown coordinates: (\(coordinates.x), \(coordinates.y))"
        }
    }
    
    /**
     * Demonstrates switch statements with fallthrough
     * 
     * - Parameter grade: Letter grade
     * - Returns: Grade description
     */
    func getGradeDescription(_ grade: Character) -> String {
        var description = ""
        
        switch grade {
        case "A":
            description += "Excellent! "
            fallthrough
        case "B":
            description += "Good! "
            fallthrough
        case "C":
            description += "Average. "
            fallthrough
        case "D":
            description += "Below average. "
            fallthrough
        case "F":
            description += "Failed. "
        default:
            description = "Invalid grade."
        }
        
        return description
    }
}

// MARK: - Control Transfer Statements

/**
 * Demonstrates control transfer statements in Swift
 * 
 * This class covers:
 * - break statements for loop termination
 * - continue statements for loop iteration
 * - return statements for function exit
 * - throw statements for error propagation
 */
class ControlTransfer {
    
    /**
     * Demonstrates break statements in loops
     * 
     * - Parameter numbers: Array of numbers to search
     * - Parameter target: Target number to find
     * - Returns: Index of first occurrence, or nil if not found
     */
    func findFirstOccurrence(_ numbers: [Int], target: Int) -> Int? {
        for (index, number) in numbers.enumerated() {
            if number == target {
                return index // Early return
            }
        }
        return nil
    }
    
    /**
     * Demonstrates continue statements in loops
     * 
     * - Parameter numbers: Array of numbers to filter
     * - Returns: Array of even numbers only
     */
    func filterEvenNumbers(_ numbers: [Int]) -> [Int] {
        var evenNumbers: [Int] = []
        
        for number in numbers {
            if number % 2 != 0 {
                continue // Skip odd numbers
            }
            evenNumbers.append(number)
        }
        
        return evenNumbers
    }
    
    /**
     * Demonstrates return statements with early exits
     * 
     * - Parameter user: User to validate
     * - Returns: Validation result
     */
    func validateUser(_ user: ConditionalStatements.User) -> Bool {
        // Early return for invalid username
        if user.username.isEmpty {
            return false
        }
        
        // Early return for invalid email
        if !user.email.contains("@") {
            return false
        }
        
        // All validations passed
        return true
    }
    
    /**
     * Demonstrates throw statements for error propagation
     * 
     * - Parameter number: Number to validate
     * - Throws: ValidationError if number is invalid
     */
    func validateNumber(_ number: Int) throws {
        if number < 0 {
            throw ValidationError.negativeNumber
        }
        
        if number > 1000 {
            throw ValidationError.numberTooLarge
        }
        
        if number == 0 {
            throw ValidationError.zeroNotAllowed
        }
    }
    
    /**
     * Custom validation error enum
     */
    enum ValidationError: Error, LocalizedError {
        case negativeNumber
        case numberTooLarge
        case zeroNotAllowed
        
        var errorDescription: String? {
            switch self {
            case .negativeNumber:
                return "Number cannot be negative"
            case .numberTooLarge:
                return "Number cannot be greater than 1000"
            case .zeroNotAllowed:
                return "Zero is not allowed"
            }
        }
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the control flow patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateControlFlow() {
    print("=== Swift Control Flow Demonstration ===\n")
    
    // Conditional Statements
    let conditionalExample = ConditionalStatements()
    let user = ConditionalStatements.User(username: "john_doe", email: "john@example.com", isVerified: true)
    
    print("User validation: \(conditionalExample.validateUser(user))")
    print("Display name: \(conditionalExample.getDisplayName(for: user))")
    print("Last login: \(conditionalExample.getLastLoginText(for: user))\n")
    
    // Loop Patterns
    let loopExample = LoopPatterns()
    let numbers = [1, 2, 3, 4, 5]
    
    print("Processed numbers: \(loopExample.processNumbers(numbers))")
    print("Items with indices: \(loopExample.processItemsWithIndices(numbers))")
    print("Generated numbers: \(loopExample.generateNumbers(count: 5))")
    print("Count to target: \(loopExample.countToTarget(10))")
    print("Retry operation success: \(loopExample.retryOperation(maxAttempts: 3))\n")
    
    // Pattern Matching
    let patternExample = PatternMatching()
    let networkResponse = PatternMatching.NetworkResponse.success(data: Data("Hello".utf8))
    let userAction = PatternMatching.UserAction.login(username: "alice", password: "secret")
    
    print("Network response: \(patternExample.handleNetworkResponse(networkResponse))")
    print("User action: \(patternExample.processUserAction(userAction))")
    print("Number category: \(patternExample.categorizeNumber(42))")
    print("Quadrant: \(patternExample.getQuadrant((x: 3, y: -4)))")
    print("Grade description: \(patternExample.getGradeDescription("B"))\n")
    
    // Control Transfer
    let controlExample = ControlTransfer()
    let searchNumbers = [1, 3, 5, 7, 9, 2, 4, 6, 8]
    
    print("First occurrence of 5: \(controlExample.findFirstOccurrence(searchNumbers, target: 5) ?? -1)")
    print("Even numbers: \(controlExample.filterEvenNumbers(searchNumbers))")
    print("User validation: \(controlExample.validateUser(user))")
    
    // Error handling example
    do {
        try controlExample.validateNumber(42)
        print("Number validation: Success")
    } catch {
        print("Number validation error: \(error.localizedDescription)")
    }
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateControlFlow()
