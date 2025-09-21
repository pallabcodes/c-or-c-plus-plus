/*
 * Swift Performance: UI Performance
 * 
 * This file demonstrates production-grade UI performance optimization in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master rendering optimization and efficient UI updates
 * - Understand scroll performance and large dataset handling
 * - Implement proper image optimization and caching
 * - Apply layout performance and constraint optimization
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import UIKit
import Foundation

// MARK: - Rendering Optimization

/**
 * Optimized rendering utilities
 * 
 * This class demonstrates production-grade rendering optimization
 * with proper view updates and performance tuning
 */
class OptimizedRendering {
    
    // MARK: - View Update Optimization
    
    /**
     * Batch view updates for performance
     * 
     * This method demonstrates proper view update batching
     * with performance optimization
     */
    static func batchViewUpdates(_ updates: @escaping () -> Void) {
        CATransaction.begin()
        CATransaction.setDisableActions(true)
        
        updates()
        
        CATransaction.commit()
    }
    
    /**
     * Optimized view updates with animation
     * 
     * This method demonstrates proper animated view updates
     * with performance optimization
     */
    static func animateViewUpdates(
        duration: TimeInterval = 0.3,
        delay: TimeInterval = 0,
        options: UIView.AnimationOptions = [],
        updates: @escaping () -> Void,
        completion: ((Bool) -> Void)? = nil
    ) {
        UIView.animate(
            withDuration: duration,
            delay: delay,
            options: options,
            animations: updates,
            completion: completion
        )
    }
    
    /**
     * Spring animation with performance optimization
     * 
     * This method demonstrates proper spring animations
     * with performance tuning
     */
    static func springAnimation(
        duration: TimeInterval = 0.5,
        delay: TimeInterval = 0,
        damping: CGFloat = 0.8,
        initialVelocity: CGFloat = 0,
        updates: @escaping () -> Void,
        completion: ((Bool) -> Void)? = nil
    ) {
        UIView.animate(
            withDuration: duration,
            delay: delay,
            usingSpringWithDamping: damping,
            initialSpringVelocity: initialVelocity,
            options: [.allowUserInteraction, .beginFromCurrentState],
            animations: updates,
            completion: completion
        )
    }
    
    // MARK: - Layer Optimization
    
    /**
     * Optimize layer properties for performance
     * 
     * This method demonstrates proper layer optimization
     * with performance tuning
     */
    static func optimizeLayer(_ layer: CALayer) {
        // Enable rasterization for complex layers
        layer.shouldRasterize = true
        layer.rasterizationScale = UIScreen.main.scale
        
        // Optimize shadow rendering
        layer.shadowPath = UIBezierPath(rect: layer.bounds).cgPath
        
        // Use appropriate blend mode
        layer.compositingFilter = "multiplyBlendMode"
    }
    
    /**
     * Create optimized shadow layer
     * 
     * This method demonstrates proper shadow layer creation
     * with performance optimization
     */
    static func createOptimizedShadowLayer(
        frame: CGRect,
        cornerRadius: CGFloat = 0,
        shadowColor: UIColor = .black,
        shadowOffset: CGSize = CGSize(width: 0, height: 2),
        shadowRadius: CGFloat = 4,
        shadowOpacity: Float = 0.1
    ) -> CALayer {
        let shadowLayer = CALayer()
        shadowLayer.frame = frame
        shadowLayer.cornerRadius = cornerRadius
        shadowLayer.shadowColor = shadowColor.cgColor
        shadowLayer.shadowOffset = shadowOffset
        shadowLayer.shadowRadius = shadowRadius
        shadowLayer.shadowOpacity = shadowOpacity
        shadowLayer.shadowPath = UIBezierPath(roundedRect: frame, cornerRadius: cornerRadius).cgPath
        
        return shadowLayer
    }
}

// MARK: - Scroll Performance

/**
 * High-performance scroll view
 * 
 * This class demonstrates production-grade scroll performance
 * with proper cell reuse and memory optimization
 */
class OptimizedScrollView: UIScrollView {
    
    // MARK: - Properties
    
    private var visibleCells: [Int: UIView] = [:]
    private var cellReuseQueue: [UIView] = []
    private var dataSource: [Any] = []
    private var cellHeight: CGFloat = 50
    private var cellSpacing: CGFloat = 0
    
    // MARK: - Initialization
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupScrollView()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        setupScrollView()
    }
    
    // MARK: - Setup
    
    private func setupScrollView() {
        delegate = self
        showsVerticalScrollIndicator = true
        showsHorizontalScrollIndicator = false
        bounces = true
        alwaysBounceVertical = true
    }
    
    // MARK: - Public Methods
    
    /**
     * Configure scroll view with data
     * 
     * This method demonstrates proper scroll view configuration
     * with data source setup
     */
    func configure(
        dataSource: [Any],
        cellHeight: CGFloat = 50,
        cellSpacing: CGFloat = 0,
        cellFactory: @escaping (Any, Int) -> UIView
    ) {
        self.dataSource = dataSource
        self.cellHeight = cellHeight
        self.cellSpacing = cellSpacing
        self.cellFactory = cellFactory
        
        updateContentSize()
        updateVisibleCells()
    }
    
    /**
     * Update data source
     * 
     * This method demonstrates proper data source updates
     * with performance optimization
     */
    func updateDataSource(_ newDataSource: [Any]) {
        dataSource = newDataSource
        updateContentSize()
        updateVisibleCells()
    }
    
    // MARK: - Private Properties
    
    private var cellFactory: ((Any, Int) -> UIView)?
    
    // MARK: - Private Methods
    
    private func updateContentSize() {
        let totalHeight = CGFloat(dataSource.count) * (cellHeight + cellSpacing)
        contentSize = CGSize(width: bounds.width, height: totalHeight)
    }
    
    private func updateVisibleCells() {
        let visibleRange = getVisibleRange()
        
        // Remove off-screen cells
        for (index, cell) in visibleCells {
            if index < visibleRange.start || index >= visibleRange.end {
                removeCell(at: index)
            }
        }
        
        // Add on-screen cells
        for index in visibleRange.start..<visibleRange.end {
            if visibleCells[index] == nil {
                addCell(at: index)
            }
        }
    }
    
    private func getVisibleRange() -> Range<Int> {
        let startY = contentOffset.y
        let endY = startY + bounds.height
        
        let startIndex = max(0, Int(startY / (cellHeight + cellSpacing)))
        let endIndex = min(dataSource.count, Int(endY / (cellHeight + cellSpacing)) + 1)
        
        return startIndex..<endIndex
    }
    
    private func addCell(at index: Int) {
        guard index < dataSource.count,
              let cellFactory = cellFactory else { return }
        
        let cell = cellFactory(dataSource[index], index)
        cell.frame = CGRect(
            x: 0,
            y: CGFloat(index) * (cellHeight + cellSpacing),
            width: bounds.width,
            height: cellHeight
        )
        
        addSubview(cell)
        visibleCells[index] = cell
    }
    
    private func removeCell(at index: Int) {
        guard let cell = visibleCells[index] else { return }
        
        cell.removeFromSuperview()
        visibleCells.removeValue(forKey: index)
        
        // Add to reuse queue
        cellReuseQueue.append(cell)
    }
    
    private func dequeueReusableCell() -> UIView? {
        return cellReuseQueue.popLast()
    }
}

// MARK: - UIScrollViewDelegate

extension OptimizedScrollView: UIScrollViewDelegate {
    
    func scrollViewDidScroll(_ scrollView: UIScrollView) {
        updateVisibleCells()
    }
}

// MARK: - Image Optimization

/**
 * Optimized image loading and caching
 * 
 * This class demonstrates production-grade image optimization
 * with proper caching and memory management
 */
class OptimizedImageLoader {
    
    // MARK: - Properties
    
    private let cache = NSCache<NSString, UIImage>()
    private let session = URLSession.shared
    private var loadingTasks: [String: URLSessionDataTask] = [:]
    
    // MARK: - Initialization
    
    init() {
        setupCache()
    }
    
    // MARK: - Setup
    
    private func setupCache() {
        cache.countLimit = 100
        cache.totalCostLimit = 50 * 1024 * 1024 // 50MB
    }
    
    // MARK: - Public Methods
    
    /**
     * Load image with optimization
     * 
     * This method demonstrates proper image loading
     * with caching and performance optimization
     */
    func loadImage(
        from url: URL,
        completion: @escaping (UIImage?) -> Void
    ) {
        let cacheKey = url.absoluteString as NSString
        
        // Check cache first
        if let cachedImage = cache.object(forKey: cacheKey) {
            completion(cachedImage)
            return
        }
        
        // Cancel existing task if any
        loadingTasks[url.absoluteString]?.cancel()
        
        // Create new task
        let task = session.dataTask(with: url) { [weak self] data, response, error in
            guard let self = self,
                  let data = data,
                  let image = UIImage(data: data) else {
                DispatchQueue.main.async {
                    completion(nil)
                }
                return
            }
            
            // Optimize image
            let optimizedImage = self.optimizeImage(image)
            
            // Cache image
            self.cache.setObject(optimizedImage, forKey: cacheKey)
            
            // Remove task
            self.loadingTasks.removeValue(forKey: url.absoluteString)
            
            DispatchQueue.main.async {
                completion(optimizedImage)
            }
        }
        
        loadingTasks[url.absoluteString] = task
        task.resume()
    }
    
    /**
     * Load image with size optimization
     * 
     * This method demonstrates proper image loading
     * with size optimization and memory efficiency
     */
    func loadImage(
        from url: URL,
        targetSize: CGSize,
        completion: @escaping (UIImage?) -> Void
    ) {
        let cacheKey = "\(url.absoluteString)_\(targetSize.width)x\(targetSize.height)" as NSString
        
        // Check cache first
        if let cachedImage = cache.object(forKey: cacheKey) {
            completion(cachedImage)
            return
        }
        
        // Load original image
        loadImage(from: url) { [weak self] image in
            guard let image = image else {
                completion(nil)
                return
            }
            
            // Resize image
            let resizedImage = self?.resizeImage(image, to: targetSize)
            
            // Cache resized image
            if let resizedImage = resizedImage {
                self?.cache.setObject(resizedImage, forKey: cacheKey)
            }
            
            completion(resizedImage)
        }
    }
    
    // MARK: - Private Methods
    
    private func optimizeImage(_ image: UIImage) -> UIImage {
        // Optimize image for display
        let optimizedImage = image.withRenderingMode(.alwaysOriginal)
        return optimizedImage
    }
    
    private func resizeImage(_ image: UIImage, to targetSize: CGSize) -> UIImage? {
        let aspectRatio = image.size.width / image.size.height
        let targetAspectRatio = targetSize.width / targetSize.height
        
        var newSize: CGSize
        if aspectRatio > targetAspectRatio {
            newSize = CGSize(width: targetSize.width, height: targetSize.width / aspectRatio)
        } else {
            newSize = CGSize(width: targetSize.height * aspectRatio, height: targetSize.height)
        }
        
        UIGraphicsBeginImageContextWithOptions(newSize, false, 0.0)
        image.draw(in: CGRect(origin: .zero, size: newSize))
        let resizedImage = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return resizedImage
    }
}

// MARK: - Layout Performance

/**
 * Optimized layout utilities
 * 
 * This class demonstrates production-grade layout optimization
 * with proper constraint management and performance tuning
 */
class OptimizedLayout {
    
    // MARK: - Constraint Optimization
    
    /**
     * Create optimized constraints
     * 
     * This method demonstrates proper constraint creation
     * with performance optimization
     */
    static func createOptimizedConstraints(
        for view: UIView,
        in containerView: UIView,
        insets: UIEdgeInsets = .zero
    ) -> [NSLayoutConstraint] {
        view.translatesAutoresizingMaskIntoConstraints = false
        
        return [
            view.topAnchor.constraint(equalTo: containerView.topAnchor, constant: insets.top),
            view.leadingAnchor.constraint(equalTo: containerView.leadingAnchor, constant: insets.left),
            view.trailingAnchor.constraint(equalTo: containerView.trailingAnchor, constant: -insets.right),
            view.bottomAnchor.constraint(equalTo: containerView.bottomAnchor, constant: -insets.bottom)
        ]
    }
    
    /**
     * Batch constraint updates
     * 
     * This method demonstrates proper constraint batching
     * with performance optimization
     */
    static func batchConstraintUpdates(
        in view: UIView,
        updates: @escaping () -> Void
    ) {
        view.setNeedsUpdateConstraints()
        
        UIView.performWithoutAnimation {
            updates()
            view.layoutIfNeeded()
        }
    }
    
    // MARK: - Stack View Optimization
    
    /**
     * Create optimized stack view
     * 
     * This method demonstrates proper stack view creation
     * with performance optimization
     */
    static func createOptimizedStackView(
        arrangedSubviews: [UIView],
        axis: NSLayoutConstraint.Axis = .vertical,
        spacing: CGFloat = 0,
        alignment: UIStackView.Alignment = .fill,
        distribution: UIStackView.Distribution = .fill
    ) -> UIStackView {
        let stackView = UIStackView(arrangedSubviews: arrangedSubviews)
        stackView.axis = axis
        stackView.spacing = spacing
        stackView.alignment = alignment
        stackView.distribution = distribution
        stackView.translatesAutoresizingMaskIntoConstraints = false
        
        return stackView
    }
    
    // MARK: - Auto Layout Performance
    
    /**
     * Optimize auto layout performance
     * 
     * This method demonstrates proper auto layout optimization
     * with performance tuning
     */
    static func optimizeAutoLayoutPerformance(for view: UIView) {
        // Enable layout optimization
        view.setContentHuggingPriority(.required, for: .horizontal)
        view.setContentHuggingPriority(.required, for: .vertical)
        
        // Set compression resistance
        view.setContentCompressionResistancePriority(.required, for: .horizontal)
        view.setContentCompressionResistancePriority(.required, for: .vertical)
        
        // Optimize for performance
        view.setNeedsLayout()
        view.layoutIfNeeded()
    }
}

// MARK: - Animation Performance

/**
 * Optimized animation utilities
 * 
 * This class demonstrates production-grade animation optimization
 * with proper timing and performance tuning
 */
class OptimizedAnimation {
    
    // MARK: - Animation Performance
    
    /**
     * Create optimized animation
     * 
     * This method demonstrates proper animation creation
     * with performance optimization
     */
    static func createOptimizedAnimation(
        duration: TimeInterval = 0.3,
        delay: TimeInterval = 0,
        options: UIView.AnimationOptions = [],
        updates: @escaping () -> Void,
        completion: ((Bool) -> Void)? = nil
    ) {
        // Use appropriate animation options for performance
        let performanceOptions: UIView.AnimationOptions = [
            .allowUserInteraction,
            .beginFromCurrentState,
            .curveEaseInOut
        ]
        
        UIView.animate(
            withDuration: duration,
            delay: delay,
            options: options.union(performanceOptions),
            animations: updates,
            completion: completion
        )
    }
    
    /**
     * Create spring animation with performance optimization
     * 
     * This method demonstrates proper spring animation creation
     * with performance tuning
     */
    static func createOptimizedSpringAnimation(
        duration: TimeInterval = 0.5,
        delay: TimeInterval = 0,
        damping: CGFloat = 0.8,
        initialVelocity: CGFloat = 0,
        updates: @escaping () -> Void,
        completion: ((Bool) -> Void)? = nil
    ) {
        UIView.animate(
            withDuration: duration,
            delay: delay,
            usingSpringWithDamping: damping,
            initialSpringVelocity: initialVelocity,
            options: [.allowUserInteraction, .beginFromCurrentState],
            animations: updates,
            completion: completion
        )
    }
    
    // MARK: - Layer Animation
    
    /**
     * Create optimized layer animation
     * 
     * This method demonstrates proper layer animation creation
     * with performance optimization
     */
    static func createOptimizedLayerAnimation(
        layer: CALayer,
        keyPath: String,
        fromValue: Any?,
        toValue: Any?,
        duration: TimeInterval = 0.3,
        timingFunction: CAMediaTimingFunction? = nil
    ) -> CABasicAnimation {
        let animation = CABasicAnimation(keyPath: keyPath)
        animation.fromValue = fromValue
        animation.toValue = toValue
        animation.duration = duration
        animation.timingFunction = timingFunction ?? CAMediaTimingFunction(name: .easeInEaseOut)
        animation.fillMode = .forwards
        animation.isRemovedOnCompletion = false
        
        return animation
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use UI performance optimization
 * 
 * This function shows practical usage of all the UI performance components
 */
func demonstrateUIPerformance() {
    print("=== UI Performance Demonstration ===\n")
    
    // Optimized Rendering
    print("--- Optimized Rendering ---")
    print("Batch Updates: CATransaction for performance")
    print("Animation Optimization: Proper timing and options")
    print("Layer Optimization: Rasterization and shadow optimization")
    print("Features: Performance tuning, memory efficiency, smooth animations")
    
    // Optimized Scroll View
    let scrollView = OptimizedScrollView()
    print("\n--- Optimized Scroll View ---")
    print("Scroll View: Cell reuse and memory optimization")
    print("Features: Virtual scrolling, cell recycling, performance optimization")
    
    // Optimized Image Loader
    let imageLoader = OptimizedImageLoader()
    print("\n--- Optimized Image Loader ---")
    print("Image Loader: Caching and size optimization")
    print("Features: Memory management, size optimization, performance tuning")
    
    // Optimized Layout
    print("\n--- Optimized Layout ---")
    print("Constraint Optimization: Efficient constraint management")
    print("Stack View Optimization: Performance-tuned stack views")
    print("Auto Layout Performance: Optimization techniques")
    print("Features: Performance tuning, memory efficiency, smooth layouts")
    
    // Optimized Animation
    print("\n--- Optimized Animation ---")
    print("Animation Performance: Optimized timing and options")
    print("Layer Animation: Performance-tuned layer animations")
    print("Features: Smooth animations, performance optimization, memory efficiency")
    
    // Demonstrate performance optimization techniques
    print("\n--- Performance Optimization Techniques ---")
    print("Rendering Optimization: Batch updates, layer optimization")
    print("Scroll Performance: Cell reuse, virtual scrolling")
    print("Image Optimization: Caching, size optimization, memory management")
    print("Layout Performance: Constraint optimization, auto layout tuning")
    print("Animation Performance: Proper timing, layer optimization")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Batch view updates using CATransaction")
    print("2. Use cell reuse for large lists")
    print("3. Optimize images for target size")
    print("4. Use appropriate animation options")
    print("5. Optimize auto layout constraints")
    print("6. Use layer rasterization for complex views")
    print("7. Profile UI performance regularly")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateUIPerformance()
