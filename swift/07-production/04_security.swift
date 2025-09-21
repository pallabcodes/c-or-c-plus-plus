/*
 * Swift Production: Security
 * 
 * This file demonstrates production-grade security practices in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master security best practices and secure coding
 * - Understand authentication and authorization patterns
 * - Implement proper data protection and encryption
 * - Apply vulnerability management and security scanning
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import Security
import CryptoKit

// MARK: - Security Manager

/**
 * Security manager
 * 
 * This class demonstrates proper security management
 * with comprehensive security practices and protection
 */
class SecurityManager {
    
    // MARK: - Properties
    
    private var securityConfig: SecurityConfiguration
    private var encryptionKey: SymmetricKey?
    private var securityAuditor: SecurityAuditor!
    
    // MARK: - Initialization
    
    init() {
        self.securityConfig = SecurityConfiguration()
        self.securityAuditor = SecurityAuditor()
        setupSecurity()
    }
    
    // MARK: - Public Methods
    
    /**
     * Encrypt data
     * 
     * This method demonstrates proper data encryption
     * with comprehensive encryption practices
     */
    func encryptData(_ data: Data, using key: SymmetricKey? = nil) throws -> EncryptedData {
        let encryptionKey = key ?? getEncryptionKey()
        
        let sealedBox = try AES.GCM.seal(data, using: encryptionKey)
        
        return EncryptedData(
            data: sealedBox.combined!,
            algorithm: .AES_GCM,
            keyId: "default",
            timestamp: Date()
        )
    }
    
    /**
     * Decrypt data
     * 
     * This method demonstrates proper data decryption
     * with comprehensive decryption practices
     */
    func decryptData(_ encryptedData: EncryptedData, using key: SymmetricKey? = nil) throws -> Data {
        let decryptionKey = key ?? getEncryptionKey()
        
        let sealedBox = try AES.GCM.SealedBox(combined: encryptedData.data)
        let decryptedData = try AES.GCM.open(sealedBox, using: decryptionKey)
        
        return decryptedData
    }
    
    /**
     * Hash password
     * 
     * This method demonstrates proper password hashing
     * with secure hashing practices
     */
    func hashPassword(_ password: String) throws -> String {
        let salt = generateSalt()
        let saltedPassword = password + salt
        
        let hashedData = SHA256.hash(data: saltedPassword.data(using: .utf8)!)
        let hashedString = hashedData.compactMap { String(format: "%02x", $0) }.joined()
        
        return "\(hashedString):\(salt)"
    }
    
    /**
     * Verify password
     * 
     * This method demonstrates proper password verification
     * with secure verification practices
     */
    func verifyPassword(_ password: String, hashedPassword: String) throws -> Bool {
        let components = hashedPassword.components(separatedBy: ":")
        guard components.count == 2 else { return false }
        
        let storedHash = components[0]
        let salt = components[1]
        
        let saltedPassword = password + salt
        let hashedData = SHA256.hash(data: saltedPassword.data(using: .utf8)!)
        let hashedString = hashedData.compactMap { String(format: "%02x", $0) }.joined()
        
        return hashedString == storedHash
    }
    
    /**
     * Generate secure token
     * 
     * This method demonstrates proper secure token generation
     * with comprehensive token security
     */
    func generateSecureToken(length: Int = 32) -> String {
        var bytes = [UInt8](repeating: 0, count: length)
        let result = SecRandomCopyBytes(kSecRandomDefault, length, &bytes)
        
        guard result == errSecSuccess else {
            // Fallback to less secure method
            return String((0..<length).map { _ in "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789".randomElement()! })
        }
        
        return Data(bytes).base64EncodedString()
    }
    
    /**
     * Validate input
     * 
     * This method demonstrates proper input validation
     * with comprehensive security validation
     */
    func validateInput(_ input: String, type: InputType) -> ValidationResult {
        var violations: [SecurityViolation] = []
        
        // Length validation
        if input.count > type.maxLength {
            violations.append(SecurityViolation(
                type: .lengthExceeded,
                message: "Input exceeds maximum length of \(type.maxLength)",
                severity: .medium
            ))
        }
        
        // Pattern validation
        if !type.pattern.isEmpty && !input.range(of: type.pattern, options: .regularExpression, range: nil, locale: nil) != nil {
            violations.append(SecurityViolation(
                type: .invalidPattern,
                message: "Input does not match required pattern",
                severity: .high
            ))
        }
        
        // SQL injection check
        if containsSQLInjection(input) {
            violations.append(SecurityViolation(
                type: .sqlInjection,
                message: "Input contains potential SQL injection",
                severity: .critical
            ))
        }
        
        // XSS check
        if containsXSS(input) {
            violations.append(SecurityViolation(
                type: .xss,
                message: "Input contains potential XSS",
                severity: .critical
            ))
        }
        
        return ValidationResult(
            isValid: violations.isEmpty,
            violations: violations
        )
    }
    
    // MARK: - Private Methods
    
    private func setupSecurity() {
        // Initialize encryption key
        encryptionKey = generateEncryptionKey()
        
        // Setup security configuration
        securityConfig.enableEncryption = true
        securityConfig.enableAuditing = true
        securityConfig.enableInputValidation = true
    }
    
    private func getEncryptionKey() -> SymmetricKey {
        if let key = encryptionKey {
            return key
        } else {
            let newKey = generateEncryptionKey()
            encryptionKey = newKey
            return newKey
        }
    }
    
    private func generateEncryptionKey() -> SymmetricKey {
        return SymmetricKey(size: .bits256)
    }
    
    private func generateSalt() -> String {
        var bytes = [UInt8](repeating: 0, count: 16)
        let result = SecRandomCopyBytes(kSecRandomDefault, 16, &bytes)
        
        guard result == errSecSuccess else {
            return UUID().uuidString
        }
        
        return Data(bytes).base64EncodedString()
    }
    
    private func containsSQLInjection(_ input: String) -> Bool {
        let sqlPatterns = [
            "'.*or.*'.*=",
            "'.*or.*'.*'.*=",
            "'.*union.*select",
            "'.*drop.*table",
            "'.*delete.*from",
            "'.*insert.*into",
            "'.*update.*set"
        ]
        
        for pattern in sqlPatterns {
            if input.range(of: pattern, options: .regularExpression, range: nil, locale: nil) != nil {
                return true
            }
        }
        
        return false
    }
    
    private func containsXSS(_ input: String) -> Bool {
        let xssPatterns = [
            "<script.*>",
            "javascript:",
            "onload=",
            "onerror=",
            "onclick=",
            "onmouseover="
        ]
        
        for pattern in xssPatterns {
            if input.range(of: pattern, options: .regularExpression, range: nil, locale: nil) != nil {
                return true
            }
        }
        
        return false
    }
}

// MARK: - Authentication Manager

/**
 * Authentication manager
 * 
 * This class demonstrates proper authentication management
 * with comprehensive authentication practices
 */
class AuthenticationManager {
    
    // MARK: - Properties
    
    private var securityManager: SecurityManager
    private var sessionManager: SessionManager
    private var userRepository: UserRepository
    
    // MARK: - Initialization
    
    init(securityManager: SecurityManager, sessionManager: SessionManager, userRepository: UserRepository) {
        self.securityManager = securityManager
        self.sessionManager = sessionManager
        self.userRepository = userRepository
    }
    
    // MARK: - Public Methods
    
    /**
     * Authenticate user
     * 
     * This method demonstrates proper user authentication
     * with comprehensive authentication practices
     */
    func authenticateUser(credentials: UserCredentials) -> AnyPublisher<AuthenticationResult, Error> {
        return Future<AuthenticationResult, Error> { promise in
            // Validate input
            let usernameValidation = self.securityManager.validateInput(credentials.username, type: .username)
            let passwordValidation = self.securityManager.validateInput(credentials.password, type: .password)
            
            if !usernameValidation.isValid || !passwordValidation.isValid {
                promise(.failure(AuthenticationError.invalidCredentials))
                return
            }
            
            // Get user from repository
            self.userRepository.getUser(by: credentials.username)
                .sink(
                    receiveCompletion: { completion in
                        if case .failure(let error) = completion {
                            promise(.failure(error))
                        }
                    },
                    receiveValue: { user in
                        // Verify password
                        do {
                            let isValidPassword = try self.securityManager.verifyPassword(credentials.password, hashedPassword: user.hashedPassword)
                            
                            if isValidPassword {
                                // Create session
                                let session = self.sessionManager.createSession(for: user)
                                let result = AuthenticationResult(
                                    success: true,
                                    user: user,
                                    session: session,
                                    message: "Authentication successful"
                                )
                                promise(.success(result))
                            } else {
                                promise(.failure(AuthenticationError.invalidCredentials))
                            }
                        } catch {
                            promise(.failure(error))
                        }
                    }
                )
                .store(in: &self.cancellables)
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Refresh token
     * 
     * This method demonstrates proper token refresh
     * with comprehensive token management
     */
    func refreshToken(_ refreshToken: String) -> AnyPublisher<AuthenticationResult, Error> {
        return Future<AuthenticationResult, Error> { promise in
            self.sessionManager.refreshSession(refreshToken: refreshToken)
                .sink(
                    receiveCompletion: { completion in
                        if case .failure(let error) = completion {
                            promise(.failure(error))
                        }
                    },
                    receiveValue: { session in
                        let result = AuthenticationResult(
                            success: true,
                            user: session.user,
                            session: session,
                            message: "Token refreshed successfully"
                        )
                        promise(.success(result))
                    }
                )
                .store(in: &self.cancellables)
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Logout user
     * 
     * This method demonstrates proper user logout
     * with comprehensive session management
     */
    func logoutUser(sessionId: String) -> AnyPublisher<LogoutResult, Error> {
        return Future<LogoutResult, Error> { promise in
            self.sessionManager.invalidateSession(sessionId: sessionId)
                .sink(
                    receiveCompletion: { completion in
                        if case .failure(let error) = completion {
                            promise(.failure(error))
                        }
                    },
                    receiveValue: { success in
                        let result = LogoutResult(
                            success: success,
                            message: success ? "Logout successful" : "Logout failed"
                        )
                        promise(.success(result))
                    }
                )
                .store(in: &self.cancellables)
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Properties
    
    private var cancellables = Set<AnyCancellable>()
}

// MARK: - Authorization Manager

/**
 * Authorization manager
 * 
 * This class demonstrates proper authorization management
 * with comprehensive authorization practices
 */
class AuthorizationManager {
    
    // MARK: - Properties
    
    private var roleRepository: RoleRepository
    private var permissionRepository: PermissionRepository
    
    // MARK: - Initialization
    
    init(roleRepository: RoleRepository, permissionRepository: PermissionRepository) {
        self.roleRepository = roleRepository
        self.permissionRepository = permissionRepository
    }
    
    // MARK: - Public Methods
    
    /**
     * Check permission
     * 
     * This method demonstrates proper permission checking
     * with comprehensive authorization practices
     */
    func checkPermission(user: User, resource: String, action: String) -> AnyPublisher<AuthorizationResult, Error> {
        return Future<AuthorizationResult, Error> { promise in
            // Get user roles
            self.roleRepository.getUserRoles(userId: user.id)
                .flatMap { roles in
                    // Get role permissions
                    self.permissionRepository.getRolePermissions(roleIds: roles.map { $0.id })
                }
                .sink(
                    receiveCompletion: { completion in
                        if case .failure(let error) = completion {
                            promise(.failure(error))
                        }
                    },
                    receiveValue: { permissions in
                        // Check if user has required permission
                        let hasPermission = permissions.contains { permission in
                            permission.resource == resource && permission.action == action
                        }
                        
                        let result = AuthorizationResult(
                            authorized: hasPermission,
                            user: user,
                            resource: resource,
                            action: action,
                            message: hasPermission ? "Access granted" : "Access denied"
                        )
                        
                        promise(.success(result))
                    }
                )
                .store(in: &self.cancellables)
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Get user permissions
     * 
     * This method demonstrates proper user permission retrieval
     * with comprehensive permission management
     */
    func getUserPermissions(user: User) -> AnyPublisher<[Permission], Error> {
        return roleRepository.getUserRoles(userId: user.id)
            .flatMap { roles in
                self.permissionRepository.getRolePermissions(roleIds: roles.map { $0.id })
            }
            .eraseToAnyPublisher()
    }
    
    // MARK: - Private Properties
    
    private var cancellables = Set<AnyCancellable>()
}

// MARK: - Vulnerability Scanner

/**
 * Vulnerability scanner
 * 
 * This class demonstrates proper vulnerability scanning
 * with comprehensive security scanning practices
 */
class VulnerabilityScanner {
    
    // MARK: - Properties
    
    private var scanResults: [VulnerabilityScanResult] = []
    private var securityRules: [SecurityRule] = []
    
    // MARK: - Initialization
    
    init() {
        setupSecurityRules()
    }
    
    // MARK: - Public Methods
    
    /**
     * Scan code for vulnerabilities
     * 
     * This method demonstrates proper code vulnerability scanning
     * with comprehensive security analysis
     */
    func scanCode(_ code: String) -> AnyPublisher<VulnerabilityScanResult, Error> {
        return Future<VulnerabilityScanResult, Error> { promise in
            var vulnerabilities: [Vulnerability] = []
            
            for rule in self.securityRules {
                let ruleVulnerabilities = self.checkRule(rule, code: code)
                vulnerabilities.append(contentsOf: ruleVulnerabilities)
            }
            
            let result = VulnerabilityScanResult(
                id: UUID().uuidString,
                code: code,
                vulnerabilities: vulnerabilities,
                timestamp: Date(),
                severity: self.calculateOverallSeverity(vulnerabilities)
            )
            
            self.scanResults.append(result)
            promise(.success(result))
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Get scan history
     * 
     * This method demonstrates proper scan history retrieval
     * with comprehensive scan result management
     */
    func getScanHistory() -> [VulnerabilityScanResult] {
        return scanResults
    }
    
    // MARK: - Private Methods
    
    private func setupSecurityRules() {
        securityRules = [
            SecurityRule(
                name: "SQL Injection",
                pattern: "'.*or.*'.*=",
                severity: .critical,
                description: "Potential SQL injection vulnerability"
            ),
            SecurityRule(
                name: "XSS",
                pattern: "<script.*>",
                severity: .high,
                description: "Potential XSS vulnerability"
            ),
            SecurityRule(
                name: "Hardcoded Password",
                pattern: "password\\s*=\\s*[\"'][^\"']+[\"']",
                severity: .high,
                description: "Hardcoded password detected"
            ),
            SecurityRule(
                name: "Weak Encryption",
                pattern: "MD5|SHA1",
                severity: .medium,
                description: "Weak encryption algorithm detected"
            )
        ]
    }
    
    private func checkRule(_ rule: SecurityRule, code: String) -> [Vulnerability] {
        var vulnerabilities: [Vulnerability] = []
        
        if let range = code.range(of: rule.pattern, options: .regularExpression, range: nil, locale: nil) {
            let vulnerability = Vulnerability(
                rule: rule,
                line: code.lineNumber(for: range),
                column: code.columnNumber(for: range),
                description: rule.description,
                severity: rule.severity
            )
            vulnerabilities.append(vulnerability)
        }
        
        return vulnerabilities
    }
    
    private func calculateOverallSeverity(_ vulnerabilities: [Vulnerability]) -> VulnerabilitySeverity {
        if vulnerabilities.contains(where: { $0.severity == .critical }) {
            return .critical
        } else if vulnerabilities.contains(where: { $0.severity == .high }) {
            return .high
        } else if vulnerabilities.contains(where: { $0.severity == .medium }) {
            return .medium
        } else {
            return .low
        }
    }
}

// MARK: - Supporting Types

/**
 * Security configuration
 * 
 * This struct demonstrates proper security configuration modeling
 * for security management
 */
struct SecurityConfiguration {
    var enableEncryption: Bool = true
    var enableAuditing: Bool = true
    var enableInputValidation: Bool = true
    var encryptionAlgorithm: EncryptionAlgorithm = .AES_GCM
    var keyRotationInterval: TimeInterval = 86400 * 30 // 30 days
}

/**
 * Encryption algorithm
 * 
 * This enum demonstrates proper encryption algorithm modeling
 * for security management
 */
enum EncryptionAlgorithm: String, CaseIterable {
    case AES_GCM = "AES-GCM"
    case AES_CBC = "AES-CBC"
    case ChaCha20_Poly1305 = "ChaCha20-Poly1305"
}

/**
 * Encrypted data
 * 
 * This struct demonstrates proper encrypted data modeling
 * for security management
 */
struct EncryptedData {
    let data: Data
    let algorithm: EncryptionAlgorithm
    let keyId: String
    let timestamp: Date
}

/**
 * Input type
 * 
 * This enum demonstrates proper input type modeling
 * for security management
 */
enum InputType {
    case username
    case password
    case email
    case general
    
    var maxLength: Int {
        switch self {
        case .username: return 50
        case .password: return 128
        case .email: return 254
        case .general: return 1000
        }
    }
    
    var pattern: String {
        switch self {
        case .username: return "^[a-zA-Z0-9_]+$"
        case .password: return "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$"
        case .email: return "^[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$"
        case .general: return ""
        }
    }
}

/**
 * Security violation
 * 
 * This struct demonstrates proper security violation modeling
 * for security management
 */
struct SecurityViolation {
    let type: ViolationType
    let message: String
    let severity: SecuritySeverity
}

/**
 * Violation type
 * 
 * This enum demonstrates proper violation type modeling
 * for security management
 */
enum ViolationType: String, CaseIterable {
    case lengthExceeded = "length_exceeded"
    case invalidPattern = "invalid_pattern"
    case sqlInjection = "sql_injection"
    case xss = "xss"
    case csrf = "csrf"
}

/**
 * Security severity
 * 
 * This enum demonstrates proper security severity modeling
 * for security management
 */
enum SecuritySeverity: String, CaseIterable {
    case low = "low"
    case medium = "medium"
    case high = "high"
    case critical = "critical"
}

/**
 * Validation result
 * 
 * This struct demonstrates proper validation result modeling
 * for security management
 */
struct ValidationResult {
    let isValid: Bool
    let violations: [SecurityViolation]
}

/**
 * User credentials
 * 
 * This struct demonstrates proper user credentials modeling
 * for authentication management
 */
struct UserCredentials {
    let username: String
    let password: String
}

/**
 * User
 * 
 * This struct demonstrates proper user modeling
 * for authentication management
 */
struct User {
    let id: String
    let username: String
    let email: String
    let hashedPassword: String
    let roles: [Role]
    let createdAt: Date
    let isActive: Bool
}

/**
 * Role
 * 
 * This struct demonstrates proper role modeling
 * for authorization management
 */
struct Role {
    let id: String
    let name: String
    let description: String
    let permissions: [Permission]
}

/**
 * Permission
 * 
 * This struct demonstrates proper permission modeling
 * for authorization management
 */
struct Permission {
    let id: String
    let resource: String
    let action: String
    let description: String
}

/**
 * Session
 * 
 * This struct demonstrates proper session modeling
 * for authentication management
 */
struct Session {
    let id: String
    let userId: String
    let accessToken: String
    let refreshToken: String
    let expiresAt: Date
    let createdAt: Date
    let user: User
}

/**
 * Authentication result
 * 
 * This struct demonstrates proper authentication result modeling
 * for authentication management
 */
struct AuthenticationResult {
    let success: Bool
    let user: User?
    let session: Session?
    let message: String
}

/**
 * Logout result
 * 
 * This struct demonstrates proper logout result modeling
 * for authentication management
 */
struct LogoutResult {
    let success: Bool
    let message: String
}

/**
 * Authorization result
 * 
 * This struct demonstrates proper authorization result modeling
 * for authorization management
 */
struct AuthorizationResult {
    let authorized: Bool
    let user: User
    let resource: String
    let action: String
    let message: String
}

/**
 * Security rule
 * 
 * This struct demonstrates proper security rule modeling
 * for vulnerability scanning
 */
struct SecurityRule {
    let name: String
    let pattern: String
    let severity: VulnerabilitySeverity
    let description: String
}

/**
 * Vulnerability
 * 
 * This struct demonstrates proper vulnerability modeling
 * for vulnerability scanning
 */
struct Vulnerability {
    let rule: SecurityRule
    let line: Int
    let column: Int
    let description: String
    let severity: VulnerabilitySeverity
}

/**
 * Vulnerability severity
 * 
 * This enum demonstrates proper vulnerability severity modeling
 * for vulnerability scanning
 */
enum VulnerabilitySeverity: String, CaseIterable {
    case low = "low"
    case medium = "medium"
    case high = "high"
    case critical = "critical"
}

/**
 * Vulnerability scan result
 * 
 * This struct demonstrates proper vulnerability scan result modeling
 * for vulnerability scanning
 */
struct VulnerabilityScanResult {
    let id: String
    let code: String
    let vulnerabilities: [Vulnerability]
    let timestamp: Date
    let severity: VulnerabilitySeverity
}

/**
 * Security auditor
 * 
 * This class demonstrates proper security auditing
 * for security management
 */
class SecurityAuditor {
    // Implementation details
}

/**
 * Session manager
 * 
 * This class demonstrates proper session management
 * for authentication management
 */
class SessionManager {
    func createSession(for user: User) -> Session {
        // Implementation details
        return Session(
            id: UUID().uuidString,
            userId: user.id,
            accessToken: "access_token",
            refreshToken: "refresh_token",
            expiresAt: Date().addingTimeInterval(3600),
            createdAt: Date(),
            user: user
        )
    }
    
    func refreshSession(refreshToken: String) -> AnyPublisher<Session, Error> {
        // Implementation details
        return Just(Session(
            id: UUID().uuidString,
            userId: "user_id",
            accessToken: "new_access_token",
            refreshToken: "new_refresh_token",
            expiresAt: Date().addingTimeInterval(3600),
            createdAt: Date(),
            user: User(
                id: "user_id",
                username: "username",
                email: "email@example.com",
                hashedPassword: "hashed_password",
                roles: [],
                createdAt: Date(),
                isActive: true
            )
        ))
        .setFailureType(to: Error.self)
        .eraseToAnyPublisher()
    }
    
    func invalidateSession(sessionId: String) -> AnyPublisher<Bool, Error> {
        // Implementation details
        return Just(true)
            .setFailureType(to: Error.self)
            .eraseToAnyPublisher()
    }
}

/**
 * User repository
 * 
 * This class demonstrates proper user repository
 * for authentication management
 */
class UserRepository {
    func getUser(by username: String) -> AnyPublisher<User, Error> {
        // Implementation details
        return Just(User(
            id: "user_id",
            username: username,
            email: "email@example.com",
            hashedPassword: "hashed_password",
            roles: [],
            createdAt: Date(),
            isActive: true
        ))
        .setFailureType(to: Error.self)
        .eraseToAnyPublisher()
    }
}

/**
 * Role repository
 * 
 * This class demonstrates proper role repository
 * for authorization management
 */
class RoleRepository {
    func getUserRoles(userId: String) -> AnyPublisher<[Role], Error> {
        // Implementation details
        return Just([])
            .setFailureType(to: Error.self)
            .eraseToAnyPublisher()
    }
}

/**
 * Permission repository
 * 
 * This class demonstrates proper permission repository
 * for authorization management
 */
class PermissionRepository {
    func getRolePermissions(roleIds: [String]) -> AnyPublisher<[Permission], Error> {
        // Implementation details
        return Just([])
            .setFailureType(to: Error.self)
            .eraseToAnyPublisher()
    }
}

/**
 * Authentication error types
 * 
 * This enum demonstrates proper error modeling
 * for authentication management
 */
enum AuthenticationError: Error, LocalizedError {
    case invalidCredentials
    case userNotFound
    case accountLocked
    case tokenExpired
    
    var errorDescription: String? {
        switch self {
        case .invalidCredentials:
            return "Invalid username or password"
        case .userNotFound:
            return "User not found"
        case .accountLocked:
            return "Account is locked"
        case .tokenExpired:
            return "Token has expired"
        }
    }
}

// MARK: - String Extensions

extension String {
    func lineNumber(for range: Range<String.Index>) -> Int {
        let beforeRange = startIndex..<range.lowerBound
        return beforeRange.components(separatedBy: "\n").count
    }
    
    func columnNumber(for range: Range<String.Index>) -> Int {
        let beforeRange = startIndex..<range.lowerBound
        let lastNewline = beforeRange.lastIndex(of: "\n") ?? startIndex
        return distance(from: lastNewline, to: range.lowerBound)
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use security practices
 * 
 * This function shows practical usage of all the security components
 */
func demonstrateSecurity() {
    print("=== Security Demonstration ===\n")
    
    // Security Manager
    let securityManager = SecurityManager()
    print("--- Security Manager ---")
    print("Security Manager: \(type(of: securityManager))")
    print("Features: Data encryption, password hashing, input validation, secure tokens")
    
    // Authentication Manager
    let sessionManager = SessionManager()
    let userRepository = UserRepository()
    let authManager = AuthenticationManager(
        securityManager: securityManager,
        sessionManager: sessionManager,
        userRepository: userRepository
    )
    print("\n--- Authentication Manager ---")
    print("Authentication Manager: \(type(of: authManager))")
    print("Features: User authentication, token management, session management")
    
    // Authorization Manager
    let roleRepository = RoleRepository()
    let permissionRepository = PermissionRepository()
    let authzManager = AuthorizationManager(
        roleRepository: roleRepository,
        permissionRepository: permissionRepository
    )
    print("\n--- Authorization Manager ---")
    print("Authorization Manager: \(type(of: authzManager))")
    print("Features: Permission checking, role management, access control")
    
    // Vulnerability Scanner
    let vulnerabilityScanner = VulnerabilityScanner()
    print("\n--- Vulnerability Scanner ---")
    print("Vulnerability Scanner: \(type(of: vulnerabilityScanner))")
    print("Features: Code scanning, vulnerability detection, security analysis")
    
    // Demonstrate security features
    print("\n--- Security Features ---")
    print("Data Protection: Encryption, hashing, secure storage")
    print("Authentication: User authentication, token management, session management")
    print("Authorization: Permission checking, role management, access control")
    print("Vulnerability Management: Code scanning, security analysis, threat detection")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Implement comprehensive data encryption")
    print("2. Use secure authentication and authorization")
    print("3. Validate all input and sanitize data")
    print("4. Implement proper session management")
    print("5. Scan for vulnerabilities regularly")
    print("6. Follow security coding practices")
    print("7. Monitor and audit security events")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateSecurity()
