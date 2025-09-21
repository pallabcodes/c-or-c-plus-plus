/*
 * Swift Fundamentals: Syntax & Types
 * 
 * This file demonstrates production-grade Swift syntax and type system usage
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master Swift's type system and type safety
 * - Understand value types vs reference types
 * - Implement proper type casting and conversion
 * - Apply type inference effectively
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Type System Fundamentals

/**
 * Demonstrates Swift's powerful type system with production-grade examples
 * 
 * Swift's type system provides:
 * - Compile-time type safety
 * - Automatic memory management
 * - Value and reference semantics
 * - Protocol-oriented programming
 */
class SwiftTypeSystem {
    
    // MARK: - Value Types vs Reference Types
    
    /**
     * Demonstrates the difference between value types (structs) and reference types (classes)
     * 
     * Value Types (Structs):
     * - Copied when assigned or passed as parameters
     * - Stored on the stack (faster access)
     * - Immutable by default
     * - Thread-safe
     * 
     * Reference Types (Classes):
     * - Shared when assigned or passed as parameters
     * - Stored on the heap
     * - Mutable by default
     * - Require synchronization for thread safety
     */
    
    // MARK: - Struct (Value Type) Example
    
    /**
     * UserProfile represents a user's profile information
     * 
     * This struct demonstrates:
     * - Value semantics (copied on assignment)
     * - Immutable properties with computed properties
     * - Method implementations
     * - Custom initializers
     */
    struct UserProfile {
        // MARK: - Properties
        
        /// User's unique identifier
        let id: UUID
        
        /// User's display name
        let displayName: String
        
        /// User's email address
        let email: String
        
        /// User's creation timestamp
        let createdAt: Date
        
        /// User's last update timestamp
        private(set) var lastUpdated: Date
        
        // MARK: - Computed Properties
        
        /// Returns a formatted display name with validation
        var formattedDisplayName: String {
            return displayName.trimmingCharacters(in: .whitespacesAndNewlines)
        }
        
        /// Returns user's age in days since creation
        var ageInDays: Int {
            return Calendar.current.dateComponents([.day], from: createdAt, to: Date()).day ?? 0
        }
        
        /// Returns true if the user profile is valid
        var isValid: Bool {
            return !displayName.isEmpty && email.contains("@")
        }
        
        // MARK: - Initializers
        
        /**
         * Creates a new UserProfile instance
         * 
         * - Parameters:
         *   - id: Unique identifier (defaults to new UUID)
         *   - displayName: User's display name
         *   - email: User's email address
         *   - createdAt: Creation timestamp (defaults to current date)
         */
        init(
            id: UUID = UUID(),
            displayName: String,
            email: String,
            createdAt: Date = Date()
        ) {
            self.id = id
            self.displayName = displayName
            self.email = email
            self.createdAt = createdAt
            self.lastUpdated = createdAt
        }
        
        // MARK: - Methods
        
        /**
         * Updates the last updated timestamp
         * 
         * This method demonstrates how to modify a struct's mutable property
         * while maintaining value semantics for other properties
         */
        mutating func updateLastModified() {
            lastUpdated = Date()
        }
        
        /**
         * Creates a copy of the profile with updated display name
         * 
         * - Parameter newDisplayName: The new display name
         * - Returns: A new UserProfile instance with updated display name
         */
        func updatingDisplayName(_ newDisplayName: String) -> UserProfile {
            var updatedProfile = self
            updatedProfile.displayName = newDisplayName
            updatedProfile.updateLastModified()
            return updatedProfile
        }
    }
    
    // MARK: - Class (Reference Type) Example
    
    /**
     * UserSession represents an active user session
     * 
     * This class demonstrates:
     * - Reference semantics (shared on assignment)
     * - Mutable state management
     * - Memory management with weak references
     * - Protocol conformance
     */
    class UserSession: CustomStringConvertible {
        // MARK: - Properties
        
        /// The user profile associated with this session
        let userProfile: UserProfile
        
        /// Session creation timestamp
        let createdAt: Date
        
        /// Last activity timestamp
        private(set) var lastActivity: Date
        
        /// Session timeout interval in seconds
        let timeoutInterval: TimeInterval
        
        /// Weak reference to session manager to avoid retain cycles
        weak var sessionManager: SessionManager?
        
        // MARK: - Computed Properties
        
        /// Returns true if the session is still valid
        var isValid: Bool {
            let timeSinceLastActivity = Date().timeIntervalSince(lastActivity)
            return timeSinceLastActivity < timeoutInterval
        }
        
        /// Returns session duration in seconds
        var duration: TimeInterval {
            return lastActivity.timeIntervalSince(createdAt)
        }
        
        /// Custom string representation for debugging
        var description: String {
            return "UserSession(user: \(userProfile.displayName), valid: \(isValid))"
        }
        
        // MARK: - Initializers
        
        /**
         * Creates a new UserSession instance
         * 
         * - Parameters:
         *   - userProfile: The user profile for this session
         *   - timeoutInterval: Session timeout in seconds (default: 3600)
         *   - sessionManager: Weak reference to session manager
         */
        init(
            userProfile: UserProfile,
            timeoutInterval: TimeInterval = 3600,
            sessionManager: SessionManager? = nil
        ) {
            self.userProfile = userProfile
            self.createdAt = Date()
            self.lastActivity = Date()
            self.timeoutInterval = timeoutInterval
            self.sessionManager = sessionManager
        }
        
        // MARK: - Methods
        
        /**
         * Updates the last activity timestamp
         * 
         * This method demonstrates mutable state management in classes
         */
        func updateActivity() {
            lastActivity = Date()
        }
        
        /**
         * Invalidates the session
         * 
         * This method demonstrates proper cleanup and notification
         */
        func invalidate() {
            sessionManager?.removeSession(self)
        }
    }
    
    // MARK: - Session Manager (Reference Type with Collection Management)
    
    /**
     * SessionManager manages active user sessions
     * 
     * This class demonstrates:
     * - Collection management with reference types
     * - Memory management and cleanup
     * - Thread safety considerations
     */
    class SessionManager {
        // MARK: - Properties
        
        /// Active sessions dictionary (userID -> session)
        private var activeSessions: [UUID: UserSession] = [:]
        
        /// Serial queue for thread safety
        private let sessionQueue = DispatchQueue(label: "com.company.sessionmanager", attributes: .concurrent)
        
        /// Maximum number of concurrent sessions
        let maxSessions: Int
        
        // MARK: - Computed Properties
        
        /// Current number of active sessions
        var sessionCount: Int {
            return sessionQueue.sync { activeSessions.count }
        }
        
        /// Array of all active sessions
        var allSessions: [UserSession] {
            return sessionQueue.sync { Array(activeSessions.values) }
        }
        
        // MARK: - Initializers
        
        /**
         * Creates a new SessionManager instance
         * 
         * - Parameter maxSessions: Maximum number of concurrent sessions (default: 1000)
         */
        init(maxSessions: Int = 1000) {
            self.maxSessions = maxSessions
        }
        
        // MARK: - Session Management
        
        /**
         * Creates a new session for the given user profile
         * 
         * - Parameter userProfile: The user profile to create a session for
         * - Returns: The created session, or nil if max sessions reached
         */
        func createSession(for userProfile: UserProfile) -> UserSession? {
            return sessionQueue.sync(flags: .barrier) {
                // Check if we've reached the maximum number of sessions
                guard activeSessions.count < maxSessions else {
                    return nil
                }
                
                // Check if user already has an active session
                if let existingSession = activeSessions[userProfile.id] {
                    existingSession.updateActivity()
                    return existingSession
                }
                
                // Create new session
                let session = UserSession(userProfile: userProfile, sessionManager: self)
                activeSessions[userProfile.id] = session
                return session
            }
        }
        
        /**
         * Removes a session from the active sessions
         * 
         * - Parameter session: The session to remove
         */
        func removeSession(_ session: UserSession) {
            sessionQueue.sync(flags: .barrier) {
                activeSessions.removeValue(forKey: session.userProfile.id)
            }
        }
        
        /**
         * Gets a session for a specific user ID
         * 
         * - Parameter userID: The user ID to look up
         * - Returns: The session if found and valid, nil otherwise
         */
        func session(for userID: UUID) -> UserSession? {
            return sessionQueue.sync {
                guard let session = activeSessions[userID] else {
                    return nil
                }
                
                // Check if session is still valid
                if session.isValid {
                    return session
                } else {
                    // Remove invalid session
                    activeSessions.removeValue(forKey: userID)
                    return nil
                }
            }
        }
        
        /**
         * Cleans up all invalid sessions
         * 
         * This method demonstrates proper cleanup and memory management
         */
        func cleanupInvalidSessions() {
            sessionQueue.sync(flags: .barrier) {
                let invalidSessions = activeSessions.values.filter { !$0.isValid }
                for session in invalidSessions {
                    activeSessions.removeValue(forKey: session.userProfile.id)
                }
            }
        }
    }
}

// MARK: - Type Casting and Conversion Examples

/**
 * Demonstrates various type casting and conversion patterns
 * used in production iOS applications
 */
class TypeCastingExamples {
    
    /**
     * Demonstrates safe type casting with optional binding
     * 
     * - Parameter object: Any object to cast
     * - Returns: Casted string if successful, nil otherwise
     */
    static func safeStringCast(_ object: Any) -> String? {
        // Safe casting with optional binding
        if let string = object as? String {
            return string
        }
        
        // Alternative: using guard statement
        guard let string = object as? String else {
            return nil
        }
        
        return string
    }
    
    /**
     * Demonstrates forced type casting with error handling
     * 
     * - Parameter object: Any object to cast
     * - Returns: Casted string or throws error
     * - Throws: TypeCastingError if casting fails
     */
    static func forcedStringCast(_ object: Any) throws -> String {
        guard let string = object as? String else {
            throw TypeCastingError.invalidType(expected: String.self, actual: type(of: object))
        }
        return string
    }
    
    /**
     * Demonstrates type checking before casting
     * 
     * - Parameter object: Any object to check
     * - Returns: True if object is a string
     */
    static func isString(_ object: Any) -> Bool {
        return object is String
    }
}

// MARK: - Type Casting Error

/**
 * Custom error type for type casting failures
 * 
 * This demonstrates proper error handling in Swift
 */
enum TypeCastingError: Error, LocalizedError {
    case invalidType(expected: Any.Type, actual: Any.Type)
    
    var errorDescription: String? {
        switch self {
        case .invalidType(let expected, let actual):
            return "Expected type \(expected), but got \(actual)"
        }
    }
}

// MARK: - Type Inference Examples

/**
 * Demonstrates Swift's powerful type inference capabilities
 * 
 * These examples show how Swift can infer types in various contexts
 */
class TypeInferenceExamples {
    
    /**
     * Demonstrates type inference in various contexts
     */
    static func demonstrateTypeInference() {
        // Array type inference
        let numbers = [1, 2, 3, 4, 5] // Inferred as [Int]
        let names = ["Alice", "Bob", "Charlie"] // Inferred as [String]
        
        // Dictionary type inference
        let userAges = ["Alice": 25, "Bob": 30, "Charlie": 35] // Inferred as [String: Int]
        
        // Function type inference
        let add = { (a: Int, b: Int) in a + b } // Inferred as (Int, Int) -> Int
        
        // Generic type inference
        let result = processItems([1, 2, 3]) { $0 * 2 } // Inferred as [Int]
        
        // Optional type inference
        let optionalString: String? = "Hello" // Explicit optional type
        let inferredOptional = Optional("World") // Inferred as String?
        
        print("Numbers: \(numbers)")
        print("Names: \(names)")
        print("User ages: \(userAges)")
        print("Add function result: \(add(5, 3))")
        print("Processed items: \(result)")
        print("Optional string: \(optionalString ?? "nil")")
        print("Inferred optional: \(inferredOptional ?? "nil")")
    }
    
    /**
     * Generic function demonstrating type inference
     * 
     * - Parameters:
     *   - items: Array of items to process
     *   - transform: Transformation function
     * - Returns: Array of transformed items
     */
    static func processItems<T, U>(_ items: [T], transform: (T) -> U) -> [U] {
        return items.map(transform)
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use the type system examples
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateTypeSystem() {
    print("=== Swift Type System Demonstration ===\n")
    
    // Create a user profile (value type)
    let userProfile = UserProfile(
        displayName: "John Doe",
        email: "john.doe@example.com"
    )
    
    print("User Profile: \(userProfile)")
    print("Formatted Name: \(userProfile.formattedDisplayName)")
    print("Age in Days: \(userProfile.ageInDays)")
    print("Is Valid: \(userProfile.isValid)\n")
    
    // Create a session manager
    let sessionManager = SwiftTypeSystem.SessionManager(maxSessions: 100)
    
    // Create a session (reference type)
    if let session = sessionManager.createSession(for: userProfile) {
        print("Session Created: \(session)")
        print("Session Valid: \(session.isValid)")
        print("Session Duration: \(session.duration) seconds\n")
        
        // Update session activity
        session.updateActivity()
        print("Updated Session: \(session)\n")
    }
    
    // Demonstrate type casting
    let objects: [Any] = ["Hello", 42, 3.14, true]
    
    for object in objects {
        if let string = TypeCastingExamples.safeStringCast(object) {
            print("Found string: \(string)")
        } else {
            print("Object is not a string: \(type(of: object))")
        }
    }
    
    print("\n=== Type Inference Examples ===")
    TypeInferenceExamples.demonstrateTypeInference()
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateTypeSystem()
