/*
 * Swift Fundamentals: Optionals
 * 
 * This file demonstrates production-grade optional handling patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master optional types and nil handling
 * - Understand optional chaining and nil coalescing
 * - Implement safe unwrapping patterns
 * - Apply advanced optional techniques for robust code
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Optional Fundamentals

/**
 * Demonstrates fundamental optional patterns used in production iOS apps
 * 
 * This class covers:
 * - Optional type declaration and initialization
 * - Optional binding and unwrapping
 * - Optional chaining and nil coalescing
 * - Guard statements for early returns
 */
class OptionalFundamentals {
    
    // MARK: - Optional Type Declaration
    
    /**
     * Demonstrates various ways to declare optional types
     * 
     * This method shows different optional declaration patterns
     */
    func demonstrateOptionalDeclaration() {
        // Explicit optional type
        var explicitOptional: String? = "Hello"
        print("Explicit optional: \(explicitOptional ?? "nil")")
        
        // Implicit optional type
        var implicitOptional = Optional("World")
        print("Implicit optional: \(implicitOptional ?? "nil")")
        
        // Nil initialization
        var nilOptional: String? = nil
        print("Nil optional: \(nilOptional ?? "nil")")
        
        // Optional with default value
        var defaultOptional: String? = "Default"
        print("Default optional: \(defaultOptional ?? "nil")")
    }
    
    // MARK: - Optional Binding
    
    /**
     * Demonstrates optional binding patterns
     * 
     * - Parameter optionalValue: Optional value to unwrap
     * - Returns: Unwrapped value or default
     */
    func demonstrateOptionalBinding(_ optionalValue: String?) -> String {
        // if-let binding
        if let unwrappedValue = optionalValue {
            return "Unwrapped value: \(unwrappedValue)"
        } else {
            return "Value is nil"
        }
    }
    
    /**
     * Demonstrates guard-let binding for early returns
     * 
     * - Parameter optionalValue: Optional value to unwrap
     * - Returns: Unwrapped value or throws error
     * - Throws: OptionalError if value is nil
     */
    func demonstrateGuardLetBinding(_ optionalValue: String?) throws -> String {
        guard let unwrappedValue = optionalValue else {
            throw OptionalError.nilValue
        }
        
        return "Unwrapped value: \(unwrappedValue)"
    }
    
    /**
     * Demonstrates multiple optional binding
     * 
     * - Parameters:
     *   - first: First optional value
     *   - second: Second optional value
     * - Returns: Combined string or nil
     */
    func demonstrateMultipleBinding(_ first: String?, _ second: String?) -> String? {
        guard let firstValue = first, let secondValue = second else {
            return nil
        }
        
        return "\(firstValue) \(secondValue)"
    }
    
    // MARK: - Optional Chaining
    
    /**
     * Demonstrates optional chaining with method calls
     * 
     * - Parameter optionalString: Optional string to process
     * - Returns: Processed string or nil
     */
    func demonstrateOptionalChaining(_ optionalString: String?) -> String? {
        return optionalString?.uppercased()
    }
    
    /**
     * Demonstrates optional chaining with property access
     * 
     * - Parameter optionalString: Optional string to process
     * - Returns: String length or nil
     */
    func demonstrateOptionalChainingWithProperties(_ optionalString: String?) -> Int? {
        return optionalString?.count
    }
    
    /**
     * Demonstrates optional chaining with subscript access
     * 
     * - Parameter optionalArray: Optional array to access
     * - Parameter index: Index to access
     * - Returns: Element at index or nil
     */
    func demonstrateOptionalChainingWithSubscript(_ optionalArray: [String]?, at index: Int) -> String? {
        return optionalArray?[index]
    }
    
    // MARK: - Nil Coalescing
    
    /**
     * Demonstrates nil coalescing operator
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Unwrapped value or default
     */
    func demonstrateNilCoalescing(_ optionalValue: String?) -> String {
        return optionalValue ?? "Default Value"
    }
    
    /**
     * Demonstrates nil coalescing with function calls
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or default
     */
    func demonstrateNilCoalescingWithFunction(_ optionalValue: String?) -> String {
        return optionalValue?.uppercased() ?? "DEFAULT"
    }
    
    /**
     * Demonstrates nil coalescing with complex expressions
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or default
     */
    func demonstrateNilCoalescingComplex(_ optionalValue: String?) -> String {
        return optionalValue?.trimmingCharacters(in: .whitespacesAndNewlines) ?? "No value provided"
    }
}

// MARK: - Supporting Types

/**
 * Custom error type for optional handling
 */
enum OptionalError: Error, LocalizedError {
    case nilValue
    case invalidValue
    case conversionFailed
    
    var errorDescription: String? {
        switch self {
        case .nilValue:
            return "Value is nil"
        case .invalidValue:
            return "Value is invalid"
        case .conversionFailed:
            return "Value conversion failed"
        }
    }
}

// MARK: - Advanced Optional Patterns

/**
 * Demonstrates advanced optional patterns for production apps
 * 
 * This class covers:
 * - Optional mapping and flatMap
 * - Optional filtering and validation
 * - Custom optional operators
 * - Error handling with optionals
 */
class AdvancedOptionalPatterns {
    
    // MARK: - Optional Mapping
    
    /**
     * Demonstrates optional mapping
     * 
     * - Parameter optionalValue: Optional value to map
     * - Returns: Mapped optional value
     */
    func demonstrateOptionalMapping(_ optionalValue: String?) -> String? {
        return optionalValue.map { $0.uppercased() }
    }
    
    /**
     * Demonstrates optional flatMap for chaining operations
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed optional value
     */
    func demonstrateOptionalFlatMap(_ optionalValue: String?) -> String? {
        return optionalValue.flatMap { value in
            guard !value.isEmpty else { return nil }
            return value.uppercased()
        }
    }
    
    /**
     * Demonstrates optional mapping with error handling
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Result with processed value or error
     */
    func demonstrateOptionalMappingWithError(_ optionalValue: String?) -> Result<String, OptionalError> {
        guard let value = optionalValue else {
            return .failure(.nilValue)
        }
        
        guard !value.isEmpty else {
            return .failure(.invalidValue)
        }
        
        return .success(value.uppercased())
    }
    
    // MARK: - Optional Filtering
    
    /**
     * Demonstrates optional filtering
     * 
     * - Parameter optionalValue: Optional value to filter
     * - Returns: Filtered optional value
     */
    func demonstrateOptionalFiltering(_ optionalValue: String?) -> String? {
        return optionalValue.filter { !$0.isEmpty }
    }
    
    /**
     * Demonstrates optional filtering with custom predicate
     * 
     * - Parameter optionalValue: Optional value to filter
     * - Parameter predicate: Predicate to apply
     * - Returns: Filtered optional value
     */
    func demonstrateOptionalFilteringWithPredicate(_ optionalValue: String?, predicate: (String) -> Bool) -> String? {
        return optionalValue.filter(predicate)
    }
    
    // MARK: - Custom Optional Operators
    
    /**
     * Demonstrates custom optional operators
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or default
     */
    func demonstrateCustomOperators(_ optionalValue: String?) -> String {
        // Using custom ??= operator
        var result = optionalValue
        result ??= "Default Value"
        return result
    }
    
    /**
     * Demonstrates custom optional chaining operator
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or nil
     */
    func demonstrateCustomChainingOperator(_ optionalValue: String?) -> String? {
        return optionalValue?.trimmingCharacters(in: .whitespacesAndNewlines)?.uppercased()
    }
}

// MARK: - Custom Optional Operators

/**
 * Custom nil coalescing assignment operator
 * 
 * - Parameters:
 *   - lhs: Optional value to assign to
 *   - rhs: Default value to assign if lhs is nil
 */
infix operator ??=: AssignmentPrecedence

func ??= <T>(lhs: inout T?, rhs: T) {
    if lhs == nil {
        lhs = rhs
    }
}

// MARK: - Optional Validation Patterns

/**
 * Demonstrates optional validation patterns for production apps
 * 
 * This class covers:
 * - Input validation with optionals
 * - Type conversion with optionals
 * - Business logic validation
 * - Error handling and reporting
 */
class OptionalValidationPatterns {
    
    // MARK: - Input Validation
    
    /**
     * Validates user input with optional handling
     * 
     * - Parameter input: User input to validate
     * - Returns: Validation result
     */
    func validateUserInput(_ input: String?) -> ValidationResult {
        guard let value = input else {
            return .failure(.nilValue)
        }
        
        guard !value.isEmpty else {
            return .failure(.emptyValue)
        }
        
        guard value.count >= 3 else {
            return .failure(.tooShort)
        }
        
        guard value.count <= 50 else {
            return .failure(.tooLong)
        }
        
        return .success(value)
    }
    
    /**
     * Validates email input with optional handling
     * 
     * - Parameter email: Email input to validate
     * - Returns: Validation result
     */
    func validateEmail(_ email: String?) -> ValidationResult {
        guard let value = email else {
            return .failure(.nilValue)
        }
        
        guard !value.isEmpty else {
            return .failure(.emptyValue)
        }
        
        guard value.contains("@") else {
            return .failure(.invalidEmail)
        }
        
        guard value.count <= 100 else {
            return .failure(.tooLong)
        }
        
        return .success(value)
    }
    
    // MARK: - Type Conversion
    
    /**
     * Converts string to integer with optional handling
     * 
     * - Parameter string: String to convert
     * - Returns: Conversion result
     */
    func convertToInt(_ string: String?) -> ConversionResult<Int> {
        guard let value = string else {
            return .failure(.nilValue)
        }
        
        guard let intValue = Int(value) else {
            return .failure(.conversionFailed)
        }
        
        return .success(intValue)
    }
    
    /**
     * Converts string to double with optional handling
     * 
     * - Parameter string: String to convert
     * - Returns: Conversion result
     */
    func convertToDouble(_ string: String?) -> ConversionResult<Double> {
        guard let value = string else {
            return .failure(.nilValue)
        }
        
        guard let doubleValue = Double(value) else {
            return .failure(.conversionFailed)
        }
        
        return .success(doubleValue)
    }
    
    // MARK: - Business Logic Validation
    
    /**
     * Validates user age with business rules
     * 
     * - Parameter ageString: Age string to validate
     * - Returns: Validation result
     */
    func validateUserAge(_ ageString: String?) -> ValidationResult {
        guard let value = ageString else {
            return .failure(.nilValue)
        }
        
        guard let age = Int(value) else {
            return .failure(.conversionFailed)
        }
        
        guard age >= 13 else {
            return .failure(.tooYoung)
        }
        
        guard age <= 120 else {
            return .failure(.tooOld)
        }
        
        return .success(value)
    }
    
    /**
     * Validates password strength with optional handling
     * 
     * - Parameter password: Password to validate
     * - Returns: Validation result
     */
    func validatePassword(_ password: String?) -> ValidationResult {
        guard let value = password else {
            return .failure(.nilValue)
        }
        
        guard value.count >= 8 else {
            return .failure(.tooShort)
        }
        
        guard value.count <= 128 else {
            return .failure(.tooLong)
        }
        
        guard value.rangeOfCharacter(from: .decimalDigits) != nil else {
            return .failure(.noDigits)
        }
        
        guard value.rangeOfCharacter(from: .uppercaseLetters) != nil else {
            return .failure(.noUppercase)
        }
        
        return .success(value)
    }
}

// MARK: - Supporting Types for Validation

/**
 * Validation result type
 */
enum ValidationResult {
    case success(String)
    case failure(ValidationError)
}

/**
 * Conversion result type
 */
enum ConversionResult<T> {
    case success(T)
    case failure(ValidationError)
}

/**
 * Validation error types
 */
enum ValidationError: Error, LocalizedError {
    case nilValue
    case emptyValue
    case tooShort
    case tooLong
    case invalidEmail
    case tooYoung
    case tooOld
    case noDigits
    case noUppercase
    case conversionFailed
    
    var errorDescription: String? {
        switch self {
        case .nilValue:
            return "Value is nil"
        case .emptyValue:
            return "Value is empty"
        case .tooShort:
            return "Value is too short"
        case .tooLong:
            return "Value is too long"
        case .invalidEmail:
            return "Invalid email format"
        case .tooYoung:
            return "Age is too young"
        case .tooOld:
            return "Age is too old"
        case .noDigits:
            return "No digits found"
        case .noUppercase:
            return "No uppercase letters found"
        case .conversionFailed:
            return "Conversion failed"
        }
    }
}

// MARK: - Optional Error Handling Patterns

/**
 * Demonstrates optional error handling patterns for production apps
 * 
 * This class covers:
 * - Optional with error handling
 * - Result type with optionals
 * - Error propagation with optionals
 * - Recovery strategies
 */
class OptionalErrorHandlingPatterns {
    
    // MARK: - Optional with Error Handling
    
    /**
     * Demonstrates optional with error handling
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or throws error
     * - Throws: OptionalError if value is nil
     */
    func processOptionalWithError(_ optionalValue: String?) throws -> String {
        guard let value = optionalValue else {
            throw OptionalError.nilValue
        }
        
        return value.uppercased()
    }
    
    /**
     * Demonstrates optional with custom error handling
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or throws error
     * - Throws: ValidationError if value is invalid
     */
    func processOptionalWithCustomError(_ optionalValue: String?) throws -> String {
        guard let value = optionalValue else {
            throw ValidationError.nilValue
        }
        
        guard !value.isEmpty else {
            throw ValidationError.emptyValue
        }
        
        return value.uppercased()
    }
    
    // MARK: - Result Type with Optionals
    
    /**
     * Demonstrates Result type with optional handling
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Result with processed value or error
     */
    func processOptionalWithResult(_ optionalValue: String?) -> Result<String, OptionalError> {
        guard let value = optionalValue else {
            return .failure(.nilValue)
        }
        
        guard !value.isEmpty else {
            return .failure(.invalidValue)
        }
        
        return .success(value.uppercased())
    }
    
    /**
     * Demonstrates Result type with custom error handling
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Result with processed value or error
     */
    func processOptionalWithCustomResult(_ optionalValue: String?) -> Result<String, ValidationError> {
        guard let value = optionalValue else {
            return .failure(.nilValue)
        }
        
        guard !value.isEmpty else {
            return .failure(.emptyValue)
        }
        
        return .success(value.uppercased())
    }
    
    // MARK: - Error Recovery Strategies
    
    /**
     * Demonstrates error recovery with fallback values
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or fallback
     */
    func processOptionalWithFallback(_ optionalValue: String?) -> String {
        guard let value = optionalValue else {
            return "Default Value"
        }
        
        guard !value.isEmpty else {
            return "Empty Value"
        }
        
        return value.uppercased()
    }
    
    /**
     * Demonstrates error recovery with retry logic
     * 
     * - Parameter optionalValue: Optional value to process
     * - Returns: Processed value or retry result
     */
    func processOptionalWithRetry(_ optionalValue: String?) -> String {
        guard let value = optionalValue else {
            // Retry with default value
            return processOptionalWithRetry("Default Value")
        }
        
        guard !value.isEmpty else {
            // Retry with fallback value
            return processOptionalWithRetry("Fallback Value")
        }
        
        return value.uppercased()
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the optional patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateOptionals() {
    print("=== Swift Optionals Demonstration ===\n")
    
    // Optional Fundamentals
    let fundamentalExample = OptionalFundamentals()
    
    print("--- Optional Declaration ---")
    fundamentalExample.demonstrateOptionalDeclaration()
    
    print("\n--- Optional Binding ---")
    print(fundamentalExample.demonstrateOptionalBinding("Hello"))
    print(fundamentalExample.demonstrateOptionalBinding(nil))
    
    do {
        print(try fundamentalExample.demonstrateGuardLetBinding("World"))
        print(try fundamentalExample.demonstrateGuardLetBinding(nil))
    } catch {
        print("Error: \(error.localizedDescription)")
    }
    
    print("\n--- Optional Chaining ---")
    print(fundamentalExample.demonstrateOptionalChaining("hello"))
    print(fundamentalExample.demonstrateOptionalChaining(nil))
    
    print("\n--- Nil Coalescing ---")
    print(fundamentalExample.demonstrateNilCoalescing("Hello"))
    print(fundamentalExample.demonstrateNilCoalescing(nil))
    
    // Advanced Optional Patterns
    let advancedExample = AdvancedOptionalPatterns()
    
    print("\n--- Optional Mapping ---")
    print(advancedExample.demonstrateOptionalMapping("hello"))
    print(advancedExample.demonstrateOptionalMapping(nil))
    
    print("\n--- Optional Filtering ---")
    print(advancedExample.demonstrateOptionalFiltering("hello"))
    print(advancedExample.demonstrateOptionalFiltering(""))
    print(advancedExample.demonstrateOptionalFiltering(nil))
    
    // Optional Validation Patterns
    let validationExample = OptionalValidationPatterns()
    
    print("\n--- Input Validation ---")
    switch validationExample.validateUserInput("Hello") {
    case .success(let value):
        print("Valid input: \(value)")
    case .failure(let error):
        print("Validation error: \(error.localizedDescription)")
    }
    
    switch validationExample.validateUserInput(nil) {
    case .success(let value):
        print("Valid input: \(value)")
    case .failure(let error):
        print("Validation error: \(error.localizedDescription)")
    }
    
    print("\n--- Type Conversion ---")
    switch validationExample.convertToInt("42") {
    case .success(let value):
        print("Converted to int: \(value)")
    case .failure(let error):
        print("Conversion error: \(error.localizedDescription)")
    }
    
    switch validationExample.convertToInt("invalid") {
    case .success(let value):
        print("Converted to int: \(value)")
    case .failure(let error):
        print("Conversion error: \(error.localizedDescription)")
    }
    
    // Optional Error Handling Patterns
    let errorHandlingExample = OptionalErrorHandlingPatterns()
    
    print("\n--- Error Handling ---")
    do {
        print(try errorHandlingExample.processOptionalWithError("hello"))
        print(try errorHandlingExample.processOptionalWithError(nil))
    } catch {
        print("Error: \(error.localizedDescription)")
    }
    
    print("\n--- Result Type ---")
    switch errorHandlingExample.processOptionalWithResult("world") {
    case .success(let value):
        print("Processed: \(value)")
    case .failure(let error):
        print("Error: \(error.localizedDescription)")
    }
    
    switch errorHandlingExample.processOptionalWithResult(nil) {
    case .success(let value):
        print("Processed: \(value)")
    case .failure(let error):
        print("Error: \(error.localizedDescription)")
    }
    
    print("\n--- Error Recovery ---")
    print(errorHandlingExample.processOptionalWithFallback("hello"))
    print(errorHandlingExample.processOptionalWithFallback(nil))
    print(errorHandlingExample.processOptionalWithRetry("world"))
    print(errorHandlingExample.processOptionalWithRetry(nil))
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateOptionals()
