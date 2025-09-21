/*
 * Swift Examples: Google Material Design Animations
 * 
 * This file demonstrates Google's Material Design animation patterns
 * used in production iOS applications, based on Google's own implementations.
 * 
 * Key Learning Objectives:
 * - Master Google's Material Design animation principles
 * - Understand Google's motion design and choreography
 * - Learn Google's animation performance optimization
 * - Apply Google's accessibility in animations
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Google Production Code Quality
 */

import Foundation
import UIKit
import SwiftUI
import CoreAnimation
import Combine

// MARK: - Google Material Animation Manager

/**
 * Google's Material Design animation manager
 * 
 * This class demonstrates Google's Material Design animation patterns
 * with comprehensive motion design and choreography
 */
class GoogleMaterialAnimationManager: NSObject, ObservableObject {
    
    // MARK: - Properties
    
    @Published var isAnimating = false
    @Published var animationProgress: Double = 0.0
    @Published var currentMotion: MaterialMotion?
    @Published var animationQueue: [MaterialAnimation] = []
    @Published var performanceMetrics: MaterialAnimationMetrics = MaterialAnimationMetrics()
    
    private var animationLayer: CALayer?
    private var displayLink: CADisplayLink?
    private var motionTimers: [Timer] = []
    private var animationCancellables = Set<AnyCancellable>()
    
    // MARK: - Initialization
    
    override init() {
        super.init()
        setupMaterialAnimationManager()
    }
    
    // MARK: - Public Methods
    
    /**
     * Animate with Material Design motion
     * 
     * This method demonstrates Google's Material Design motion
     * with comprehensive motion design principles
     */
    func animateWithMaterialMotion(
        view: UIView,
        motion: MaterialMotion,
        duration: TimeInterval = 0.3,
        easing: MaterialEasing = .standard
    ) -> AnyPublisher<MaterialAnimationResult, Error> {
        return Future<MaterialAnimationResult, Error> { promise in
            self.isAnimating = true
            self.currentMotion = motion
            
            let timingFunction = self.createTimingFunction(for: easing)
            
            UIView.animate(
                withDuration: duration,
                delay: 0,
                options: [.curveEaseInOut, .allowUserInteraction],
                animations: {
                    self.applyMaterialMotion(motion, to: view)
                },
                completion: { finished in
                    self.isAnimating = false
                    self.currentMotion = nil
                    promise(.success(MaterialAnimationResult(success: true, motion: motion, duration: duration)))
                }
            )
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Animate with Material Design choreography
     * 
     * This method demonstrates Google's Material Design choreography
     * with comprehensive motion coordination
     */
    func animateWithChoreography(
        views: [UIView],
        choreography: MaterialChoreography,
        duration: TimeInterval = 0.5
    ) -> AnyPublisher<MaterialAnimationResult, Error> {
        return Future<MaterialAnimationResult, Error> { promise in
            self.isAnimating = true
            
            let group = DispatchGroup()
            var results: [MaterialAnimationResult] = []
            
            for (index, view) in views.enumerated() {
                group.enter()
                
                let delay = choreography.delays[index] ?? 0
                let motion = choreography.motions[index]
                
                DispatchQueue.main.asyncAfter(deadline: .now() + delay) {
                    self.animateWithMaterialMotion(view: view, motion: motion, duration: duration)
                        .sink(
                            receiveCompletion: { completion in
                                if case .failure(let error) = completion {
                                    results.append(MaterialAnimationResult(success: false, motion: motion, duration: duration, error: error))
                                }
                                group.leave()
                            },
                            receiveValue: { result in
                                results.append(result)
                                group.leave()
                            }
                        )
                        .store(in: &self.animationCancellables)
                }
            }
            
            group.notify(queue: .main) {
                self.isAnimating = false
                let success = results.allSatisfy { $0.success }
                promise(.success(MaterialAnimationResult(success: success, motion: .fade, duration: duration)))
            }
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Animate with Material Design elevation
     * 
     * This method demonstrates Google's Material Design elevation
     * with comprehensive shadow and depth animation
     */
    func animateElevation(
        view: UIView,
        from elevation: MaterialElevation,
        to targetElevation: MaterialElevation,
        duration: TimeInterval = 0.2
    ) -> AnyPublisher<MaterialAnimationResult, Error> {
        return Future<MaterialAnimationResult, Error> { promise in
            self.isAnimating = true
            
            let elevationAnimation = CABasicAnimation(keyPath: "shadowOpacity")
            elevationAnimation.fromValue = elevation.shadowOpacity
            elevationAnimation.toValue = targetElevation.shadowOpacity
            elevationAnimation.duration = duration
            elevationAnimation.timingFunction = CAMediaTimingFunction(name: .easeInEaseOut)
            
            let scaleAnimation = CABasicAnimation(keyPath: "transform.scale")
            scaleAnimation.fromValue = elevation.scale
            scaleAnimation.toValue = targetElevation.scale
            scaleAnimation.duration = duration
            scaleAnimation.timingFunction = CAMediaTimingFunction(name: .easeInEaseOut)
            
            let group = CAAnimationGroup()
            group.animations = [elevationAnimation, scaleAnimation]
            group.duration = duration
            
            CATransaction.begin()
            CATransaction.setCompletionBlock {
                self.isAnimating = false
                promise(.success(MaterialAnimationResult(success: true, motion: .elevation, duration: duration)))
            }
            
            view.layer.add(group, forKey: "elevation")
            CATransaction.commit()
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Animate with Material Design ripple
     * 
     * This method demonstrates Google's Material Design ripple effect
     * with comprehensive touch feedback animation
     */
    func animateRipple(
        view: UIView,
        at point: CGPoint,
        color: UIColor = .systemBlue,
        duration: TimeInterval = 0.3
    ) -> AnyPublisher<MaterialAnimationResult, Error> {
        return Future<MaterialAnimationResult, Error> { promise in
            let rippleLayer = self.createRippleLayer(at: point, in: view, color: color)
            view.layer.addSublayer(rippleLayer)
            
            let scaleAnimation = CABasicAnimation(keyPath: "transform.scale")
            scaleAnimation.fromValue = 0.0
            scaleAnimation.toValue = 1.0
            scaleAnimation.duration = duration
            scaleAnimation.timingFunction = CAMediaTimingFunction(name: .easeOut)
            
            let opacityAnimation = CABasicAnimation(keyPath: "opacity")
            opacityAnimation.fromValue = 0.6
            opacityAnimation.toValue = 0.0
            opacityAnimation.duration = duration
            opacityAnimation.timingFunction = CAMediaTimingFunction(name: .easeOut)
            
            let group = CAAnimationGroup()
            group.animations = [scaleAnimation, opacityAnimation]
            group.duration = duration
            
            CATransaction.begin()
            CATransaction.setCompletionBlock {
                rippleLayer.removeFromSuperlayer()
                promise(.success(MaterialAnimationResult(success: true, motion: .ripple, duration: duration)))
            }
            
            rippleLayer.add(group, forKey: "ripple")
            CATransaction.commit()
        }
        .eraseToAnyPublisher()
    }
    
    /**
     * Animate with Material Design transition
     * 
     * This method demonstrates Google's Material Design transitions
     * with comprehensive shared element transitions
     */
    func animateTransition(
        from sourceView: UIView,
        to destinationView: UIView,
        transition: MaterialTransition,
        duration: TimeInterval = 0.4
    ) -> AnyPublisher<MaterialAnimationResult, Error> {
        return Future<MaterialAnimationResult, Error> { promise in
            self.isAnimating = true
            
            let sourceFrame = sourceView.convert(sourceView.bounds, to: nil)
            let destinationFrame = destinationView.convert(destinationView.bounds, to: nil)
            
            let transitionView = UIView(frame: sourceFrame)
            transitionView.backgroundColor = sourceView.backgroundColor
            transitionView.layer.cornerRadius = sourceView.layer.cornerRadius
            
            if let sourceImage = sourceView as? UIImageView {
                let imageView = UIImageView(image: sourceImage.image)
                imageView.contentMode = sourceImage.contentMode
                transitionView.addSubview(imageView)
                imageView.frame = transitionView.bounds
            }
            
            UIApplication.shared.windows.first?.addSubview(transitionView)
            
            UIView.animate(
                withDuration: duration,
                delay: 0,
                usingSpringWithDamping: 0.8,
                initialSpringVelocity: 0.2,
                options: [.curveEaseInOut],
                animations: {
                    transitionView.frame = destinationFrame
                    transitionView.layer.cornerRadius = destinationView.layer.cornerRadius
                },
                completion: { finished in
                    transitionView.removeFromSuperview()
                    self.isAnimating = false
                    promise(.success(MaterialAnimationResult(success: true, motion: .transition, duration: duration)))
                }
            )
        }
        .eraseToAnyPublisher()
    }
    
    // MARK: - Private Methods
    
    private func setupMaterialAnimationManager() {
        setupDisplayLink()
        setupPerformanceMonitoring()
    }
    
    private func setupDisplayLink() {
        displayLink = CADisplayLink(target: self, selector: #selector(displayLinkTick))
        displayLink?.add(to: .main, forMode: .common)
    }
    
    @objc private func displayLinkTick() {
        updateAnimationProgress()
        updatePerformanceMetrics()
    }
    
    private func updateAnimationProgress() {
        // Calculate current animation progress
        // This would be implemented based on current animations
    }
    
    private func updatePerformanceMetrics() {
        let frameRate = displayLink?.preferredFramesPerSecond ?? 60
        performanceMetrics = MaterialAnimationMetrics(
            frameRate: Double(frameRate),
            averageFrameTime: 1.0 / Double(frameRate),
            droppedFrames: 0,
            memoryUsage: getCurrentMemoryUsage()
        )
    }
    
    private func getCurrentMemoryUsage() -> Int64 {
        var info = mach_task_basic_info()
        var count = mach_msg_type_number_t(MemoryLayout<mach_task_basic_info>.size)/4
        
        let kerr: kern_return_t = withUnsafeMutablePointer(to: &info) {
            $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                task_info(mach_task_self_,
                         task_flavor_t(MACH_TASK_BASIC_INFO),
                         $0,
                         &count)
            }
        }
        
        return kerr == KERN_SUCCESS ? Int64(info.resident_size) : 0
    }
    
    private func createTimingFunction(for easing: MaterialEasing) -> CAMediaTimingFunction {
        switch easing {
        case .standard:
            return CAMediaTimingFunction(name: .easeInEaseOut)
        case .decelerate:
            return CAMediaTimingFunction(name: .easeOut)
        case .accelerate:
            return CAMediaTimingFunction(name: .easeIn)
        case .sharp:
            return CAMediaTimingFunction(controlPoints: 0.4, 0.0, 0.6, 1.0)
        case .custom(let c1x, let c1y, let c2x, let c2y):
            return CAMediaTimingFunction(controlPoints: Float(c1x), Float(c1y), Float(c2x), Float(c2y))
        }
    }
    
    private func applyMaterialMotion(_ motion: MaterialMotion, to view: UIView) {
        switch motion {
        case .fade(let alpha):
            view.alpha = alpha
        case .scale(let scale):
            view.transform = CGAffineTransform(scaleX: scale, y: scale)
        case .slide(let translation):
            view.transform = CGAffineTransform(translationX: translation.x, y: translation.y)
        case .rotate(let angle):
            view.transform = CGAffineTransform(rotationAngle: angle)
        case .elevation(let elevation):
            view.layer.shadowOpacity = elevation.shadowOpacity
            view.layer.shadowRadius = elevation.shadowRadius
            view.layer.shadowOffset = elevation.shadowOffset
        case .ripple:
            // Ripple is handled separately
            break
        case .transition:
            // Transition is handled separately
            break
        }
    }
    
    private func createRippleLayer(at point: CGPoint, in view: UIView, color: UIColor) -> CALayer {
        let rippleLayer = CALayer()
        rippleLayer.backgroundColor = color.cgColor
        rippleLayer.frame = CGRect(x: point.x - 25, y: point.y - 25, width: 50, height: 50)
        rippleLayer.cornerRadius = 25
        return rippleLayer
    }
}

// MARK: - Google Material Design Accessibility

/**
 * Google's Material Design accessibility manager
 * 
 * This class demonstrates Google's Material Design accessibility
 * with comprehensive accessibility animation support
 */
class GoogleMaterialAccessibilityManager: ObservableObject {
    
    // MARK: - Properties
    
    @Published var isAccessibilityEnabled = false
    @Published var reducedMotionEnabled = false
    @Published var highContrastEnabled = false
    @Published var largeTextEnabled = false
    
    // MARK: - Initialization
    
    init() {
        setupAccessibilityManager()
    }
    
    // MARK: - Public Methods
    
    /**
     * Check accessibility preferences
     * 
     * This method demonstrates Google's accessibility preference checking
     * with comprehensive accessibility support
     */
    func checkAccessibilityPreferences() {
        isAccessibilityEnabled = UIAccessibility.isVoiceOverRunning
        reducedMotionEnabled = UIAccessibility.isReduceMotionEnabled
        highContrastEnabled = UIAccessibility.isIncreaseContrastEnabled
        largeTextEnabled = UIAccessibility.isBoldTextEnabled
    }
    
    /**
     * Adapt animation for accessibility
     * 
     * This method demonstrates Google's accessibility animation adaptation
     * with comprehensive accessibility support
     */
    func adaptAnimationForAccessibility(
        _ animation: MaterialAnimation,
        duration: TimeInterval
    ) -> MaterialAnimation {
        var adaptedAnimation = animation
        
        if reducedMotionEnabled {
            adaptedAnimation.duration = min(duration, 0.2) // Reduce duration for reduced motion
        }
        
        if highContrastEnabled {
            // Enhance contrast in animations
            adaptedAnimation.contrastMultiplier = 1.2
        }
        
        if largeTextEnabled {
            // Adjust animation scale for large text
            adaptedAnimation.scaleMultiplier = 1.1
        }
        
        return adaptedAnimation
    }
    
    // MARK: - Private Methods
    
    private func setupAccessibilityManager() {
        checkAccessibilityPreferences()
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.voiceOverStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.checkAccessibilityPreferences()
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.reduceMotionStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.checkAccessibilityPreferences()
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.increaseContrastStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.checkAccessibilityPreferences()
        }
        
        NotificationCenter.default.addObserver(
            forName: UIAccessibility.boldTextStatusDidChangeNotification,
            object: nil,
            queue: .main
        ) { _ in
            self.checkAccessibilityPreferences()
        }
    }
}

// MARK: - Supporting Types

/**
 * Material motion
 * 
 * This enum demonstrates proper Material Design motion modeling
 * for Google's animation system
 */
enum MaterialMotion {
    case fade(CGFloat)
    case scale(CGFloat)
    case slide(CGPoint)
    case rotate(CGFloat)
    case elevation(MaterialElevation)
    case ripple
    case transition
}

/**
 * Material easing
 * 
 * This enum demonstrates proper Material Design easing modeling
 * for Google's animation system
 */
enum MaterialEasing {
    case standard
    case decelerate
    case accelerate
    case sharp
    case custom(Double, Double, Double, Double)
}

/**
 * Material elevation
 * 
 * This struct demonstrates proper Material Design elevation modeling
 * for Google's animation system
 */
struct MaterialElevation {
    let level: Int
    let shadowOpacity: Float
    let shadowRadius: CGFloat
    let shadowOffset: CGSize
    let scale: CGFloat
    
    static let level0 = MaterialElevation(level: 0, shadowOpacity: 0.0, shadowRadius: 0, shadowOffset: .zero, scale: 1.0)
    static let level1 = MaterialElevation(level: 1, shadowOpacity: 0.2, shadowRadius: 1, shadowOffset: CGSize(width: 0, height: 1), scale: 1.0)
    static let level2 = MaterialElevation(level: 2, shadowOpacity: 0.14, shadowRadius: 2, shadowOffset: CGSize(width: 0, height: 2), scale: 1.0)
    static let level4 = MaterialElevation(level: 4, shadowOpacity: 0.12, shadowRadius: 4, shadowOffset: CGSize(width: 0, height: 4), scale: 1.0)
    static let level8 = MaterialElevation(level: 8, shadowOpacity: 0.1, shadowRadius: 8, shadowOffset: CGSize(width: 0, height: 8), scale: 1.0)
    static let level16 = MaterialElevation(level: 16, shadowOpacity: 0.08, shadowRadius: 16, shadowOffset: CGSize(width: 0, height: 16), scale: 1.0)
    static let level24 = MaterialElevation(level: 24, shadowOpacity: 0.06, shadowRadius: 24, shadowOffset: CGSize(width: 0, height: 24), scale: 1.0)
}

/**
 * Material choreography
 * 
 * This struct demonstrates proper Material Design choreography modeling
 * for Google's animation system
 */
struct MaterialChoreography {
    let motions: [MaterialMotion]
    let delays: [TimeInterval?]
    let duration: TimeInterval
    let easing: MaterialEasing
}

/**
 * Material transition
 * 
 * This enum demonstrates proper Material Design transition modeling
 * for Google's animation system
 */
enum MaterialTransition {
    case sharedElement
    case fade
    case slide
    case scale
    case custom(String)
}

/**
 * Material animation
 * 
 * This struct demonstrates proper Material Design animation modeling
 * for Google's animation system
 */
struct MaterialAnimation: Identifiable {
    let id = UUID()
    let motion: MaterialMotion
    let duration: TimeInterval
    let easing: MaterialEasing
    let delay: TimeInterval
    var contrastMultiplier: CGFloat = 1.0
    var scaleMultiplier: CGFloat = 1.0
}

/**
 * Material animation result
 * 
 * This struct demonstrates proper Material Design animation result modeling
 * for Google's animation system
 */
struct MaterialAnimationResult {
    let success: Bool
    let motion: MaterialMotion
    let duration: TimeInterval
    let error: Error?
}

/**
 * Material animation metrics
 * 
 * This struct demonstrates proper Material Design animation metrics modeling
 * for Google's animation system
 */
struct MaterialAnimationMetrics {
    let frameRate: Double
    let averageFrameTime: TimeInterval
    let droppedFrames: Int
    let memoryUsage: Int64
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use Google Material Design animations
 * 
 * This function shows practical usage of all the Google Material animation components
 */
func demonstrateGoogleMaterialAnimations() {
    print("=== Google Material Design Animations Demonstration ===\n")
    
    // Material Animation Manager
    let materialAnimationManager = GoogleMaterialAnimationManager()
    print("--- Material Animation Manager ---")
    print("Material Animation Manager: \(type(of: materialAnimationManager))")
    print("Features: Material motion, choreography, elevation, ripple, transitions")
    
    // Accessibility Manager
    let accessibilityManager = GoogleMaterialAccessibilityManager()
    print("\n--- Accessibility Manager ---")
    print("Accessibility Manager: \(type(of: accessibilityManager))")
    print("Features: Accessibility adaptation, reduced motion, high contrast, large text")
    
    // Demonstrate features
    print("\n--- Features ---")
    print("Material Motion: Fade, scale, slide, rotate, elevation animations")
    print("Choreography: Coordinated animations with delays and easing")
    print("Elevation: Shadow and depth animations")
    print("Ripple: Touch feedback animations")
    print("Transitions: Shared element transitions")
    print("Accessibility: Reduced motion and accessibility adaptations")
    
    // Demonstrate best practices
    print("\n--- Best Practices ---")
    print("1. Follow Material Design motion principles")
    print("2. Use appropriate easing curves for natural feel")
    print("3. Implement proper accessibility support")
    print("4. Use elevation animations for depth perception")
    print("5. Implement ripple effects for touch feedback")
    print("6. Use choreography for coordinated animations")
    print("7. Test with various accessibility settings")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateGoogleMaterialAnimations()
