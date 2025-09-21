/*
 * Swift Examples: Apple-Style UI Patterns
 * 
 * This file demonstrates Apple's design system and UI patterns
 * used in production iOS applications, based on Apple's own implementations.
 * 
 * Key Learning Objectives:
 * - Master Apple's design system and UI patterns
 * - Understand Apple's accessibility implementation
 * - Learn Apple's performance optimization techniques
 * - Apply Apple's user experience principles
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple Production Code Quality
 */

import SwiftUI
import UIKit

// MARK: - Apple Design System

/**
 * Apple's design system implementation
 * 
 * This class demonstrates Apple's design system patterns
 * used in production iOS applications
 */
class AppleDesignSystem {
    
    // MARK: - Color System
    
    /**
     * Apple's color system
     * 
     * This struct demonstrates Apple's color system implementation
     * with semantic color naming and accessibility support
     */
    struct ColorSystem {
        // Primary Colors
        static let primary = Color(red: 0.0, green: 0.48, blue: 1.0)
        static let primaryVariant = Color(red: 0.0, green: 0.38, blue: 0.8)
        static let onPrimary = Color.white
        
        // Secondary Colors
        static let secondary = Color(red: 0.55, green: 0.55, blue: 0.57)
        static let secondaryVariant = Color(red: 0.45, green: 0.45, blue: 0.47)
        static let onSecondary = Color.white
        
        // Background Colors
        static let background = Color(UIColor.systemBackground)
        static let surface = Color(UIColor.secondarySystemBackground)
        static let onBackground = Color(UIColor.label)
        static let onSurface = Color(UIColor.secondaryLabel)
        
        // Error Colors
        static let error = Color(red: 1.0, green: 0.23, blue: 0.19)
        static let onError = Color.white
        
        // Success Colors
        static let success = Color(red: 0.2, green: 0.78, blue: 0.35)
        static let onSuccess = Color.white
        
        // Warning Colors
        static let warning = Color(red: 1.0, green: 0.58, blue: 0.0)
        static let onWarning = Color.white
        
        // Info Colors
        static let info = Color(red: 0.0, green: 0.48, blue: 1.0)
        static let onInfo = Color.white
        
        // Semantic Colors
        static let separator = Color(UIColor.separator)
        static let groupedBackground = Color(UIColor.systemGroupedBackground)
        static let groupedSecondaryBackground = Color(UIColor.secondarySystemGroupedBackground)
    }
    
    // MARK: - Typography System
    
    /**
     * Apple's typography system
     * 
     * This struct demonstrates Apple's typography system implementation
     * with comprehensive font scaling and accessibility support
     */
    struct TypographySystem {
        // Large Title
        static let largeTitle = Font.largeTitle.weight(.bold)
        static let largeTitleEmphasized = Font.largeTitle.weight(.black)
        
        // Title
        static let title1 = Font.title.weight(.bold)
        static let title2 = Font.title2.weight(.bold)
        static let title3 = Font.title3.weight(.semibold)
        
        // Headline
        static let headline = Font.headline.weight(.semibold)
        
        // Body
        static let body = Font.body
        static let bodyEmphasized = Font.body.weight(.semibold)
        
        // Callout
        static let callout = Font.callout
        static let calloutEmphasized = Font.callout.weight(.semibold)
        
        // Subheadline
        static let subheadline = Font.subheadline
        static let subheadlineEmphasized = Font.subheadline.weight(.semibold)
        
        // Footnote
        static let footnote = Font.footnote
        static let footnoteEmphasized = Font.footnote.weight(.semibold)
        
        // Caption
        static let caption1 = Font.caption
        static let caption2 = Font.caption2
    }
    
    // MARK: - Spacing System
    
    /**
     * Apple's spacing system
     * 
     * This struct demonstrates Apple's spacing system implementation
     * with consistent spacing values and responsive design
     */
    struct SpacingSystem {
        // Micro Spacing
        static let micro: CGFloat = 2
        static let tiny: CGFloat = 4
        static let small: CGFloat = 8
        static let medium: CGFloat = 16
        static let large: CGFloat = 24
        static let extraLarge: CGFloat = 32
        static let huge: CGFloat = 48
        static let massive: CGFloat = 64
        
        // Responsive Spacing
        static func responsive(_ base: CGFloat) -> CGFloat {
            let screenWidth = UIScreen.main.bounds.width
            let scaleFactor = screenWidth / 375.0 // iPhone 12 base width
            return base * scaleFactor
        }
    }
    
    // MARK: - Corner Radius System
    
    /**
     * Apple's corner radius system
     * 
     * This struct demonstrates Apple's corner radius system implementation
     * with consistent corner radius values
     */
    struct CornerRadiusSystem {
        static let small: CGFloat = 4
        static let medium: CGFloat = 8
        static let large: CGFloat = 12
        static let extraLarge: CGFloat = 16
        static let round: CGFloat = 999
    }
    
    // MARK: - Shadow System
    
    /**
     * Apple's shadow system
     * 
     * This struct demonstrates Apple's shadow system implementation
     * with consistent shadow values and depth
     */
    struct ShadowSystem {
        static let small = Shadow(
            color: Color.black.opacity(0.1),
            radius: 2,
            x: 0,
            y: 1
        )
        
        static let medium = Shadow(
            color: Color.black.opacity(0.15),
            radius: 4,
            x: 0,
            y: 2
        )
        
        static let large = Shadow(
            color: Color.black.opacity(0.2),
            radius: 8,
            x: 0,
            y: 4
        )
        
        static let extraLarge = Shadow(
            color: Color.black.opacity(0.25),
            radius: 16,
            x: 0,
            y: 8
        )
    }
}

// MARK: - Apple UI Components

/**
 * Apple-style button component
 * 
 * This struct demonstrates Apple's button implementation
 * with comprehensive styling and accessibility support
 */
struct AppleButton: View {
    
    // MARK: - Properties
    
    let title: String
    let style: ButtonStyle
    let size: ButtonSize
    let action: () -> Void
    
    @State private var isPressed = false
    
    // MARK: - Button Style
    
    enum ButtonStyle {
        case primary
        case secondary
        case tertiary
        case destructive
        case plain
    }
    
    // MARK: - Button Size
    
    enum ButtonSize {
        case small
        case medium
        case large
    }
    
    // MARK: - Body
    
    var body: some View {
        Button(action: action) {
            Text(title)
                .font(buttonFont)
                .foregroundColor(buttonForegroundColor)
                .padding(.horizontal, horizontalPadding)
                .padding(.vertical, verticalPadding)
                .background(buttonBackground)
                .cornerRadius(AppleDesignSystem.CornerRadiusSystem.medium)
                .overlay(
                    RoundedRectangle(cornerRadius: AppleDesignSystem.CornerRadiusSystem.medium)
                        .stroke(buttonBorderColor, lineWidth: buttonBorderWidth)
                )
                .scaleEffect(isPressed ? 0.95 : 1.0)
                .animation(.easeInOut(duration: 0.1), value: isPressed)
        }
        .buttonStyle(PlainButtonStyle())
        .onLongPressGesture(minimumDuration: 0, maximumDistance: .infinity, pressing: { pressing in
            isPressed = pressing
        }, perform: {})
        .accessibilityLabel(title)
        .accessibilityHint(accessibilityHint)
    }
    
    // MARK: - Computed Properties
    
    private var buttonFont: Font {
        switch size {
        case .small:
            return AppleDesignSystem.TypographySystem.callout
        case .medium:
            return AppleDesignSystem.TypographySystem.body
        case .large:
            return AppleDesignSystem.TypographySystem.headline
        }
    }
    
    private var horizontalPadding: CGFloat {
        switch size {
        case .small:
            return AppleDesignSystem.SpacingSystem.medium
        case .medium:
            return AppleDesignSystem.SpacingSystem.large
        case .large:
            return AppleDesignSystem.SpacingSystem.extraLarge
        }
    }
    
    private var verticalPadding: CGFloat {
        switch size {
        case .small:
            return AppleDesignSystem.SpacingSystem.small
        case .medium:
            return AppleDesignSystem.SpacingSystem.medium
        case .large:
            return AppleDesignSystem.SpacingSystem.large
        }
    }
    
    private var buttonBackground: some View {
        Group {
            switch style {
            case .primary:
                AppleDesignSystem.ColorSystem.primary
            case .secondary:
                AppleDesignSystem.ColorSystem.secondary
            case .tertiary:
                Color.clear
            case .destructive:
                AppleDesignSystem.ColorSystem.error
            case .plain:
                Color.clear
            }
        }
    }
    
    private var buttonForegroundColor: Color {
        switch style {
        case .primary:
            return AppleDesignSystem.ColorSystem.onPrimary
        case .secondary:
            return AppleDesignSystem.ColorSystem.onSecondary
        case .tertiary:
            return AppleDesignSystem.ColorSystem.primary
        case .destructive:
            return AppleDesignSystem.ColorSystem.onError
        case .plain:
            return AppleDesignSystem.ColorSystem.primary
        }
    }
    
    private var buttonBorderColor: Color {
        switch style {
        case .primary, .secondary, .destructive:
            return Color.clear
        case .tertiary, .plain:
            return AppleDesignSystem.ColorSystem.primary
        }
    }
    
    private var buttonBorderWidth: CGFloat {
        switch style {
        case .primary, .secondary, .destructive:
            return 0
        case .tertiary, .plain:
            return 1
        }
    }
    
    private var accessibilityHint: String {
        switch style {
        case .primary:
            return "Primary action button"
        case .secondary:
            return "Secondary action button"
        case .tertiary:
            return "Tertiary action button"
        case .destructive:
            return "Destructive action button"
        case .plain:
            return "Plain action button"
        }
    }
}

/**
 * Apple-style card component
 * 
 * This struct demonstrates Apple's card implementation
 * with comprehensive styling and accessibility support
 */
struct AppleCard: View {
    
    // MARK: - Properties
    
    let content: AnyView
    let style: CardStyle
    let padding: CardPadding
    
    // MARK: - Card Style
    
    enum CardStyle {
        case elevated
        case filled
        case outlined
        case plain
    }
    
    // MARK: - Card Padding
    
    enum CardPadding {
        case none
        case small
        case medium
        case large
    }
    
    // MARK: - Body
    
    var body: some View {
        content
            .padding(cardPadding)
            .background(cardBackground)
            .cornerRadius(AppleDesignSystem.CornerRadiusSystem.large)
            .overlay(
                RoundedRectangle(cornerRadius: AppleDesignSystem.CornerRadiusSystem.large)
                    .stroke(cardBorderColor, lineWidth: cardBorderWidth)
            )
            .shadow(
                color: cardShadowColor,
                radius: cardShadowRadius,
                x: cardShadowX,
                y: cardShadowY
            )
    }
    
    // MARK: - Computed Properties
    
    private var cardPadding: CGFloat {
        switch padding {
        case .none:
            return 0
        case .small:
            return AppleDesignSystem.SpacingSystem.small
        case .medium:
            return AppleDesignSystem.SpacingSystem.medium
        case .large:
            return AppleDesignSystem.SpacingSystem.large
        }
    }
    
    private var cardBackground: some View {
        Group {
            switch style {
            case .elevated:
                AppleDesignSystem.ColorSystem.surface
            case .filled:
                AppleDesignSystem.ColorSystem.surface
            case .outlined:
                Color.clear
            case .plain:
                Color.clear
            }
        }
    }
    
    private var cardBorderColor: Color {
        switch style {
        case .elevated, .filled:
            return Color.clear
        case .outlined:
            return AppleDesignSystem.ColorSystem.separator
        case .plain:
            return Color.clear
        }
    }
    
    private var cardBorderWidth: CGFloat {
        switch style {
        case .elevated, .filled, .plain:
            return 0
        case .outlined:
            return 1
        }
    }
    
    private var cardShadowColor: Color {
        switch style {
        case .elevated:
            return AppleDesignSystem.ShadowSystem.medium.color
        case .filled, .outlined, .plain:
            return Color.clear
        }
    }
    
    private var cardShadowRadius: CGFloat {
        switch style {
        case .elevated:
            return AppleDesignSystem.ShadowSystem.medium.radius
        case .filled, .outlined, .plain:
            return 0
        }
    }
    
    private var cardShadowX: CGFloat {
        switch style {
        case .elevated:
            return AppleDesignSystem.ShadowSystem.medium.x
        case .filled, .outlined, .plain:
            return 0
        }
    }
    
    private var cardShadowY: CGFloat {
        switch style {
        case .elevated:
            return AppleDesignSystem.ShadowSystem.medium.y
        case .filled, .outlined, .plain:
            return 0
        }
    }
}

// MARK: - Apple Navigation Patterns

/**
 * Apple-style navigation coordinator
 * 
 * This class demonstrates Apple's navigation patterns
 * with comprehensive navigation management
 */
class AppleNavigationCoordinator: ObservableObject {
    
    // MARK: - Properties
    
    @Published var navigationPath = NavigationPath()
    @Published var presentedSheet: SheetType?
    @Published var presentedFullScreenCover: FullScreenCoverType?
    @Published var presentedAlert: AlertType?
    
    // MARK: - Navigation Types
    
    enum SheetType: Identifiable {
        case settings
        case profile
        case search
        case custom(String)
        
        var id: String {
            switch self {
            case .settings: return "settings"
            case .profile: return "profile"
            case .search: return "search"
            case .custom(let id): return id
            }
        }
    }
    
    enum FullScreenCoverType: Identifiable {
        case onboarding
        case login
        case custom(String)
        
        var id: String {
            switch self {
            case .onboarding: return "onboarding"
            case .login: return "login"
            case .custom(let id): return id
            }
        }
    }
    
    enum AlertType: Identifiable {
        case confirmation(String, String)
        case error(String)
        case success(String)
        case custom(String, String)
        
        var id: String {
            switch self {
            case .confirmation(let title, _): return "confirmation_\(title)"
            case .error(let message): return "error_\(message)"
            case .success(let message): return "success_\(message)"
            case .custom(let title, _): return "custom_\(title)"
            }
        }
    }
    
    // MARK: - Navigation Methods
    
    /**
     * Navigate to destination
     * 
     * This method demonstrates Apple's navigation patterns
     * with comprehensive navigation management
     */
    func navigate(to destination: NavigationDestination) {
        navigationPath.append(destination)
    }
    
    /**
     * Present sheet
     * 
     * This method demonstrates Apple's sheet presentation patterns
     * with comprehensive sheet management
     */
    func presentSheet(_ sheet: SheetType) {
        presentedSheet = sheet
    }
    
    /**
     * Present full screen cover
     * 
     * This method demonstrates Apple's full screen cover presentation patterns
     * with comprehensive cover management
     */
    func presentFullScreenCover(_ cover: FullScreenCoverType) {
        presentedFullScreenCover = cover
    }
    
    /**
     * Present alert
     * 
     * This method demonstrates Apple's alert presentation patterns
     * with comprehensive alert management
     */
    func presentAlert(_ alert: AlertType) {
        presentedAlert = alert
    }
    
    /**
     * Dismiss current presentation
     * 
     * This method demonstrates Apple's dismissal patterns
     * with comprehensive dismissal management
     */
    func dismiss() {
        if presentedSheet != nil {
            presentedSheet = nil
        } else if presentedFullScreenCover != nil {
            presentedFullScreenCover = nil
        } else if presentedAlert != nil {
            presentedAlert = nil
        } else {
            navigationPath.removeLast()
        }
    }
    
    /**
     * Pop to root
     * 
     * This method demonstrates Apple's root navigation patterns
     * with comprehensive root navigation management
     */
    func popToRoot() {
        navigationPath = NavigationPath()
    }
}

// MARK: - Apple Accessibility Patterns

/**
 * Apple-style accessibility manager
 * 
 * This class demonstrates Apple's accessibility implementation
 * with comprehensive accessibility support
 */
class AppleAccessibilityManager: ObservableObject {
    
    // MARK: - Properties
    
    @Published var isVoiceOverEnabled = false
    @Published var isSwitchControlEnabled = false
    @Published var isAssistiveTouchEnabled = false
    @Published var isReduceMotionEnabled = false
    @Published var isReduceTransparencyEnabled = false
    @Published var isIncreaseContrastEnabled = false
    @Published var isBoldTextEnabled = false
    @Published var isLargerTextEnabled = false
    
    // MARK: - Initialization
    
    init() {
        setupAccessibilityObservers()
    }
    
    // MARK: - Public Methods
    
    /**
     * Check accessibility feature
     * 
     * This method demonstrates Apple's accessibility feature checking
     * with comprehensive accessibility support
     */
    func isAccessibilityFeatureEnabled(_ feature: AccessibilityFeature) -> Bool {
        switch feature {
        case .voiceOver:
            return isVoiceOverEnabled
        case .switchControl:
            return isSwitchControlEnabled
        case .assistiveTouch:
            return isAssistiveTouchEnabled
        case .reduceMotion:
            return isReduceMotionEnabled
        case .reduceTransparency:
            return isReduceTransparencyEnabled
        case .increaseContrast:
            return isIncreaseContrastEnabled
        case .boldText:
            return isBoldTextEnabled
        case .largerText:
            return isLargerTextEnabled
        }
    }
    
    /**
     * Get accessibility hint
     * 
     * This method demonstrates Apple's accessibility hint generation
     * with comprehensive accessibility support
     */
    func getAccessibilityHint(for element: AccessibilityElement) -> String {
        switch element {
        case .button(let title):
            return "Button: \(title). Double tap to activate."
        case .image(let description):
            return "Image: \(description)."
        case .text(let content):
            return "Text: \(content)."
        case .link(let title, let url):
            return "Link: \(title). Opens \(url)."
        case .textField(let placeholder):
            return "Text field: \(placeholder). Enter text."
        case .slider(let value, let range):
            return "Slider: \(value) of \(range). Adjust value."
        case .switch(let isOn):
            return "Switch: \(isOn ? "On" : "Off"). Toggle to change."
        case .tab(let title):
            return "Tab: \(title). Switch to this tab."
        case .cell(let title):
            return "Cell: \(title). Select to view details."
        }
    }
    
    // MARK: - Private Methods
    
    private func setupAccessibilityObservers() {
        // Observe accessibility feature changes
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.voiceOverStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isVoiceOverEnabled = UIAccessibility.isVoiceOverRunning
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.switchControlStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isSwitchControlEnabled = UIAccessibility.isSwitchControlRunning
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.assistiveTouchStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isAssistiveTouchEnabled = UIAccessibility.isAssistiveTouchRunning
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.reduceMotionStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isReduceMotionEnabled = UIAccessibility.isReduceMotionEnabled
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.reduceTransparencyStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isReduceTransparencyEnabled = UIAccessibility.isReduceTransparencyEnabled
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.increaseContrastStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isIncreaseContrastEnabled = UIAccessibility.isIncreaseContrastEnabled
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.boldTextStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isBoldTextEnabled = UIAccessibility.isBoldTextEnabled
        }
        
        NotificationCenter.default.addObserver(
            forName: UIContentSizeCategory.didChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.isLargerTextEnabled = UIAccessibility.isLargerTextEnabled
        }
    }
}

// MARK: - Supporting Types

/**
 * Navigation destination
 * 
 * This enum demonstrates proper navigation destination modeling
 * for Apple-style navigation patterns
 */
enum NavigationDestination: Hashable {
    case home
    case profile
    case settings
    case detail(String)
    case custom(String)
}

/**
 * Shadow
 * 
 * This struct demonstrates proper shadow modeling
 * for Apple's design system
 */
struct Shadow {
    let color: Color
    let radius: CGFloat
    let x: CGFloat
    let y: CGFloat
}

/**
 * Accessibility feature
 * 
 * This enum demonstrates proper accessibility feature modeling
 * for Apple's accessibility system
 */
enum AccessibilityFeature {
    case voiceOver
    case switchControl
    case assistiveTouch
    case reduceMotion
    case reduceTransparency
    case increaseContrast
    case boldText
    case largerText
}

/**
 * Accessibility element
 * 
 * This enum demonstrates proper accessibility element modeling
 * for Apple's accessibility system
 */
enum AccessibilityElement {
    case button(String)
    case image(String)
    case text(String)
    case link(String, String)
    case textField(String)
    case slider(Double, ClosedRange<Double>)
    case switch(Bool)
    case tab(String)
    case cell(String)
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use Apple-style UI patterns
 * 
 * This function shows practical usage of all the Apple UI components
 */
func demonstrateAppleUIPatterns() {
    print("=== Apple UI Patterns Demonstration ===\n")
    
    // Design System
    print("--- Design System ---")
    print("Color System: Semantic colors, accessibility support")
    print("Typography System: Comprehensive font scaling")
    print("Spacing System: Consistent spacing values")
    print("Corner Radius System: Consistent corner radius values")
    print("Shadow System: Consistent shadow values")
    
    // UI Components
    print("\n--- UI Components ---")
    print("Apple Button: Multiple styles, sizes, accessibility support")
    print("Apple Card: Multiple styles, padding options, shadow support")
    print("Apple Navigation: Comprehensive navigation management")
    print("Apple Accessibility: Full accessibility feature support")
    
    // Best Practices
    print("\n--- Best Practices ---")
    print("1. Use semantic color naming for better maintainability")
    print("2. Implement comprehensive accessibility support")
    print("3. Follow Apple's Human Interface Guidelines")
    print("4. Use consistent spacing and typography")
    print("5. Implement proper touch feedback and animations")
    print("6. Support all accessibility features")
    print("7. Test with VoiceOver and other assistive technologies")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateAppleUIPatterns()
