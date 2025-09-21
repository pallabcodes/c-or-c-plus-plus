/*
 * Swift Testing: UI Testing
 * 
 * This file demonstrates production-grade UI testing strategies in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master XCUITest framework and automated UI testing
 * - Understand test automation and CI/CD integration
 * - Implement proper accessibility testing and validation
 * - Apply performance testing and responsiveness validation
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import XCTest
import Foundation

// MARK: - UI Test Base Class

/**
 * Base UI test class
 * 
 * This class demonstrates proper UI test base implementation
 * with common setup and teardown patterns
 */
class BaseUITest: XCTestCase {
    
    // MARK: - Properties
    
    var app: XCUIApplication!
    var testData: TestDataManager!
    
    // MARK: - Setup and Teardown
    
    override func setUp() {
        super.setUp()
        
        // Setup app
        app = XCUIApplication()
        app.launchArguments = ["--uitesting"]
        app.launchEnvironment = [
            "UITESTING": "true",
            "MOCK_NETWORK": "true"
        ]
        
        // Setup test data
        testData = TestDataManager()
        
        // Launch app
        app.launch()
        
        // Wait for app to be ready
        waitForAppToBeReady()
    }
    
    override func tearDown() {
        // Cleanup test data
        testData.cleanup()
        
        // Terminate app
        app.terminate()
        
        super.tearDown()
    }
    
    // MARK: - Helper Methods
    
    /**
     * Wait for app to be ready
     * 
     * This method demonstrates proper app readiness waiting
     * with timeout handling
     */
    private func waitForAppToBeReady() {
        let readyElement = app.staticTexts["App Ready"]
        XCTAssertTrue(readyElement.waitForExistence(timeout: 10.0), "App should be ready within 10 seconds")
    }
    
    /**
     * Take screenshot
     * 
     * This method demonstrates proper screenshot capture
     * for test documentation and debugging
     */
    func takeScreenshot(name: String) {
        let screenshot = XCUIScreen.main.screenshot()
        let attachment = XCTAttachment(screenshot: screenshot)
        attachment.name = name
        attachment.lifetime = .keepAlways
        add(attachment)
    }
    
    /**
     * Wait for element to exist
     * 
     * This method demonstrates proper element waiting
     * with timeout handling
     */
    func waitForElement(_ element: XCUIElement, timeout: TimeInterval = 5.0) -> Bool {
        return element.waitForExistence(timeout: timeout)
    }
    
    /**
     * Wait for element to disappear
     * 
     * This method demonstrates proper element disappearance waiting
     * with timeout handling
     */
    func waitForElementToDisappear(_ element: XCUIElement, timeout: TimeInterval = 5.0) -> Bool {
        return element.waitForNonExistence(timeout: timeout)
    }
}

// MARK: - Login Flow UI Tests

/**
 * Login flow UI tests
 * 
 * This class demonstrates proper login flow testing
 * with comprehensive UI validation
 */
class LoginFlowUITests: BaseUITest {
    
    // MARK: - Test Methods
    
    func testSuccessfulLogin() {
        // Given
        let username = testData.validUsername
        let password = testData.validPassword
        
        // When
        performLogin(username: username, password: password)
        
        // Then
        XCTAssertTrue(app.navigationBars["Dashboard"].exists, "Should navigate to dashboard after successful login")
        XCTAssertTrue(app.staticTexts["Welcome, \(username)"].exists, "Should display welcome message with username")
    }
    
    func testFailedLoginWithInvalidCredentials() {
        // Given
        let username = testData.invalidUsername
        let password = testData.invalidPassword
        
        // When
        performLogin(username: username, password: password)
        
        // Then
        XCTAssertTrue(app.alerts["Login Failed"].exists, "Should show login failed alert")
        XCTAssertTrue(app.alerts["Login Failed"].staticTexts["Invalid username or password"].exists, "Should show correct error message")
    }
    
    func testLoginWithEmptyFields() {
        // Given
        let username = ""
        let password = ""
        
        // When
        performLogin(username: username, password: password)
        
        // Then
        XCTAssertTrue(app.alerts["Validation Error"].exists, "Should show validation error alert")
        XCTAssertTrue(app.alerts["Validation Error"].staticTexts["Please enter username and password"].exists, "Should show correct validation message")
    }
    
    func testLoginWithNetworkError() {
        // Given
        testData.simulateNetworkError = true
        let username = testData.validUsername
        let password = testData.validPassword
        
        // When
        performLogin(username: username, password: password)
        
        // Then
        XCTAssertTrue(app.alerts["Network Error"].exists, "Should show network error alert")
        XCTAssertTrue(app.alerts["Network Error"].staticTexts["Unable to connect to server"].exists, "Should show correct network error message")
    }
    
    // MARK: - Helper Methods
    
    private func performLogin(username: String, password: String) {
        // Enter username
        let usernameField = app.textFields["Username"]
        XCTAssertTrue(waitForElement(usernameField), "Username field should exist")
        usernameField.tap()
        usernameField.typeText(username)
        
        // Enter password
        let passwordField = app.secureTextFields["Password"]
        XCTAssertTrue(waitForElement(passwordField), "Password field should exist")
        passwordField.tap()
        passwordField.typeText(password)
        
        // Tap login button
        let loginButton = app.buttons["Login"]
        XCTAssertTrue(waitForElement(loginButton), "Login button should exist")
        loginButton.tap()
        
        // Wait for login to complete
        let loadingIndicator = app.activityIndicators["Loading"]
        if loadingIndicator.exists {
            XCTAssertTrue(waitForElementToDisappear(loadingIndicator, timeout: 10.0), "Loading indicator should disappear")
        }
    }
}

// MARK: - Navigation Flow UI Tests

/**
 * Navigation flow UI tests
 * 
 * This class demonstrates proper navigation flow testing
 * with comprehensive UI validation
 */
class NavigationFlowUITests: BaseUITest {
    
    // MARK: - Test Methods
    
    func testTabBarNavigation() {
        // Given
        performLogin()
        
        // When
        let tabBar = app.tabBars.firstMatch
        XCTAssertTrue(waitForElement(tabBar), "Tab bar should exist")
        
        // Test Home tab
        let homeTab = tabBar.buttons["Home"]
        homeTab.tap()
        XCTAssertTrue(app.staticTexts["Home Content"].exists, "Should show home content")
        
        // Test Profile tab
        let profileTab = tabBar.buttons["Profile"]
        profileTab.tap()
        XCTAssertTrue(app.staticTexts["Profile Content"].exists, "Should show profile content")
        
        // Test Settings tab
        let settingsTab = tabBar.buttons["Settings"]
        settingsTab.tap()
        XCTAssertTrue(app.staticTexts["Settings Content"].exists, "Should show settings content")
    }
    
    func testNavigationStack() {
        // Given
        performLogin()
        
        // When
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // Navigate to detail view
        let detailButton = app.buttons["View Details"]
        XCTAssertTrue(waitForElement(detailButton), "Detail button should exist")
        detailButton.tap()
        
        // Then
        XCTAssertTrue(app.navigationBars["Detail View"].exists, "Should navigate to detail view")
        XCTAssertTrue(app.buttons["Back"].exists, "Should have back button")
        
        // Test back navigation
        app.buttons["Back"].tap()
        XCTAssertTrue(app.navigationBars["Home"].exists, "Should navigate back to home")
    }
    
    func testModalPresentation() {
        // Given
        performLogin()
        
        // When
        let modalButton = app.buttons["Show Modal"]
        XCTAssertTrue(waitForElement(modalButton), "Modal button should exist")
        modalButton.tap()
        
        // Then
        XCTAssertTrue(app.navigationBars["Modal View"].exists, "Should present modal view")
        XCTAssertTrue(app.buttons["Close"].exists, "Should have close button")
        
        // Test modal dismissal
        app.buttons["Close"].tap()
        XCTAssertTrue(waitForElementToDisappear(app.navigationBars["Modal View"]), "Modal should be dismissed")
    }
    
    // MARK: - Helper Methods
    
    private func performLogin() {
        let usernameField = app.textFields["Username"]
        let passwordField = app.secureTextFields["Password"]
        let loginButton = app.buttons["Login"]
        
        usernameField.tap()
        usernameField.typeText(testData.validUsername)
        
        passwordField.tap()
        passwordField.typeText(testData.validPassword)
        
        loginButton.tap()
        
        let loadingIndicator = app.activityIndicators["Loading"]
        if loadingIndicator.exists {
            waitForElementToDisappear(loadingIndicator, timeout: 10.0)
        }
    }
}

// MARK: - Accessibility UI Tests

/**
 * Accessibility UI tests
 * 
 * This class demonstrates proper accessibility testing
 * with VoiceOver and accessibility validation
 */
class AccessibilityUITests: BaseUITest {
    
    // MARK: - Test Methods
    
    func testVoiceOverNavigation() {
        // Given
        performLogin()
        
        // When
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // Then
        XCTAssertTrue(homeTab.isAccessibilityElement, "Home tab should be accessible")
        XCTAssertEqual(homeTab.label, "Home", "Home tab should have correct accessibility label")
        XCTAssertEqual(homeTab.hint, "Double tap to view home content", "Home tab should have correct accessibility hint")
    }
    
    func testAccessibilityLabels() {
        // Given
        performLogin()
        
        // When
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // Then
        let titleLabel = app.staticTexts["Home Content"]
        XCTAssertTrue(titleLabel.isAccessibilityElement, "Title label should be accessible")
        XCTAssertEqual(titleLabel.label, "Home Content", "Title label should have correct accessibility label")
        
        let detailButton = app.buttons["View Details"]
        XCTAssertTrue(detailButton.isAccessibilityElement, "Detail button should be accessible")
        XCTAssertEqual(detailButton.label, "View Details", "Detail button should have correct accessibility label")
        XCTAssertEqual(detailButton.hint, "Double tap to view detailed information", "Detail button should have correct accessibility hint")
    }
    
    func testAccessibilityTraits() {
        // Given
        performLogin()
        
        // When
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // Then
        let detailButton = app.buttons["View Details"]
        XCTAssertTrue(detailButton.hasTraits(.button), "Detail button should have button trait")
        XCTAssertTrue(detailButton.isEnabled, "Detail button should be enabled")
        
        let disabledButton = app.buttons["Disabled Button"]
        if disabledButton.exists {
            XCTAssertFalse(disabledButton.isEnabled, "Disabled button should not be enabled")
        }
    }
    
    func testAccessibilityElements() {
        // Given
        performLogin()
        
        // When
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // Then
        let scrollView = app.scrollViews.firstMatch
        if scrollView.exists {
            let accessibleElements = scrollView.accessibilityElements
            XCTAssertGreaterThan(accessibleElements.count, 0, "Scroll view should have accessible elements")
        }
    }
}

// MARK: - Performance UI Tests

/**
 * Performance UI tests
 * 
 * This class demonstrates proper performance testing
 * with UI responsiveness and performance validation
 */
class PerformanceUITests: BaseUITest {
    
    // MARK: - Test Methods
    
    func testScrollPerformance() {
        // Given
        performLogin()
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // When
        let scrollView = app.scrollViews.firstMatch
        XCTAssertTrue(waitForElement(scrollView), "Scroll view should exist")
        
        // Measure scroll performance
        let startTime = Date()
        scrollView.swipeUp()
        scrollView.swipeDown()
        let endTime = Date()
        
        // Then
        let scrollDuration = endTime.timeIntervalSince(startTime)
        XCTAssertLessThan(scrollDuration, 1.0, "Scroll should complete within 1 second")
    }
    
    func testAnimationPerformance() {
        // Given
        performLogin()
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // When
        let animateButton = app.buttons["Animate"]
        XCTAssertTrue(waitForElement(animateButton), "Animate button should exist")
        
        let startTime = Date()
        animateButton.tap()
        
        // Wait for animation to complete
        let animatedElement = app.staticTexts["Animated Content"]
        XCTAssertTrue(waitForElement(animatedElement, timeout: 5.0), "Animated element should appear")
        
        let endTime = Date()
        
        // Then
        let animationDuration = endTime.timeIntervalSince(startTime)
        XCTAssertLessThan(animationDuration, 2.0, "Animation should complete within 2 seconds")
    }
    
    func testMemoryUsage() {
        // Given
        performLogin()
        
        // When
        let homeTab = app.tabBars.buttons["Home"]
        homeTab.tap()
        
        // Navigate through multiple views
        for _ in 0..<5 {
            let detailButton = app.buttons["View Details"]
            if detailButton.exists {
                detailButton.tap()
                app.buttons["Back"].tap()
            }
        }
        
        // Then
        // In a real test, you would measure memory usage here
        // For demonstration, we'll just verify the app is still responsive
        XCTAssertTrue(app.isRunning, "App should still be running")
        XCTAssertTrue(homeTab.exists, "Home tab should still be accessible")
    }
}

// MARK: - Test Data Manager

/**
 * Test data manager
 * 
 * This class demonstrates proper test data management
 * for UI testing
 */
class TestDataManager {
    
    // MARK: - Properties
    
    let validUsername = "testuser"
    let validPassword = "testpassword"
    let invalidUsername = "invaliduser"
    let invalidPassword = "invalidpassword"
    
    var simulateNetworkError = false
    
    // MARK: - Public Methods
    
    func cleanup() {
        simulateNetworkError = false
        // Additional cleanup if needed
    }
}

// MARK: - UI Test Utilities

/**
 * UI test utilities
 * 
 * This class demonstrates proper UI test utility functions
 * for common testing patterns
 */
class UITestUtilities {
    
    // MARK: - Static Methods
    
    /**
     * Wait for app to be ready
     * 
     * This method demonstrates proper app readiness waiting
     * with timeout handling
     */
    static func waitForAppToBeReady(_ app: XCUIApplication, timeout: TimeInterval = 10.0) -> Bool {
        let readyElement = app.staticTexts["App Ready"]
        return readyElement.waitForExistence(timeout: timeout)
    }
    
    /**
     * Take screenshot with timestamp
     * 
     * This method demonstrates proper screenshot capture
     * with timestamp naming
     */
    static func takeScreenshot(_ testCase: XCTestCase, name: String) {
        let screenshot = XCUIScreen.main.screenshot()
        let attachment = XCTAttachment(screenshot: screenshot)
        attachment.name = "\(name)_\(Date().timeIntervalSince1970)"
        attachment.lifetime = .keepAlways
        testCase.add(attachment)
    }
    
    /**
     * Wait for element with custom timeout
     * 
     * This method demonstrates proper element waiting
     * with custom timeout handling
     */
    static func waitForElement(_ element: XCUIElement, timeout: TimeInterval = 5.0) -> Bool {
        return element.waitForExistence(timeout: timeout)
    }
    
    /**
     * Wait for element to disappear with custom timeout
     * 
     * This method demonstrates proper element disappearance waiting
     * with custom timeout handling
     */
    static func waitForElementToDisappear(_ element: XCUIElement, timeout: TimeInterval = 5.0) -> Bool {
        return element.waitForNonExistence(timeout: timeout)
    }
    
    /**
     * Tap element with retry
     * 
     * This method demonstrates proper element tapping
     * with retry mechanism
     */
    static func tapElementWithRetry(_ element: XCUIElement, maxRetries: Int = 3) -> Bool {
        for _ in 0..<maxRetries {
            if element.exists && element.isHittable {
                element.tap()
                return true
            }
            Thread.sleep(forTimeInterval: 0.5)
        }
        return false
    }
    
    /**
     * Type text with retry
     * 
     * This method demonstrates proper text input
     * with retry mechanism
     */
    static func typeTextWithRetry(_ element: XCUIElement, text: String, maxRetries: Int = 3) -> Bool {
        for _ in 0..<maxRetries {
            if element.exists && element.isHittable {
                element.tap()
                element.typeText(text)
                return true
            }
            Thread.sleep(forTimeInterval: 0.5)
        }
        return false
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use UI testing strategies
 * 
 * This function shows practical usage of all the UI testing components
 */
func demonstrateUITesting() {
    print("=== UI Testing Demonstration ===\n")
    
    // Base UI Test
    print("--- Base UI Test ---")
    print("BaseUITest: Common setup and teardown patterns")
    print("Features: App launch, test data management, helper methods")
    
    // Login Flow UI Tests
    print("\n--- Login Flow UI Tests ---")
    print("LoginFlowUITests: Comprehensive login flow testing")
    print("Features: Successful login, failed login, validation testing")
    
    // Navigation Flow UI Tests
    print("\n--- Navigation Flow UI Tests ---")
    print("NavigationFlowUITests: Navigation and flow testing")
    print("Features: Tab bar navigation, navigation stack, modal presentation")
    
    // Accessibility UI Tests
    print("\n--- Accessibility UI Tests ---")
    print("AccessibilityUITests: VoiceOver and accessibility testing")
    print("Features: Accessibility labels, traits, elements, navigation")
    
    // Performance UI Tests
    print("\n--- Performance UI Tests ---")
    print("PerformanceUITests: UI performance and responsiveness testing")
    print("Features: Scroll performance, animation performance, memory usage")
    
    // Test Data Manager
    print("\n--- Test Data Manager ---")
    print("TestDataManager: Test data management and cleanup")
    print("Features: Test data creation, cleanup, network simulation")
    
    // UI Test Utilities
    print("\n--- UI Test Utilities ---")
    print("UITestUtilities: Common UI test utility functions")
    print("Features: Element waiting, screenshot capture, retry mechanisms")
    
    // Demonstrate testing techniques
    print("\n--- Testing Techniques ---")
    print("UI Testing: Automated UI interaction and validation")
    print("Accessibility Testing: VoiceOver and accessibility validation")
    print("Performance Testing: UI responsiveness and performance validation")
    print("Test Automation: CI/CD integration and automation")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Use descriptive test names and organization")
    print("2. Test real user flows and interactions")
    print("3. Validate accessibility and usability")
    print("4. Test performance and responsiveness")
    print("5. Use appropriate test data and cleanup")
    print("6. Take screenshots for documentation")
    print("7. Integrate with CI/CD pipeline")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateUITesting()
