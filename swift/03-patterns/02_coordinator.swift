/*
 * Design Patterns: Coordinator Pattern
 * 
 * This file demonstrates production-grade Coordinator pattern implementation in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master Coordinator pattern for navigation management
 * - Understand flow coordination and deep linking
 * - Implement proper separation of navigation logic
 * - Apply state management and persistence patterns
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation
import UIKit

// MARK: - Coordinator Protocol

/**
 * Base coordinator protocol defining common coordinator behavior
 * 
 * This protocol demonstrates the core Coordinator pattern interface
 * with proper lifecycle management and navigation coordination
 */
protocol Coordinator: AnyObject {
    var childCoordinators: [Coordinator] { get set }
    var navigationController: UINavigationController { get }
    var parentCoordinator: Coordinator? { get set }
    
    func start()
    func finish()
    func addChildCoordinator(_ coordinator: Coordinator)
    func removeChildCoordinator(_ coordinator: Coordinator)
    func removeAllChildCoordinators()
}

/**
 * Default implementation of Coordinator protocol
 * 
 * This extension provides common coordinator functionality
 * that can be shared across all coordinator implementations
 */
extension Coordinator {
    
    func addChildCoordinator(_ coordinator: Coordinator) {
        coordinator.parentCoordinator = self
        childCoordinators.append(coordinator)
    }
    
    func removeChildCoordinator(_ coordinator: Coordinator) {
        coordinator.parentCoordinator = nil
        childCoordinators.removeAll { $0 === coordinator }
    }
    
    func removeAllChildCoordinators() {
        childCoordinators.forEach { $0.parentCoordinator = nil }
        childCoordinators.removeAll()
    }
    
    func finish() {
        parentCoordinator?.removeChildCoordinator(self)
    }
}

// MARK: - App Coordinator

/**
 * Main app coordinator managing the overall application flow
 * 
 * This class demonstrates the root coordinator pattern
 * with proper app lifecycle management and flow coordination
 */
class AppCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let window: UIWindow
    private let dependencyContainer: DIContainer
    
    init(window: UIWindow, dependencyContainer: DIContainer) {
        self.window = window
        self.dependencyContainer = dependencyContainer
        self.navigationController = UINavigationController()
    }
    
    func start() {
        window.rootViewController = navigationController
        window.makeKeyAndVisible()
        
        // Check authentication state and start appropriate flow
        if isUserAuthenticated() {
            startMainFlow()
        } else {
            startAuthenticationFlow()
        }
    }
    
    private func isUserAuthenticated() -> Bool {
        // In production, this would check actual authentication state
        return UserDefaults.standard.bool(forKey: "isAuthenticated")
    }
    
    private func startMainFlow() {
        let mainCoordinator = MainTabCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer
        )
        addChildCoordinator(mainCoordinator)
        mainCoordinator.start()
    }
    
    private func startAuthenticationFlow() {
        let authCoordinator = AuthenticationCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer
        )
        addChildCoordinator(authCoordinator)
        authCoordinator.start()
    }
}

// MARK: - Authentication Coordinator

/**
 * Authentication coordinator managing login and registration flows
 * 
 * This class demonstrates authentication flow coordination
 * with proper state management and navigation handling
 */
class AuthenticationCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let authService: AuthenticationServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.authService = dependencyContainer.resolve(AuthenticationServiceProtocol.self)
    }
    
    func start() {
        showLoginViewController()
    }
    
    private func showLoginViewController() {
        let loginViewModel = LoginViewModel(authService: authService)
        let loginViewController = LoginViewController(viewModel: loginViewModel)
        loginViewController.coordinator = self
        
        navigationController.setViewControllers([loginViewController], animated: false)
    }
    
    func showRegistrationViewController() {
        let registrationViewModel = RegistrationViewModel(authService: authService)
        let registrationViewController = RegistrationViewController(viewModel: registrationViewModel)
        registrationViewController.coordinator = self
        
        navigationController.pushViewController(registrationViewController, animated: true)
    }
    
    func showForgotPasswordViewController() {
        let forgotPasswordViewModel = ForgotPasswordViewModel(authService: authService)
        let forgotPasswordViewController = ForgotPasswordViewController(viewModel: forgotPasswordViewModel)
        forgotPasswordViewController.coordinator = self
        
        navigationController.pushViewController(forgotPasswordViewController, animated: true)
    }
    
    func authenticationDidSucceed() {
        // Notify parent coordinator
        if let appCoordinator = parentCoordinator as? AppCoordinator {
            appCoordinator.authenticationDidSucceed()
        }
    }
    
    func authenticationDidFail() {
        // Handle authentication failure
        showLoginViewController()
    }
}

// MARK: - Main Tab Coordinator

/**
 * Main tab coordinator managing the main application tabs
 * 
 * This class demonstrates tab-based navigation coordination
 * with proper tab management and flow coordination
 */
class MainTabCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private var tabBarController: UITabBarController?
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
    }
    
    func start() {
        let tabBarController = UITabBarController()
        self.tabBarController = tabBarController
        
        // Create tab coordinators
        let homeCoordinator = HomeCoordinator(
            navigationController: UINavigationController(),
            dependencyContainer: dependencyContainer
        )
        let profileCoordinator = ProfileCoordinator(
            navigationController: UINavigationController(),
            dependencyContainer: dependencyContainer
        )
        let settingsCoordinator = SettingsCoordinator(
            navigationController: UINavigationController(),
            dependencyContainer: dependencyContainer
        )
        
        // Add child coordinators
        addChildCoordinator(homeCoordinator)
        addChildCoordinator(profileCoordinator)
        addChildCoordinator(settingsCoordinator)
        
        // Start coordinators
        homeCoordinator.start()
        profileCoordinator.start()
        settingsCoordinator.start()
        
        // Setup tab bar
        tabBarController.viewControllers = [
            homeCoordinator.navigationController,
            profileCoordinator.navigationController,
            settingsCoordinator.navigationController
        ]
        
        // Setup tab bar items
        homeCoordinator.navigationController.tabBarItem = UITabBarItem(
            title: "Home",
            image: UIImage(systemName: "house"),
            selectedImage: UIImage(systemName: "house.fill")
        )
        
        profileCoordinator.navigationController.tabBarItem = UITabBarItem(
            title: "Profile",
            image: UIImage(systemName: "person"),
            selectedImage: UIImage(systemName: "person.fill")
        )
        
        settingsCoordinator.navigationController.tabBarItem = UITabBarItem(
            title: "Settings",
            image: UIImage(systemName: "gear"),
            selectedImage: UIImage(systemName: "gear.fill")
        )
        
        navigationController.setViewControllers([tabBarController], animated: false)
    }
}

// MARK: - Home Coordinator

/**
 * Home coordinator managing the home screen flow
 * 
 * This class demonstrates content screen coordination
 * with proper navigation and state management
 */
class HomeCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let homeService: HomeServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.homeService = dependencyContainer.resolve(HomeServiceProtocol.self)
    }
    
    func start() {
        showHomeViewController()
    }
    
    private func showHomeViewController() {
        let homeViewModel = HomeViewModel(homeService: homeService)
        let homeViewController = HomeViewController(viewModel: homeViewModel)
        homeViewController.coordinator = self
        
        navigationController.setViewControllers([homeViewController], animated: false)
    }
    
    func showPostDetailViewController(postId: UUID) {
        let postDetailCoordinator = PostDetailCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            postId: postId
        )
        addChildCoordinator(postDetailCoordinator)
        postDetailCoordinator.start()
    }
    
    func showUserProfileViewController(userId: UUID) {
        let profileCoordinator = UserProfileCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            userId: userId
        )
        addChildCoordinator(profileCoordinator)
        profileCoordinator.start()
    }
}

// MARK: - Profile Coordinator

/**
 * Profile coordinator managing the user profile flow
 * 
 * This class demonstrates profile management coordination
 * with proper navigation and state management
 */
class ProfileCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let profileService: ProfileServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.profileService = dependencyContainer.resolve(ProfileServiceProtocol.self)
    }
    
    func start() {
        showProfileViewController()
    }
    
    private func showProfileViewController() {
        let profileViewModel = ProfileViewModel(profileService: profileService)
        let profileViewController = ProfileViewController(viewModel: profileViewModel)
        profileViewController.coordinator = self
        
        navigationController.setViewControllers([profileViewController], animated: false)
    }
    
    func showEditProfileViewController() {
        let editProfileViewModel = EditProfileViewModel(profileService: profileService)
        let editProfileViewController = EditProfileViewController(viewModel: editProfileViewModel)
        editProfileViewController.coordinator = self
        
        navigationController.pushViewController(editProfileViewController, animated: true)
    }
    
    func showSettingsViewController() {
        let settingsCoordinator = SettingsCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer
        )
        addChildCoordinator(settingsCoordinator)
        settingsCoordinator.start()
    }
}

// MARK: - Settings Coordinator

/**
 * Settings coordinator managing the settings flow
 * 
 * This class demonstrates settings management coordination
 * with proper navigation and state management
 */
class SettingsCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let settingsService: SettingsServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.settingsService = dependencyContainer.resolve(SettingsServiceProtocol.self)
    }
    
    func start() {
        showSettingsViewController()
    }
    
    private func showSettingsViewController() {
        let settingsViewModel = SettingsViewModel(settingsService: settingsService)
        let settingsViewController = SettingsViewController(viewModel: settingsViewModel)
        settingsViewController.coordinator = self
        
        navigationController.setViewControllers([settingsViewController], animated: false)
    }
    
    func showAccountSettingsViewController() {
        let accountSettingsViewModel = AccountSettingsViewModel(settingsService: settingsService)
        let accountSettingsViewController = AccountSettingsViewController(viewModel: accountSettingsViewModel)
        accountSettingsViewController.coordinator = self
        
        navigationController.pushViewController(accountSettingsViewController, animated: true)
    }
    
    func showPrivacySettingsViewController() {
        let privacySettingsViewModel = PrivacySettingsViewModel(settingsService: settingsService)
        let privacySettingsViewController = PrivacySettingsViewController(viewModel: privacySettingsViewModel)
        privacySettingsViewController.coordinator = self
        
        navigationController.pushViewController(privacySettingsViewController, animated: true)
    }
    
    func showNotificationSettingsViewController() {
        let notificationSettingsViewModel = NotificationSettingsViewModel(settingsService: settingsService)
        let notificationSettingsViewController = NotificationSettingsViewController(viewModel: notificationSettingsViewModel)
        notificationSettingsViewController.coordinator = self
        
        navigationController.pushViewController(notificationSettingsViewController, animated: true)
    }
    
    func logout() {
        // Handle logout
        if let appCoordinator = parentCoordinator as? AppCoordinator {
            appCoordinator.logout()
        }
    }
}

// MARK: - Deep Linking Coordinator

/**
 * Deep linking coordinator managing URL-based navigation
 * 
 * This class demonstrates deep linking coordination
 * with proper URL parsing and navigation handling
 */
class DeepLinkingCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let deepLinkService: DeepLinkServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.deepLinkService = dependencyContainer.resolve(DeepLinkServiceProtocol.self)
    }
    
    func start() {
        // Deep linking coordinator doesn't have a default view
        // It handles URL-based navigation
    }
    
    func handleDeepLink(_ url: URL) {
        let deepLink = deepLinkService.parseDeepLink(url)
        
        switch deepLink {
        case .home:
            navigateToHome()
        case .profile(let userId):
            navigateToProfile(userId: userId)
        case .post(let postId):
            navigateToPost(postId: postId)
        case .settings:
            navigateToSettings()
        case .unknown:
            handleUnknownDeepLink(url)
        }
    }
    
    private func navigateToHome() {
        // Navigate to home tab
        if let tabBarController = navigationController.viewControllers.first as? UITabBarController {
            tabBarController.selectedIndex = 0
        }
    }
    
    private func navigateToProfile(userId: UUID) {
        // Navigate to profile
        let profileCoordinator = UserProfileCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            userId: userId
        )
        addChildCoordinator(profileCoordinator)
        profileCoordinator.start()
    }
    
    private func navigateToPost(postId: UUID) {
        // Navigate to post detail
        let postDetailCoordinator = PostDetailCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            postId: postId
        )
        addChildCoordinator(postDetailCoordinator)
        postDetailCoordinator.start()
    }
    
    private func navigateToSettings() {
        // Navigate to settings tab
        if let tabBarController = navigationController.viewControllers.first as? UITabBarController {
            tabBarController.selectedIndex = 2
        }
    }
    
    private func handleUnknownDeepLink(_ url: URL) {
        // Handle unknown deep links
        print("Unknown deep link: \(url)")
    }
}

// MARK: - Post Detail Coordinator

/**
 * Post detail coordinator managing post detail flow
 * 
 * This class demonstrates content detail coordination
 * with proper navigation and state management
 */
class PostDetailCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let postId: UUID
    private let postService: PostServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer, postId: UUID) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.postId = postId
        self.postService = dependencyContainer.resolve(PostServiceProtocol.self)
    }
    
    func start() {
        showPostDetailViewController()
    }
    
    private func showPostDetailViewController() {
        let postDetailViewModel = PostDetailViewModel(postService: postService, postId: postId)
        let postDetailViewController = PostDetailViewController(viewModel: postDetailViewModel)
        postDetailViewController.coordinator = self
        
        navigationController.pushViewController(postDetailViewController, animated: true)
    }
    
    func showUserProfileViewController(userId: UUID) {
        let profileCoordinator = UserProfileCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            userId: userId
        )
        addChildCoordinator(profileCoordinator)
        profileCoordinator.start()
    }
    
    func showCommentsViewController(postId: UUID) {
        let commentsCoordinator = CommentsCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            postId: postId
        )
        addChildCoordinator(commentsCoordinator)
        commentsCoordinator.start()
    }
}

// MARK: - User Profile Coordinator

/**
 * User profile coordinator managing user profile flow
 * 
 * This class demonstrates user profile coordination
 * with proper navigation and state management
 */
class UserProfileCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let userId: UUID
    private let profileService: ProfileServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer, userId: UUID) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.userId = userId
        self.profileService = dependencyContainer.resolve(ProfileServiceProtocol.self)
    }
    
    func start() {
        showUserProfileViewController()
    }
    
    private func showUserProfileViewController() {
        let userProfileViewModel = UserProfileViewModel(
            userId: userId,
            profileService: profileService
        )
        let userProfileViewController = UserProfileViewController(viewModel: userProfileViewModel)
        userProfileViewController.coordinator = self
        
        navigationController.pushViewController(userProfileViewController, animated: true)
    }
    
    func showFollowersViewController(userId: UUID) {
        let followersCoordinator = FollowersCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            userId: userId
        )
        addChildCoordinator(followersCoordinator)
        followersCoordinator.start()
    }
    
    func showFollowingViewController(userId: UUID) {
        let followingCoordinator = FollowingCoordinator(
            navigationController: navigationController,
            dependencyContainer: dependencyContainer,
            userId: userId
        )
        addChildCoordinator(followingCoordinator)
        followingCoordinator.start()
    }
}

// MARK: - Comments Coordinator

/**
 * Comments coordinator managing comments flow
 * 
 * This class demonstrates comments coordination
 * with proper navigation and state management
 */
class CommentsCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let postId: UUID
    private let commentsService: CommentsServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer, postId: UUID) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.postId = postId
        self.commentsService = dependencyContainer.resolve(CommentsServiceProtocol.self)
    }
    
    func start() {
        showCommentsViewController()
    }
    
    private func showCommentsViewController() {
        let commentsViewModel = CommentsViewModel(commentsService: commentsService, postId: postId)
        let commentsViewController = CommentsViewController(viewModel: commentsViewModel)
        commentsViewController.coordinator = self
        
        navigationController.pushViewController(commentsViewController, animated: true)
    }
}

// MARK: - Followers Coordinator

/**
 * Followers coordinator managing followers flow
 * 
 * This class demonstrates followers coordination
 * with proper navigation and state management
 */
class FollowersCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let userId: UUID
    private let followersService: FollowersServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer, userId: UUID) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.userId = userId
        self.followersService = dependencyContainer.resolve(FollowersServiceProtocol.self)
    }
    
    func start() {
        showFollowersViewController()
    }
    
    private func showFollowersViewController() {
        let followersViewModel = FollowersViewModel(followersService: followersService, userId: userId)
        let followersViewController = FollowersViewController(viewModel: followersViewModel)
        followersViewController.coordinator = self
        
        navigationController.pushViewController(followersViewController, animated: true)
    }
}

// MARK: - Following Coordinator

/**
 * Following coordinator managing following flow
 * 
 * This class demonstrates following coordination
 * with proper navigation and state management
 */
class FollowingCoordinator: Coordinator {
    
    var childCoordinators: [Coordinator] = []
    var navigationController: UINavigationController
    var parentCoordinator: Coordinator?
    
    private let dependencyContainer: DIContainer
    private let userId: UUID
    private let followingService: FollowingServiceProtocol
    
    init(navigationController: UINavigationController, dependencyContainer: DIContainer, userId: UUID) {
        self.navigationController = navigationController
        self.dependencyContainer = dependencyContainer
        self.userId = userId
        self.followingService = dependencyContainer.resolve(FollowingServiceProtocol.self)
    }
    
    func start() {
        showFollowingViewController()
    }
    
    private func showFollowingViewController() {
        let followingViewModel = FollowingViewModel(followingService: followingService, userId: userId)
        let followingViewController = FollowingViewController(viewModel: followingViewModel)
        followingViewController.coordinator = self
        
        navigationController.pushViewController(followingViewController, animated: true)
    }
}

// MARK: - Supporting Types

/**
 * Deep link types for URL-based navigation
 */
enum DeepLink {
    case home
    case profile(UUID)
    case post(UUID)
    case settings
    case unknown
}

/**
 * Deep link service protocol for URL parsing
 */
protocol DeepLinkServiceProtocol {
    func parseDeepLink(_ url: URL) -> DeepLink
}

/**
 * Deep link service implementation
 */
class DeepLinkService: DeepLinkServiceProtocol {
    
    func parseDeepLink(_ url: URL) -> DeepLink {
        guard let components = URLComponents(url: url, resolvingAgainstBaseURL: false) else {
            return .unknown
        }
        
        switch components.path {
        case "/home":
            return .home
        case "/profile":
            if let userIdString = components.queryItems?.first(where: { $0.name == "id" })?.value,
               let userId = UUID(uuidString: userIdString) {
                return .profile(userId)
            }
            return .unknown
        case "/post":
            if let postIdString = components.queryItems?.first(where: { $0.name == "id" })?.value,
               let postId = UUID(uuidString: postIdString) {
                return .post(postId)
            }
            return .unknown
        case "/settings":
            return .settings
        default:
            return .unknown
        }
    }
}

// MARK: - Service Protocols

protocol AuthenticationServiceProtocol {}
protocol HomeServiceProtocol {}
protocol ProfileServiceProtocol {}
protocol SettingsServiceProtocol {}
protocol PostServiceProtocol {}
protocol CommentsServiceProtocol {}
protocol FollowersServiceProtocol {}
protocol FollowingServiceProtocol {}

// MARK: - View Controllers (Placeholder)

class LoginViewController: UIViewController {
    weak var coordinator: AuthenticationCoordinator?
    let viewModel: LoginViewModel
    
    init(viewModel: LoginViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class RegistrationViewController: UIViewController {
    weak var coordinator: AuthenticationCoordinator?
    let viewModel: RegistrationViewModel
    
    init(viewModel: RegistrationViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class ForgotPasswordViewController: UIViewController {
    weak var coordinator: AuthenticationCoordinator?
    let viewModel: ForgotPasswordViewModel
    
    init(viewModel: ForgotPasswordViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class HomeViewController: UIViewController {
    weak var coordinator: HomeCoordinator?
    let viewModel: HomeViewModel
    
    init(viewModel: HomeViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class ProfileViewController: UIViewController {
    weak var coordinator: ProfileCoordinator?
    let viewModel: ProfileViewModel
    
    init(viewModel: ProfileViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class EditProfileViewController: UIViewController {
    weak var coordinator: ProfileCoordinator?
    let viewModel: EditProfileViewModel
    
    init(viewModel: EditProfileViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class SettingsViewController: UIViewController {
    weak var coordinator: SettingsCoordinator?
    let viewModel: SettingsViewModel
    
    init(viewModel: SettingsViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class AccountSettingsViewController: UIViewController {
    weak var coordinator: SettingsCoordinator?
    let viewModel: AccountSettingsViewModel
    
    init(viewModel: AccountSettingsViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class PrivacySettingsViewController: UIViewController {
    weak var coordinator: SettingsCoordinator?
    let viewModel: PrivacySettingsViewModel
    
    init(viewModel: PrivacySettingsViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class NotificationSettingsViewController: UIViewController {
    weak var coordinator: SettingsCoordinator?
    let viewModel: NotificationSettingsViewModel
    
    init(viewModel: NotificationSettingsViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class PostDetailViewController: UIViewController {
    weak var coordinator: PostDetailCoordinator?
    let viewModel: PostDetailViewModel
    
    init(viewModel: PostDetailViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class UserProfileViewController: UIViewController {
    weak var coordinator: UserProfileCoordinator?
    let viewModel: UserProfileViewModel
    
    init(viewModel: UserProfileViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class CommentsViewController: UIViewController {
    weak var coordinator: CommentsCoordinator?
    let viewModel: CommentsViewModel
    
    init(viewModel: CommentsViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class FollowersViewController: UIViewController {
    weak var coordinator: FollowersCoordinator?
    let viewModel: FollowersViewModel
    
    init(viewModel: FollowersViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class FollowingViewController: UIViewController {
    weak var coordinator: FollowingCoordinator?
    let viewModel: FollowingViewModel
    
    init(viewModel: FollowingViewModel) {
        self.viewModel = viewModel
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

// MARK: - View Models (Placeholder)

class LoginViewModel: ObservableObject {}
class RegistrationViewModel: ObservableObject {}
class ForgotPasswordViewModel: ObservableObject {}
class HomeViewModel: ObservableObject {}
class ProfileViewModel: ObservableObject {}
class EditProfileViewModel: ObservableObject {}
class SettingsViewModel: ObservableObject {}
class AccountSettingsViewModel: ObservableObject {}
class PrivacySettingsViewModel: ObservableObject {}
class NotificationSettingsViewModel: ObservableObject {}
class PostDetailViewModel: ObservableObject {}
class UserProfileViewModel: ObservableObject {}
class CommentsViewModel: ObservableObject {}
class FollowersViewModel: ObservableObject {}
class FollowingViewModel: ObservableObject {}

// MARK: - Usage Examples

/**
 * Demonstrates how to use the Coordinator pattern
 * 
 * This function shows practical usage of all the coordinator components
 */
func demonstrateCoordinatorPattern() {
    print("=== Coordinator Pattern Demonstration ===\n")
    
    // Setup dependency injection
    let container = DIContainer()
    container.setup()
    
    // Create app coordinator
    let window = UIWindow(frame: UIScreen.main.bounds)
    let appCoordinator = AppCoordinator(window: window, dependencyContainer: container)
    
    // Start the app
    appCoordinator.start()
    
    print("--- App Coordinator Started ---")
    print("Child coordinators: \(appCoordinator.childCoordinators.count)")
    
    // Demonstrate deep linking
    let deepLinkCoordinator = DeepLinkingCoordinator(
        navigationController: appCoordinator.navigationController,
        dependencyContainer: container
    )
    
    print("\n--- Deep Linking ---")
    let profileURL = URL(string: "myapp://profile?id=123e4567-e89b-12d3-a456-426614174000")!
    deepLinkCoordinator.handleDeepLink(profileURL)
    
    let postURL = URL(string: "myapp://post?id=987fcdeb-51a2-43d7-8f9e-123456789abc")!
    deepLinkCoordinator.handleDeepLink(postURL)
    
    let settingsURL = URL(string: "myapp://settings")!
    deepLinkCoordinator.handleDeepLink(settingsURL)
    
    print("\n--- Coordinator Hierarchy ---")
    print("App Coordinator")
    print("├── Authentication Coordinator")
    print("├── Main Tab Coordinator")
    print("│   ├── Home Coordinator")
    print("│   ├── Profile Coordinator")
    print("│   └── Settings Coordinator")
    print("└── Deep Linking Coordinator")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateCoordinatorPattern()
