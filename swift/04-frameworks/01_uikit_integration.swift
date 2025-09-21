/*
 * iOS Frameworks: UIKit Integration
 * 
 * This file demonstrates production-grade UIKit integration patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master advanced UIKit components and custom controls
 * - Understand animation and transition patterns
 * - Implement gesture recognition and touch handling
 * - Apply Auto Layout and responsive design principles
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import UIKit
import Foundation

// MARK: - Custom UI Components

/**
 * Custom button with advanced styling and animation
 * 
 * This class demonstrates production-grade custom UI components
 * with proper accessibility support and animation
 */
class CustomButton: UIButton {
    
    // MARK: - Properties
    
    enum Style {
        case primary
        case secondary
        case destructive
        case ghost
    }
    
    private let style: Style
    private var originalBackgroundColor: UIColor?
    private var originalTitleColor: UIColor?
    
    // MARK: - Initialization
    
    init(style: Style, title: String) {
        self.style = style
        super.init(frame: .zero)
        
        setupButton()
        setTitle(title, for: .normal)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    // MARK: - Setup
    
    private func setupButton() {
        // Configure button appearance
        configureAppearance()
        
        // Setup constraints
        setupConstraints()
        
        // Configure accessibility
        setupAccessibility()
        
        // Add touch handlers
        addTarget(self, action: #selector(buttonTouched), for: .touchDown)
        addTarget(self, action: #selector(buttonReleased), for: [.touchUpInside, .touchUpOutside, .touchCancel])
    }
    
    private func configureAppearance() {
        layer.cornerRadius = 8
        layer.masksToBounds = true
        
        // Configure based on style
        switch style {
        case .primary:
            backgroundColor = .systemBlue
            setTitleColor(.white, for: .normal)
        case .secondary:
            backgroundColor = .systemGray5
            setTitleColor(.systemBlue, for: .normal)
        case .destructive:
            backgroundColor = .systemRed
            setTitleColor(.white, for: .normal)
        case .ghost:
            backgroundColor = .clear
            setTitleColor(.systemBlue, for: .normal)
            layer.borderWidth = 1
            layer.borderColor = UIColor.systemBlue.cgColor
        }
        
        // Store original colors for animation
        originalBackgroundColor = backgroundColor
        originalTitleColor = titleColor(for: .normal)
        
        // Configure title font
        titleLabel?.font = UIFont.systemFont(ofSize: 16, weight: .medium)
    }
    
    private func setupConstraints() {
        translatesAutoresizingMaskIntoConstraints = false
        
        // Set minimum height
        heightAnchor.constraint(greaterThanOrEqualToConstant: 44).isActive = true
        
        // Set horizontal padding
        contentEdgeInsets = UIEdgeInsets(top: 12, left: 24, bottom: 12, right: 24)
    }
    
    private func setupAccessibility() {
        isAccessibilityElement = true
        accessibilityTraits = .button
        accessibilityHint = "Double tap to activate"
    }
    
    // MARK: - Touch Handling
    
    @objc private func buttonTouched() {
        animateTouchDown()
    }
    
    @objc private func buttonReleased() {
        animateTouchUp()
    }
    
    private func animateTouchDown() {
        UIView.animate(withDuration: 0.1, delay: 0, options: [.allowUserInteraction, .beginFromCurrentState]) {
            self.transform = CGAffineTransform(scaleX: 0.95, y: 0.95)
            self.alpha = 0.8
        }
    }
    
    private func animateTouchUp() {
        UIView.animate(withDuration: 0.1, delay: 0, options: [.allowUserInteraction, .beginFromCurrentState]) {
            self.transform = .identity
            self.alpha = 1.0
        }
    }
    
    // MARK: - Public Methods
    
    func setLoading(_ isLoading: Bool) {
        if isLoading {
            setTitle("", for: .normal)
            addLoadingIndicator()
        } else {
            setTitle(originalTitle, for: .normal)
            removeLoadingIndicator()
        }
    }
    
    private func addLoadingIndicator() {
        let activityIndicator = UIActivityIndicatorView(style: .medium)
        activityIndicator.color = originalTitleColor
        activityIndicator.translatesAutoresizingMaskIntoConstraints = false
        activityIndicator.startAnimating()
        
        addSubview(activityIndicator)
        activityIndicator.centerXAnchor.constraint(equalTo: centerXAnchor).isActive = true
        activityIndicator.centerYAnchor.constraint(equalTo: centerYAnchor).isActive = true
    }
    
    private func removeLoadingIndicator() {
        subviews.compactMap { $0 as? UIActivityIndicatorView }.forEach { $0.removeFromSuperview() }
    }
}

// MARK: - Advanced Table View Controller

/**
 * Advanced table view controller with custom cells and animations
 * 
 * This class demonstrates production-grade table view implementation
 * with proper data management and user experience
 */
class AdvancedTableViewController: UIViewController {
    
    // MARK: - Properties
    
    private let tableView = UITableView(frame: .zero, style: .plain)
    private var dataSource: [TableItem] = []
    private let refreshControl = UIRefreshControl()
    
    // MARK: - Lifecycle
    
    override func viewDidLoad() {
        super.viewDidLoad()
        setupTableView()
        setupRefreshControl()
        loadData()
    }
    
    // MARK: - Setup
    
    private func setupTableView() {
        view.addSubview(tableView)
        tableView.translatesAutoresizingMaskIntoConstraints = false
        
        NSLayoutConstraint.activate([
            tableView.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor),
            tableView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            tableView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            tableView.bottomAnchor.constraint(equalTo: view.bottomAnchor)
        ])
        
        // Configure table view
        tableView.delegate = self
        tableView.dataSource = self
        tableView.separatorStyle = .none
        tableView.backgroundColor = .systemBackground
        
        // Register custom cell
        tableView.register(AdvancedTableViewCell.self, forCellReuseIdentifier: AdvancedTableViewCell.identifier)
    }
    
    private func setupRefreshControl() {
        refreshControl.addTarget(self, action: #selector(refreshData), for: .valueChanged)
        tableView.refreshControl = refreshControl
    }
    
    // MARK: - Data Management
    
    private func loadData() {
        // Simulate network request
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            // Simulate delay
            Thread.sleep(forTimeInterval: 1.0)
            
            let items = self?.generateMockData() ?? []
            
            DispatchQueue.main.async {
                self?.dataSource = items
                self?.tableView.reloadData()
                self?.refreshControl.endRefreshing()
            }
        }
    }
    
    @objc private func refreshData() {
        loadData()
    }
    
    private func generateMockData() -> [TableItem] {
        return (1...50).map { index in
            TableItem(
                id: UUID(),
                title: "Item \(index)",
                subtitle: "Subtitle for item \(index)",
                imageURL: "https://picsum.photos/100/100?random=\(index)",
                isFavorite: Bool.random()
            )
        }
    }
}

// MARK: - Table View Data Source & Delegate

extension AdvancedTableViewController: UITableViewDataSource, UITableViewDelegate {
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return dataSource.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: AdvancedTableViewCell.identifier, for: indexPath) as? AdvancedTableViewCell else {
            return UITableViewCell()
        }
        
        let item = dataSource[indexPath.row]
        cell.configure(with: item)
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 80
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let item = dataSource[indexPath.row]
        handleItemSelection(item)
    }
    
    private func handleItemSelection(_ item: TableItem) {
        // Handle item selection
        print("Selected item: \(item.title)")
    }
}

// MARK: - Custom Table View Cell

/**
 * Custom table view cell with advanced features
 * 
 * This class demonstrates production-grade custom cell implementation
 * with proper layout, animation, and user interaction
 */
class AdvancedTableViewCell: UITableViewCell {
    
    static let identifier = "AdvancedTableViewCell"
    
    // MARK: - UI Elements
    
    private let containerView = UIView()
    private let titleLabel = UILabel()
    private let subtitleLabel = UILabel()
    private let thumbnailImageView = UIImageView()
    private let favoriteButton = UIButton(type: .system)
    private let separatorView = UIView()
    
    // MARK: - Properties
    
    private var item: TableItem?
    private var favoriteButtonAction: (() -> Void)?
    
    // MARK: - Initialization
    
    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        setupCell()
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    // MARK: - Setup
    
    private func setupCell() {
        selectionStyle = .none
        backgroundColor = .clear
        
        setupContainerView()
        setupThumbnailImageView()
        setupLabels()
        setupFavoriteButton()
        setupSeparatorView()
        setupConstraints()
    }
    
    private func setupContainerView() {
        containerView.backgroundColor = .systemBackground
        containerView.layer.cornerRadius = 8
        containerView.layer.shadowColor = UIColor.black.cgColor
        containerView.layer.shadowOffset = CGSize(width: 0, height: 1)
        containerView.layer.shadowRadius = 2
        containerView.layer.shadowOpacity = 0.1
        
        contentView.addSubview(containerView)
    }
    
    private func setupThumbnailImageView() {
        thumbnailImageView.contentMode = .scaleAspectFill
        thumbnailImageView.clipsToBounds = true
        thumbnailImageView.layer.cornerRadius = 4
        thumbnailImageView.backgroundColor = .systemGray5
        
        containerView.addSubview(thumbnailImageView)
    }
    
    private func setupLabels() {
        titleLabel.font = UIFont.systemFont(ofSize: 16, weight: .medium)
        titleLabel.textColor = .label
        titleLabel.numberOfLines = 1
        
        subtitleLabel.font = UIFont.systemFont(ofSize: 14, weight: .regular)
        subtitleLabel.textColor = .secondaryLabel
        subtitleLabel.numberOfLines = 2
        
        containerView.addSubview(titleLabel)
        containerView.addSubview(subtitleLabel)
    }
    
    private func setupFavoriteButton() {
        favoriteButton.setImage(UIImage(systemName: "heart"), for: .normal)
        favoriteButton.setImage(UIImage(systemName: "heart.fill"), for: .selected)
        favoriteButton.tintColor = .systemRed
        favoriteButton.addTarget(self, action: #selector(favoriteButtonTapped), for: .touchUpInside)
        
        containerView.addSubview(favoriteButton)
    }
    
    private func setupSeparatorView() {
        separatorView.backgroundColor = .separator
        
        containerView.addSubview(separatorView)
    }
    
    private func setupConstraints() {
        containerView.translatesAutoresizingMaskIntoConstraints = false
        thumbnailImageView.translatesAutoresizingMaskIntoConstraints = false
        titleLabel.translatesAutoresizingMaskIntoConstraints = false
        subtitleLabel.translatesAutoresizingMaskIntoConstraints = false
        favoriteButton.translatesAutoresizingMaskIntoConstraints = false
        separatorView.translatesAutoresizingMaskIntoConstraints = false
        
        NSLayoutConstraint.activate([
            // Container view
            containerView.topAnchor.constraint(equalTo: contentView.topAnchor, constant: 4),
            containerView.leadingAnchor.constraint(equalTo: contentView.leadingAnchor, constant: 16),
            containerView.trailingAnchor.constraint(equalTo: contentView.trailingAnchor, constant: -16),
            containerView.bottomAnchor.constraint(equalTo: contentView.bottomAnchor, constant: -4),
            
            // Thumbnail image view
            thumbnailImageView.leadingAnchor.constraint(equalTo: containerView.leadingAnchor, constant: 12),
            thumbnailImageView.centerYAnchor.constraint(equalTo: containerView.centerYAnchor),
            thumbnailImageView.widthAnchor.constraint(equalToConstant: 60),
            thumbnailImageView.heightAnchor.constraint(equalToConstant: 60),
            
            // Title label
            titleLabel.topAnchor.constraint(equalTo: containerView.topAnchor, constant: 12),
            titleLabel.leadingAnchor.constraint(equalTo: thumbnailImageView.trailingAnchor, constant: 12),
            titleLabel.trailingAnchor.constraint(equalTo: favoriteButton.leadingAnchor, constant: -8),
            
            // Subtitle label
            subtitleLabel.topAnchor.constraint(equalTo: titleLabel.bottomAnchor, constant: 4),
            subtitleLabel.leadingAnchor.constraint(equalTo: titleLabel.leadingAnchor),
            subtitleLabel.trailingAnchor.constraint(equalTo: titleLabel.trailingAnchor),
            subtitleLabel.bottomAnchor.constraint(lessThanOrEqualTo: containerView.bottomAnchor, constant: -12),
            
            // Favorite button
            favoriteButton.trailingAnchor.constraint(equalTo: containerView.trailingAnchor, constant: -12),
            favoriteButton.centerYAnchor.constraint(equalTo: containerView.centerYAnchor),
            favoriteButton.widthAnchor.constraint(equalToConstant: 30),
            favoriteButton.heightAnchor.constraint(equalToConstant: 30),
            
            // Separator view
            separatorView.leadingAnchor.constraint(equalTo: containerView.leadingAnchor),
            separatorView.trailingAnchor.constraint(equalTo: containerView.trailingAnchor),
            separatorView.bottomAnchor.constraint(equalTo: containerView.bottomAnchor),
            separatorView.heightAnchor.constraint(equalToConstant: 0.5)
        ])
    }
    
    // MARK: - Configuration
    
    func configure(with item: TableItem) {
        self.item = item
        
        titleLabel.text = item.title
        subtitleLabel.text = item.subtitle
        favoriteButton.isSelected = item.isFavorite
        
        // Load image asynchronously
        loadImage(from: item.imageURL)
    }
    
    private func loadImage(from urlString: String) {
        guard let url = URL(string: urlString) else { return }
        
        // Simulate image loading
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            // In production, you would use a proper image loading library
            // like SDWebImage or Kingfisher
            let image = UIImage(systemName: "photo")
            
            DispatchQueue.main.async {
                self?.thumbnailImageView.image = image
            }
        }
    }
    
    // MARK: - Actions
    
    @objc private func favoriteButtonTapped() {
        guard let item = item else { return }
        
        // Animate button state change
        UIView.animate(withDuration: 0.2, delay: 0, options: [.allowUserInteraction, .beginFromCurrentState]) {
            self.favoriteButton.transform = CGAffineTransform(scaleX: 1.2, y: 1.2)
        } completion: { _ in
            UIView.animate(withDuration: 0.2) {
                self.favoriteButton.transform = .identity
            }
        }
        
        // Toggle favorite state
        favoriteButton.isSelected.toggle()
        
        // Notify delegate or use callback
        favoriteButtonAction?()
    }
    
    func setFavoriteButtonAction(_ action: @escaping () -> Void) {
        favoriteButtonAction = action
    }
}

// MARK: - Gesture Recognition

/**
 * Custom gesture recognizer for advanced touch handling
 * 
 * This class demonstrates production-grade gesture recognition
 * with proper touch handling and user experience
 */
class CustomGestureRecognizer: UIGestureRecognizer {
    
    // MARK: - Properties
    
    private var startPoint: CGPoint = .zero
    private var currentPoint: CGPoint = .zero
    private var minimumDistance: CGFloat = 10
    private var maximumDistance: CGFloat = 100
    
    // MARK: - Touch Handling
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        
        startPoint = touch.location(in: view)
        currentPoint = startPoint
        state = .began
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        
        currentPoint = touch.location(in: view)
        let distance = sqrt(pow(currentPoint.x - startPoint.x, 2) + pow(currentPoint.y - startPoint.y, 2))
        
        if distance >= minimumDistance {
            state = .changed
        }
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        let distance = sqrt(pow(currentPoint.x - startPoint.x, 2) + pow(currentPoint.y - startPoint.y, 2))
        
        if distance >= minimumDistance && distance <= maximumDistance {
            state = .ended
        } else {
            state = .cancelled
        }
    }
    
    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        state = .cancelled
    }
    
    // MARK: - Public Methods
    
    func getGestureDirection() -> UISwipeGestureRecognizer.Direction? {
        let deltaX = currentPoint.x - startPoint.x
        let deltaY = currentPoint.y - startPoint.y
        
        if abs(deltaX) > abs(deltaY) {
            return deltaX > 0 ? .right : .left
        } else {
            return deltaY > 0 ? .down : .up
        }
    }
    
    func getGestureDistance() -> CGFloat {
        return sqrt(pow(currentPoint.x - startPoint.x, 2) + pow(currentPoint.y - startPoint.y, 2))
    }
}

// MARK: - Animation Utilities

/**
 * Animation utilities for smooth UI transitions
 * 
 * This class demonstrates production-grade animation patterns
 * with proper timing and easing functions
 */
class AnimationUtilities {
    
    // MARK: - Spring Animations
    
    static func springAnimation(
        duration: TimeInterval = 0.5,
        delay: TimeInterval = 0,
        damping: CGFloat = 0.8,
        initialVelocity: CGFloat = 0,
        animations: @escaping () -> Void,
        completion: ((Bool) -> Void)? = nil
    ) {
        UIView.animate(
            withDuration: duration,
            delay: delay,
            usingSpringWithDamping: damping,
            initialSpringVelocity: initialVelocity,
            options: [.allowUserInteraction, .beginFromCurrentState],
            animations: animations,
            completion: completion
        )
    }
    
    // MARK: - Fade Animations
    
    static func fadeIn(
        view: UIView,
        duration: TimeInterval = 0.3,
        completion: ((Bool) -> Void)? = nil
    ) {
        view.alpha = 0
        UIView.animate(
            withDuration: duration,
            animations: {
                view.alpha = 1
            },
            completion: completion
        )
    }
    
    static func fadeOut(
        view: UIView,
        duration: TimeInterval = 0.3,
        completion: ((Bool) -> Void)? = nil
    ) {
        UIView.animate(
            withDuration: duration,
            animations: {
                view.alpha = 0
            },
            completion: completion
        )
    }
    
    // MARK: - Scale Animations
    
    static func scaleIn(
        view: UIView,
        duration: TimeInterval = 0.3,
        scale: CGFloat = 0.8,
        completion: ((Bool) -> Void)? = nil
    ) {
        view.transform = CGAffineTransform(scaleX: scale, y: scale)
        UIView.animate(
            withDuration: duration,
            animations: {
                view.transform = .identity
            },
            completion: completion
        )
    }
    
    static func scaleOut(
        view: UIView,
        duration: TimeInterval = 0.3,
        scale: CGFloat = 0.8,
        completion: ((Bool) -> Void)? = nil
    ) {
        UIView.animate(
            withDuration: duration,
            animations: {
                view.transform = CGAffineTransform(scaleX: scale, y: scale)
            },
            completion: completion
        )
    }
    
    // MARK: - Slide Animations
    
    static func slideIn(
        view: UIView,
        from direction: SlideDirection,
        duration: TimeInterval = 0.3,
        completion: ((Bool) -> Void)? = nil
    ) {
        let screenBounds = UIScreen.main.bounds
        var initialTransform: CGAffineTransform
        
        switch direction {
        case .left:
            initialTransform = CGAffineTransform(translationX: -screenBounds.width, y: 0)
        case .right:
            initialTransform = CGAffineTransform(translationX: screenBounds.width, y: 0)
        case .top:
            initialTransform = CGAffineTransform(translationX: 0, y: -screenBounds.height)
        case .bottom:
            initialTransform = CGAffineTransform(translationX: 0, y: screenBounds.height)
        }
        
        view.transform = initialTransform
        UIView.animate(
            withDuration: duration,
            animations: {
                view.transform = .identity
            },
            completion: completion
        )
    }
    
    static func slideOut(
        view: UIView,
        to direction: SlideDirection,
        duration: TimeInterval = 0.3,
        completion: ((Bool) -> Void)? = nil
    ) {
        let screenBounds = UIScreen.main.bounds
        var finalTransform: CGAffineTransform
        
        switch direction {
        case .left:
            finalTransform = CGAffineTransform(translationX: -screenBounds.width, y: 0)
        case .right:
            finalTransform = CGAffineTransform(translationX: screenBounds.width, y: 0)
        case .top:
            finalTransform = CGAffineTransform(translationX: 0, y: -screenBounds.height)
        case .bottom:
            finalTransform = CGAffineTransform(translationX: 0, y: screenBounds.height)
        }
        
        UIView.animate(
            withDuration: duration,
            animations: {
                view.transform = finalTransform
            },
            completion: completion
        )
    }
}

// MARK: - Supporting Types

/**
 * Table item model for demonstration
 */
struct TableItem {
    let id: UUID
    let title: String
    let subtitle: String
    let imageURL: String
    let isFavorite: Bool
}

/**
 * Slide direction enum for animations
 */
enum SlideDirection {
    case left
    case right
    case top
    case bottom
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use UIKit integration patterns
 * 
 * This function shows practical usage of all the UIKit components
 */
func demonstrateUIKitIntegration() {
    print("=== UIKit Integration Demonstration ===\n")
    
    // Create custom button
    let primaryButton = CustomButton(style: .primary, title: "Primary Button")
    let secondaryButton = CustomButton(style: .secondary, title: "Secondary Button")
    let destructiveButton = CustomButton(style: .destructive, title: "Destructive Button")
    let ghostButton = CustomButton(style: .ghost, title: "Ghost Button")
    
    print("--- Custom Buttons ---")
    print("Primary button: \(type(of: primaryButton))")
    print("Secondary button: \(type(of: secondaryButton))")
    print("Destructive button: \(type(of: destructiveButton))")
    print("Ghost button: \(type(of: ghostButton))")
    
    // Demonstrate button loading state
    primaryButton.setLoading(true)
    DispatchQueue.main.asyncAfter(deadline: .now() + 2.0) {
        primaryButton.setLoading(false)
    }
    
    // Create advanced table view controller
    let tableViewController = AdvancedTableViewController()
    print("\n--- Advanced Table View ---")
    print("Table view controller: \(type(of: tableViewController))")
    print("Data source count: \(tableViewController.dataSource.count)")
    
    // Create custom gesture recognizer
    let gestureRecognizer = CustomGestureRecognizer()
    print("\n--- Custom Gesture Recognizer ---")
    print("Gesture recognizer: \(type(of: gestureRecognizer))")
    print("Minimum distance: \(gestureRecognizer.minimumDistance)")
    print("Maximum distance: \(gestureRecognizer.maximumDistance)")
    
    // Demonstrate animations
    print("\n--- Animation Utilities ---")
    print("Spring animation available")
    print("Fade animations available")
    print("Scale animations available")
    print("Slide animations available")
    
    // Create custom cell
    let customCell = AdvancedTableViewCell(style: .default, reuseIdentifier: AdvancedTableViewCell.identifier)
    let mockItem = TableItem(
        id: UUID(),
        title: "Sample Item",
        subtitle: "This is a sample subtitle for demonstration",
        imageURL: "https://example.com/image.jpg",
        isFavorite: false
    )
    customCell.configure(with: mockItem)
    
    print("\n--- Custom Table View Cell ---")
    print("Custom cell: \(type(of: customCell))")
    print("Configured with item: \(mockItem.title)")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateUIKitIntegration()
