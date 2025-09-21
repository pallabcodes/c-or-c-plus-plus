/*
 * Advanced Swift: Error Handling
 * 
 * This file demonstrates production-grade error handling patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master comprehensive error handling strategies
 * - Understand custom error types and hierarchies
 * - Implement Result type and functional error handling
 * - Apply error recovery and graceful degradation patterns
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Error Type Definitions

/**
 * Comprehensive error hierarchy for production iOS applications
 * 
 * This enum demonstrates proper error categorization and hierarchy
 * following industry best practices from top-tier companies
 */
enum AppError: Error, LocalizedError, CustomNSError {
    // MARK: - Network Errors
    
    case networkUnavailable
    case requestTimeout
    case invalidURL
    case serverError(Int, String)
    case clientError(Int, String)
    case noData
    case invalidResponse
    case decodingError(String)
    case encodingError(String)
    
    // MARK: - Authentication Errors
    
    case authenticationRequired
    case invalidCredentials
    case tokenExpired
    case tokenInvalid
    case accountLocked
    case accountSuspended
    case twoFactorRequired
    case biometricNotAvailable
    case biometricNotEnrolled
    
    // MARK: - Data Errors
    
    case dataCorrupted
    case dataNotFound
    case dataValidationFailed(String)
    case dataSyncFailed
    case dataMigrationFailed
    case dataBackupFailed
    case dataRestoreFailed
    
    // MARK: - File System Errors
    
    case fileNotFound
    case fileAccessDenied
    case fileCorrupted
    case fileTooLarge
    case insufficientStorage
    case diskFull
    case permissionDenied
    
    // MARK: - Business Logic Errors
    
    case invalidInput(String)
    case businessRuleViolation(String)
    case operationNotAllowed
    case quotaExceeded
    case rateLimitExceeded
    case maintenanceMode
    case featureUnavailable
    
    // MARK: - System Errors
    
    case systemError(String)
    case memoryError
    case cpuError
    case batteryLow
    case thermalThrottling
    case hardwareFailure
    
    // MARK: - Custom Error Properties
    
    var errorDescription: String? {
        switch self {
        // Network Errors
        case .networkUnavailable:
            return "Network connection is unavailable"
        case .requestTimeout:
            return "Request timed out"
        case .invalidURL:
            return "Invalid URL provided"
        case .serverError(let code, let message):
            return "Server error \(code): \(message)"
        case .clientError(let code, let message):
            return "Client error \(code): \(message)"
        case .noData:
            return "No data received"
        case .invalidResponse:
            return "Invalid response format"
        case .decodingError(let message):
            return "Decoding error: \(message)"
        case .encodingError(let message):
            return "Encoding error: \(message)"
            
        // Authentication Errors
        case .authenticationRequired:
            return "Authentication is required"
        case .invalidCredentials:
            return "Invalid credentials provided"
        case .tokenExpired:
            return "Authentication token has expired"
        case .tokenInvalid:
            return "Authentication token is invalid"
        case .accountLocked:
            return "Account is locked"
        case .accountSuspended:
            return "Account is suspended"
        case .twoFactorRequired:
            return "Two-factor authentication is required"
        case .biometricNotAvailable:
            return "Biometric authentication is not available"
        case .biometricNotEnrolled:
            return "Biometric authentication is not enrolled"
            
        // Data Errors
        case .dataCorrupted:
            return "Data is corrupted"
        case .dataNotFound:
            return "Data not found"
        case .dataValidationFailed(let message):
            return "Data validation failed: \(message)"
        case .dataSyncFailed:
            return "Data synchronization failed"
        case .dataMigrationFailed:
            return "Data migration failed"
        case .dataBackupFailed:
            return "Data backup failed"
        case .dataRestoreFailed:
            return "Data restore failed"
            
        // File System Errors
        case .fileNotFound:
            return "File not found"
        case .fileAccessDenied:
            return "File access denied"
        case .fileCorrupted:
            return "File is corrupted"
        case .fileTooLarge:
            return "File is too large"
        case .insufficientStorage:
            return "Insufficient storage space"
        case .diskFull:
            return "Disk is full"
        case .permissionDenied:
            return "Permission denied"
            
        // Business Logic Errors
        case .invalidInput(let message):
            return "Invalid input: \(message)"
        case .businessRuleViolation(let message):
            return "Business rule violation: \(message)"
        case .operationNotAllowed:
            return "Operation is not allowed"
        case .quotaExceeded:
            return "Quota exceeded"
        case .rateLimitExceeded:
            return "Rate limit exceeded"
        case .maintenanceMode:
            return "System is in maintenance mode"
        case .featureUnavailable:
            return "Feature is not available"
            
        // System Errors
        case .systemError(let message):
            return "System error: \(message)"
        case .memoryError:
            return "Memory error occurred"
        case .cpuError:
            return "CPU error occurred"
        case .batteryLow:
            return "Battery is low"
        case .thermalThrottling:
            return "Thermal throttling is active"
        case .hardwareFailure:
            return "Hardware failure detected"
        }
    }
    
    var failureReason: String? {
        switch self {
        case .networkUnavailable:
            return "Check your internet connection and try again"
        case .requestTimeout:
            return "The request took too long to complete"
        case .invalidURL:
            return "The provided URL is malformed"
        case .serverError(let code, _):
            return "Server returned error code \(code)"
        case .clientError(let code, _):
            return "Client error with code \(code)"
        case .noData:
            return "The server returned no data"
        case .invalidResponse:
            return "The response format is not as expected"
        case .decodingError:
            return "Failed to decode the response data"
        case .encodingError:
            return "Failed to encode the request data"
        case .authenticationRequired:
            return "Please log in to continue"
        case .invalidCredentials:
            return "Username or password is incorrect"
        case .tokenExpired:
            return "Your session has expired, please log in again"
        case .tokenInvalid:
            return "Your session is invalid, please log in again"
        case .accountLocked:
            return "Your account has been locked for security reasons"
        case .accountSuspended:
            return "Your account has been suspended"
        case .twoFactorRequired:
            return "Please complete two-factor authentication"
        case .biometricNotAvailable:
            return "Biometric authentication is not supported on this device"
        case .biometricNotEnrolled:
            return "Please set up biometric authentication in Settings"
        case .dataCorrupted:
            return "The data has been corrupted and cannot be used"
        case .dataNotFound:
            return "The requested data could not be found"
        case .dataValidationFailed:
            return "The data failed validation checks"
        case .dataSyncFailed:
            return "Failed to synchronize data with the server"
        case .dataMigrationFailed:
            return "Failed to migrate data to the new format"
        case .dataBackupFailed:
            return "Failed to backup your data"
        case .dataRestoreFailed:
            return "Failed to restore your data"
        case .fileNotFound:
            return "The requested file could not be found"
        case .fileAccessDenied:
            return "You don't have permission to access this file"
        case .fileCorrupted:
            return "The file is corrupted and cannot be opened"
        case .fileTooLarge:
            return "The file is too large to process"
        case .insufficientStorage:
            return "There is not enough storage space available"
        case .diskFull:
            return "The disk is full and cannot store more data"
        case .permissionDenied:
            return "You don't have permission to perform this action"
        case .invalidInput:
            return "The input provided is not valid"
        case .businessRuleViolation:
            return "This action violates a business rule"
        case .operationNotAllowed:
            return "This operation is not allowed in the current context"
        case .quotaExceeded:
            return "You have exceeded your quota limit"
        case .rateLimitExceeded:
            return "You have exceeded the rate limit, please try again later"
        case .maintenanceMode:
            return "The system is currently under maintenance"
        case .featureUnavailable:
            return "This feature is not available in your region or plan"
        case .systemError:
            return "An internal system error occurred"
        case .memoryError:
            return "A memory error occurred while processing your request"
        case .cpuError:
            return "A CPU error occurred while processing your request"
        case .batteryLow:
            return "Your device battery is low, please charge it"
        case .thermalThrottling:
            return "Your device is overheating and performance is reduced"
        case .hardwareFailure:
            return "A hardware failure has been detected"
        }
    }
    
    var recoverySuggestion: String? {
        switch self {
        case .networkUnavailable:
            return "Check your internet connection and try again"
        case .requestTimeout:
            return "Try again in a few moments"
        case .invalidURL:
            return "Please check the URL and try again"
        case .serverError:
            return "Please try again later or contact support"
        case .clientError:
            return "Please check your request and try again"
        case .noData:
            return "Please try again or contact support"
        case .invalidResponse:
            return "Please try again or contact support"
        case .decodingError:
            return "Please try again or contact support"
        case .encodingError:
            return "Please try again or contact support"
        case .authenticationRequired:
            return "Please log in to continue"
        case .invalidCredentials:
            return "Please check your username and password"
        case .tokenExpired:
            return "Please log in again"
        case .tokenInvalid:
            return "Please log in again"
        case .accountLocked:
            return "Please contact support to unlock your account"
        case .accountSuspended:
            return "Please contact support for assistance"
        case .twoFactorRequired:
            return "Please complete two-factor authentication"
        case .biometricNotAvailable:
            return "Please use password authentication instead"
        case .biometricNotEnrolled:
            return "Please set up biometric authentication in Settings"
        case .dataCorrupted:
            return "Please try again or contact support"
        case .dataNotFound:
            return "Please try again or contact support"
        case .dataValidationFailed:
            return "Please check your input and try again"
        case .dataSyncFailed:
            return "Please try again or contact support"
        case .dataMigrationFailed:
            return "Please contact support for assistance"
        case .dataBackupFailed:
            return "Please try again or contact support"
        case .dataRestoreFailed:
            return "Please try again or contact support"
        case .fileNotFound:
            return "Please check the file path and try again"
        case .fileAccessDenied:
            return "Please check your permissions and try again"
        case .fileCorrupted:
            return "Please try again or contact support"
        case .fileTooLarge:
            return "Please try a smaller file or contact support"
        case .insufficientStorage:
            return "Please free up some space and try again"
        case .diskFull:
            return "Please free up some space and try again"
        case .permissionDenied:
            return "Please check your permissions and try again"
        case .invalidInput:
            return "Please check your input and try again"
        case .businessRuleViolation:
            return "Please check the rules and try again"
        case .operationNotAllowed:
            return "Please check the context and try again"
        case .quotaExceeded:
            return "Please wait for the quota to reset or upgrade your plan"
        case .rateLimitExceeded:
            return "Please wait a moment and try again"
        case .maintenanceMode:
            return "Please try again later"
        case .featureUnavailable:
            return "Please check your plan or region"
        case .systemError:
            return "Please try again or contact support"
        case .memoryError:
            return "Please try again or restart the app"
        case .cpuError:
            return "Please try again or restart the app"
        case .batteryLow:
            return "Please charge your device and try again"
        case .thermalThrottling:
            return "Please let your device cool down and try again"
        case .hardwareFailure:
            return "Please contact support for assistance"
        }
    }
    
    var errorCode: Int {
        switch self {
        // Network Errors (1000-1999)
        case .networkUnavailable: return 1001
        case .requestTimeout: return 1002
        case .invalidURL: return 1003
        case .serverError: return 1004
        case .clientError: return 1005
        case .noData: return 1006
        case .invalidResponse: return 1007
        case .decodingError: return 1008
        case .encodingError: return 1009
        
        // Authentication Errors (2000-2999)
        case .authenticationRequired: return 2001
        case .invalidCredentials: return 2002
        case .tokenExpired: return 2003
        case .tokenInvalid: return 2004
        case .accountLocked: return 2005
        case .accountSuspended: return 2006
        case .twoFactorRequired: return 2007
        case .biometricNotAvailable: return 2008
        case .biometricNotEnrolled: return 2009
        
        // Data Errors (3000-3999)
        case .dataCorrupted: return 3001
        case .dataNotFound: return 3002
        case .dataValidationFailed: return 3003
        case .dataSyncFailed: return 3004
        case .dataMigrationFailed: return 3005
        case .dataBackupFailed: return 3006
        case .dataRestoreFailed: return 3007
        
        // File System Errors (4000-4999)
        case .fileNotFound: return 4001
        case .fileAccessDenied: return 4002
        case .fileCorrupted: return 4003
        case .fileTooLarge: return 4004
        case .insufficientStorage: return 4005
        case .diskFull: return 4006
        case .permissionDenied: return 4007
        
        // Business Logic Errors (5000-5999)
        case .invalidInput: return 5001
        case .businessRuleViolation: return 5002
        case .operationNotAllowed: return 5003
        case .quotaExceeded: return 5004
        case .rateLimitExceeded: return 5005
        case .maintenanceMode: return 5006
        case .featureUnavailable: return 5007
        
        // System Errors (6000-6999)
        case .systemError: return 6001
        case .memoryError: return 6002
        case .cpuError: return 6003
        case .batteryLow: return 6004
        case .thermalThrottling: return 6005
        case .hardwareFailure: return 6006
        }
    }
    
    var userInfo: [String: Any] {
        var info: [String: Any] = [
            NSLocalizedDescriptionKey: errorDescription ?? "Unknown error",
            NSLocalizedFailureReasonErrorKey: failureReason ?? "Unknown reason",
            NSLocalizedRecoverySuggestionErrorKey: recoverySuggestion ?? "Please try again"
        ]
        
        switch self {
        case .serverError(let code, let message):
            info["serverCode"] = code
            info["serverMessage"] = message
        case .clientError(let code, let message):
            info["clientCode"] = code
            info["clientMessage"] = message
        case .dataValidationFailed(let message):
            info["validationMessage"] = message
        case .invalidInput(let message):
            info["inputMessage"] = message
        case .businessRuleViolation(let message):
            info["ruleMessage"] = message
        case .systemError(let message):
            info["systemMessage"] = message
        default:
            break
        }
        
        return info
    }
}

// MARK: - Error Handling Strategies

/**
 * Demonstrates comprehensive error handling strategies used in production iOS apps
 * 
 * This class covers:
 * - Throwing functions and error propagation
 * - Do-catch error handling patterns
 * - Error recovery and fallback strategies
 * - Error logging and monitoring
 */
class ErrorHandlingStrategies {
    
    // MARK: - Throwing Functions
    
    /**
     * Demonstrates a throwing function with comprehensive error handling
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Data fetched from the URL
     * - Throws: AppError if the operation fails
     */
    func fetchData(from url: URL) throws -> Data {
        // Validate URL
        guard url.scheme == "https" else {
            throw AppError.invalidURL
        }
        
        // Simulate network request
        let request = URLRequest(url: url)
        let semaphore = DispatchSemaphore(value: 0)
        var result: Data?
        var error: Error?
        
        URLSession.shared.dataTask(with: request) { data, response, networkError in
            if let networkError = networkError {
                error = networkError
            } else if let httpResponse = response as? HTTPURLResponse {
                if httpResponse.statusCode >= 500 {
                    error = AppError.serverError(httpResponse.statusCode, "Server error")
                } else if httpResponse.statusCode >= 400 {
                    error = AppError.clientError(httpResponse.statusCode, "Client error")
                } else if data == nil {
                    error = AppError.noData
                } else {
                    result = data
                }
            } else {
                error = AppError.invalidResponse
            }
            semaphore.signal()
        }.resume()
        
        semaphore.wait()
        
        if let error = error {
            throw error
        }
        
        guard let data = result else {
            throw AppError.noData
        }
        
        return data
    }
    
    /**
     * Demonstrates a throwing function with data validation
     * 
     * - Parameter data: Data to validate
     * - Returns: Validated data
     * - Throws: AppError if validation fails
     */
    func validateData(_ data: Data) throws -> Data {
        // Check if data is empty
        guard !data.isEmpty else {
            throw AppError.dataNotFound
        }
        
        // Check if data is too large (10MB limit)
        guard data.count <= 10 * 1024 * 1024 else {
            throw AppError.fileTooLarge
        }
        
        // Check if data is valid JSON
        do {
            _ = try JSONSerialization.jsonObject(with: data)
        } catch {
            throw AppError.dataValidationFailed("Invalid JSON format")
        }
        
        return data
    }
    
    /**
     * Demonstrates a throwing function with business logic validation
     * 
     * - Parameter user: User data to validate
     * - Returns: Validated user data
     * - Throws: AppError if validation fails
     */
    func validateUser(_ user: User) throws -> User {
        // Validate username
        guard !user.username.isEmpty else {
            throw AppError.invalidInput("Username cannot be empty")
        }
        
        guard user.username.count >= 3 else {
            throw AppError.invalidInput("Username must be at least 3 characters")
        }
        
        guard user.username.count <= 50 else {
            throw AppError.invalidInput("Username must be at most 50 characters")
        }
        
        // Validate email
        guard !user.email.isEmpty else {
            throw AppError.invalidInput("Email cannot be empty")
        }
        
        guard user.email.contains("@") else {
            throw AppError.invalidInput("Email must contain @ symbol")
        }
        
        // Validate age
        guard user.age >= 13 else {
            throw AppError.businessRuleViolation("User must be at least 13 years old")
        }
        
        guard user.age <= 120 else {
            throw AppError.businessRuleViolation("User age must be at most 120 years")
        }
        
        return user
    }
    
    // MARK: - Do-Catch Error Handling
    
    /**
     * Demonstrates comprehensive do-catch error handling
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Result with data or error
     */
    func fetchDataWithErrorHandling(from url: URL) -> Result<Data, AppError> {
        do {
            let data = try fetchData(from: url)
            let validatedData = try validateData(data)
            return .success(validatedData)
        } catch let error as AppError {
            return .failure(error)
        } catch {
            return .failure(.systemError("Unexpected error: \(error.localizedDescription)"))
        }
    }
    
    /**
     * Demonstrates error handling with specific error types
     * 
     * - Parameter user: User data to process
     * - Returns: Result with processed user or error
     */
    func processUserWithErrorHandling(_ user: User) -> Result<User, AppError> {
        do {
            let validatedUser = try validateUser(user)
            return .success(validatedUser)
        } catch let error as AppError {
            return .failure(error)
        } catch {
            return .failure(.systemError("Unexpected error: \(error.localizedDescription)"))
        }
    }
    
    // MARK: - Error Recovery Strategies
    
    /**
     * Demonstrates error recovery with fallback strategies
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Data or fallback data
     */
    func fetchDataWithRecovery(from url: URL) -> Data {
        do {
            let data = try fetchData(from: url)
            return data
        } catch AppError.networkUnavailable {
            // Return cached data if available
            return getCachedData() ?? Data()
        } catch AppError.requestTimeout {
            // Retry with shorter timeout
            return fetchDataWithRetry(from: url, maxRetries: 3)
        } catch AppError.serverError(let code, _) where code >= 500 {
            // Retry after delay for server errors
            Thread.sleep(forTimeInterval: 2.0)
            return fetchDataWithRecovery(from: url)
        } catch {
            // Log error and return empty data
            logError(error)
            return Data()
        }
    }
    
    /**
     * Demonstrates error recovery with retry logic
     * 
     * - Parameters:
     *   - url: URL to fetch data from
     *   - maxRetries: Maximum number of retries
     * - Returns: Data or empty data if all retries fail
     */
    func fetchDataWithRetry(from url: URL, maxRetries: Int) -> Data {
        for attempt in 1...maxRetries {
            do {
                let data = try fetchData(from: url)
                return data
            } catch AppError.networkUnavailable where attempt < maxRetries {
                // Wait before retry
                Thread.sleep(forTimeInterval: Double(attempt))
                continue
            } catch AppError.requestTimeout where attempt < maxRetries {
                // Wait before retry
                Thread.sleep(forTimeInterval: Double(attempt))
                continue
            } catch {
                // Log error and return empty data
                logError(error)
                return Data()
            }
        }
        
        return Data()
    }
    
    // MARK: - Error Logging and Monitoring
    
    /**
     * Demonstrates error logging for monitoring and debugging
     * 
     * - Parameter error: Error to log
     */
    func logError(_ error: Error) {
        let timestamp = Date()
        let errorInfo = [
            "timestamp": timestamp,
            "error": error.localizedDescription,
            "errorCode": (error as? AppError)?.errorCode ?? -1,
            "userInfo": (error as? AppError)?.userInfo ?? [:]
        ] as [String: Any]
        
        // Log to console (in production, this would go to a logging service)
        print("Error logged: \(errorInfo)")
        
        // In production, this would send to monitoring service
        // sendToMonitoringService(errorInfo)
    }
    
    /**
     * Demonstrates error monitoring and analytics
     * 
     * - Parameter error: Error to monitor
     */
    func monitorError(_ error: Error) {
        guard let appError = error as? AppError else { return }
        
        let errorMetrics = [
            "errorType": String(describing: type(of: appError)),
            "errorCode": appError.errorCode,
            "errorCategory": getErrorCategory(appError),
            "timestamp": Date(),
            "userAgent": "iOS App",
            "appVersion": "1.0.0"
        ]
        
        // In production, this would send to analytics service
        // sendToAnalyticsService(errorMetrics)
        print("Error monitored: \(errorMetrics)")
    }
    
    /**
     * Gets the category of an error for monitoring purposes
     * 
     * - Parameter error: Error to categorize
     * - Returns: Error category string
     */
    private func getErrorCategory(_ error: AppError) -> String {
        switch error {
        case .networkUnavailable, .requestTimeout, .invalidURL, .serverError, .clientError, .noData, .invalidResponse, .decodingError, .encodingError:
            return "Network"
        case .authenticationRequired, .invalidCredentials, .tokenExpired, .tokenInvalid, .accountLocked, .accountSuspended, .twoFactorRequired, .biometricNotAvailable, .biometricNotEnrolled:
            return "Authentication"
        case .dataCorrupted, .dataNotFound, .dataValidationFailed, .dataSyncFailed, .dataMigrationFailed, .dataBackupFailed, .dataRestoreFailed:
            return "Data"
        case .fileNotFound, .fileAccessDenied, .fileCorrupted, .fileTooLarge, .insufficientStorage, .diskFull, .permissionDenied:
            return "FileSystem"
        case .invalidInput, .businessRuleViolation, .operationNotAllowed, .quotaExceeded, .rateLimitExceeded, .maintenanceMode, .featureUnavailable:
            return "BusinessLogic"
        case .systemError, .memoryError, .cpuError, .batteryLow, .thermalThrottling, .hardwareFailure:
            return "System"
        }
    }
    
    // MARK: - Helper Methods
    
    /**
     * Gets cached data for fallback scenarios
     * 
     * - Returns: Cached data or nil if not available
     */
    private func getCachedData() -> Data? {
        // In production, this would retrieve from cache
        return nil
    }
}

// MARK: - Result Type Patterns

/**
 * Demonstrates Result type patterns used in production iOS apps
 * 
 * This class covers:
 * - Result type for functional error handling
 * - Result type with custom error types
 * - Result type chaining and transformation
 * - Result type with async operations
 */
class ResultTypePatterns {
    
    // MARK: - Basic Result Type Usage
    
    /**
     * Demonstrates basic Result type usage
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Result with data or error
     */
    func fetchDataResult(from url: URL) -> Result<Data, AppError> {
        // Simulate network request
        let request = URLRequest(url: url)
        let semaphore = DispatchSemaphore(value: 0)
        var result: Result<Data, AppError> = .failure(.networkUnavailable)
        
        URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                result = .failure(.networkUnavailable)
            } else if let httpResponse = response as? HTTPURLResponse {
                if httpResponse.statusCode >= 500 {
                    result = .failure(.serverError(httpResponse.statusCode, "Server error"))
                } else if httpResponse.statusCode >= 400 {
                    result = .failure(.clientError(httpResponse.statusCode, "Client error"))
                } else if let data = data {
                    result = .success(data)
                } else {
                    result = .failure(.noData)
                }
            } else {
                result = .failure(.invalidResponse)
            }
            semaphore.signal()
        }.resume()
        
        semaphore.wait()
        return result
    }
    
    /**
     * Demonstrates Result type with data validation
     * 
     * - Parameter data: Data to validate
     * - Returns: Result with validated data or error
     */
    func validateDataResult(_ data: Data) -> Result<Data, AppError> {
        guard !data.isEmpty else {
            return .failure(.dataNotFound)
        }
        
        guard data.count <= 10 * 1024 * 1024 else {
            return .failure(.fileTooLarge)
        }
        
        do {
            _ = try JSONSerialization.jsonObject(with: data)
            return .success(data)
        } catch {
            return .failure(.dataValidationFailed("Invalid JSON format"))
        }
    }
    
    // MARK: - Result Type Chaining
    
    /**
     * Demonstrates Result type chaining
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Result with processed data or error
     */
    func fetchAndValidateData(from url: URL) -> Result<Data, AppError> {
        return fetchDataResult(from: url)
            .flatMap { data in
                validateDataResult(data)
            }
    }
    
    /**
     * Demonstrates Result type transformation
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Result with processed data or error
     */
    func fetchAndProcessData(from url: URL) -> Result<ProcessedData, AppError> {
        return fetchAndValidateData(from: url)
            .map { data in
                ProcessedData(data: data, processedAt: Date())
            }
    }
    
    // MARK: - Result Type with Async Operations
    
    /**
     * Demonstrates Result type with async operations
     * 
     * - Parameter url: URL to fetch data from
     * - Parameter completion: Completion handler with result
     */
    func fetchDataAsync(from url: URL, completion: @escaping (Result<Data, AppError>) -> Void) {
        DispatchQueue.global(qos: .userInitiated).async {
            let result = self.fetchDataResult(from: url)
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    /**
     * Demonstrates Result type with async operations and chaining
     * 
     * - Parameter url: URL to fetch data from
     * - Parameter completion: Completion handler with result
     */
    func fetchAndValidateDataAsync(from url: URL, completion: @escaping (Result<Data, AppError>) -> Void) {
        DispatchQueue.global(qos: .userInitiated).async {
            let result = self.fetchAndValidateData(from: url)
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    // MARK: - Result Type with Error Recovery
    
    /**
     * Demonstrates Result type with error recovery
     * 
     * - Parameter url: URL to fetch data from
     * - Returns: Result with data or fallback data
     */
    func fetchDataWithRecovery(from url: URL) -> Result<Data, AppError> {
        let result = fetchDataResult(from: url)
        
        switch result {
        case .success(let data):
            return .success(data)
        case .failure(.networkUnavailable):
            // Return cached data if available
            if let cachedData = getCachedData() {
                return .success(cachedData)
            } else {
                return .failure(.networkUnavailable)
            }
        case .failure(.requestTimeout):
            // Retry with shorter timeout
            return fetchDataWithRetry(from: url, maxRetries: 3)
        case .failure(let error):
            return .failure(error)
        }
    }
    
    /**
     * Demonstrates Result type with retry logic
     * 
     * - Parameters:
     *   - url: URL to fetch data from
     *   - maxRetries: Maximum number of retries
     * - Returns: Result with data or error
     */
    func fetchDataWithRetry(from url: URL, maxRetries: Int) -> Result<Data, AppError> {
        for attempt in 1...maxRetries {
            let result = fetchDataResult(from: url)
            
            switch result {
            case .success(let data):
                return .success(data)
            case .failure(.networkUnavailable) where attempt < maxRetries:
                Thread.sleep(forTimeInterval: Double(attempt))
                continue
            case .failure(.requestTimeout) where attempt < maxRetries:
                Thread.sleep(forTimeInterval: Double(attempt))
                continue
            case .failure(let error):
                return .failure(error)
            }
        }
        
        return .failure(.requestTimeout)
    }
    
    // MARK: - Helper Methods
    
    /**
     * Gets cached data for fallback scenarios
     * 
     * - Returns: Cached data or nil if not available
     */
    private func getCachedData() -> Data? {
        // In production, this would retrieve from cache
        return nil
    }
}

// MARK: - Supporting Types

/**
 * User data structure for demonstration
 */
struct User {
    let username: String
    let email: String
    let age: Int
}

/**
 * Processed data structure for demonstration
 */
struct ProcessedData {
    let data: Data
    let processedAt: Date
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the error handling patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateErrorHandling() {
    print("=== Swift Error Handling Demonstration ===\n")
    
    // Error Handling Strategies
    let strategyExample = ErrorHandlingStrategies()
    
    print("--- Error Handling Strategies ---")
    let url = URL(string: "https://api.example.com/data")!
    
    // Throwing functions
    do {
        let data = try strategyExample.fetchData(from: url)
        print("Data fetched successfully: \(data.count) bytes")
    } catch let error as AppError {
        print("App error: \(error.localizedDescription)")
        print("Recovery suggestion: \(error.recoverySuggestion ?? "None")")
    } catch {
        print("Unexpected error: \(error.localizedDescription)")
    }
    
    // Do-catch error handling
    let result = strategyExample.fetchDataWithErrorHandling(from: url)
    switch result {
    case .success(let data):
        print("Data fetched successfully: \(data.count) bytes")
    case .failure(let error):
        print("Error: \(error.localizedDescription)")
    }
    
    // Error recovery
    let recoveredData = strategyExample.fetchDataWithRecovery(from: url)
    print("Recovered data: \(recoveredData.count) bytes")
    
    // Result Type Patterns
    let resultExample = ResultTypePatterns()
    
    print("\n--- Result Type Patterns ---")
    let resultData = resultExample.fetchDataResult(from: url)
    switch resultData {
    case .success(let data):
        print("Data fetched successfully: \(data.count) bytes")
    case .failure(let error):
        print("Error: \(error.localizedDescription)")
    }
    
    // Result type chaining
    let chainedResult = resultExample.fetchAndValidateData(from: url)
    switch chainedResult {
    case .success(let data):
        print("Data fetched and validated: \(data.count) bytes")
    case .failure(let error):
        print("Error: \(error.localizedDescription)")
    }
    
    // Result type with async operations
    resultExample.fetchDataAsync(from: url) { result in
        switch result {
        case .success(let data):
            print("Async data fetched: \(data.count) bytes")
        case .failure(let error):
            print("Async error: \(error.localizedDescription)")
        }
    }
    
    // Error recovery with Result type
    let recoveryResult = resultExample.fetchDataWithRecovery(from: url)
    switch recoveryResult {
    case .success(let data):
        print("Recovery data: \(data.count) bytes")
    case .failure(let error):
        print("Recovery error: \(error.localizedDescription)")
    }
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateErrorHandling()
