/*
 * Advanced Swift: Protocols
 * 
 * This file demonstrates production-grade protocol-oriented programming patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master protocol-oriented programming (POP) design patterns
 * - Understand protocol extensions and default implementations
 * - Implement protocol composition and inheritance
 * - Apply advanced protocol techniques for flexible architecture
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Protocol Fundamentals

/**
 * Demonstrates fundamental protocol patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic protocol definition and conformance
 * - Protocol requirements and optional methods
 * - Protocol inheritance and composition
 * - Protocol extensions and default implementations
 */
class ProtocolFundamentals {
    
    // MARK: - Basic Protocol Definition
    
    /**
     * Protocol for objects that can be drawn
     * 
     * This protocol demonstrates basic protocol definition with required methods
     */
    protocol Drawable {
        func draw()
        func draw(at point: CGPoint)
        func draw(in rect: CGRect)
    }
    
    /**
     * Protocol for objects that can be animated
     * 
     * This protocol demonstrates protocol with properties and methods
     */
    protocol Animatable {
        var isAnimating: Bool { get }
        var animationDuration: TimeInterval { get set }
        
        func startAnimation()
        func stopAnimation()
        func pauseAnimation()
        func resumeAnimation()
    }
    
    /**
     * Protocol for objects that can be serialized
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Serializable {
        associatedtype DataType
        
        func serialize() -> DataType
        static func deserialize(from data: DataType) -> Self?
    }
    
    // MARK: - Protocol Conformance
    
    /**
     * Circle class that conforms to Drawable protocol
     * 
     * This class demonstrates basic protocol conformance
     */
    class Circle: Drawable {
        let radius: Double
        let center: CGPoint
        
        init(radius: Double, center: CGPoint) {
            self.radius = radius
            self.center = center
        }
        
        func draw() {
            print("Drawing circle with radius \(radius) at \(center)")
        }
        
        func draw(at point: CGPoint) {
            print("Drawing circle with radius \(radius) at \(point)")
        }
        
        func draw(in rect: CGRect) {
            print("Drawing circle with radius \(radius) in \(rect)")
        }
    }
    
    /**
     * Rectangle class that conforms to Drawable protocol
     * 
     * This class demonstrates protocol conformance with different implementations
     */
    class Rectangle: Drawable {
        let width: Double
        let height: Double
        let origin: CGPoint
        
        init(width: Double, height: Double, origin: CGPoint) {
            self.width = width
            self.height = height
            self.origin = origin
        }
        
        func draw() {
            print("Drawing rectangle \(width)x\(height) at \(origin)")
        }
        
        func draw(at point: CGPoint) {
            print("Drawing rectangle \(width)x\(height) at \(point)")
        }
        
        func draw(in rect: CGRect) {
            print("Drawing rectangle \(width)x\(height) in \(rect)")
        }
    }
    
    /**
     * Animated circle class that conforms to multiple protocols
     * 
     * This class demonstrates multiple protocol conformance
     */
    class AnimatedCircle: Drawable, Animatable {
        let radius: Double
        let center: CGPoint
        var animationDuration: TimeInterval = 1.0
        private var _isAnimating: Bool = false
        
        var isAnimating: Bool {
            return _isAnimating
        }
        
        init(radius: Double, center: CGPoint) {
            self.radius = radius
            self.center = center
        }
        
        func draw() {
            print("Drawing animated circle with radius \(radius) at \(center)")
        }
        
        func draw(at point: CGPoint) {
            print("Drawing animated circle with radius \(radius) at \(point)")
        }
        
        func draw(in rect: CGRect) {
            print("Drawing animated circle with radius \(radius) in \(rect)")
        }
        
        func startAnimation() {
            _isAnimating = true
            print("Starting animation for \(animationDuration) seconds")
        }
        
        func stopAnimation() {
            _isAnimating = false
            print("Stopping animation")
        }
        
        func pauseAnimation() {
            print("Pausing animation")
        }
        
        func resumeAnimation() {
            print("Resuming animation")
        }
    }
}

// MARK: - Protocol Extensions

/**
 * Demonstrates protocol extensions used in production iOS apps
 * 
 * This class covers:
 * - Default implementations for protocol methods
 * - Protocol extensions with constraints
 * - Protocol extensions with computed properties
 * - Protocol extensions with static methods
 */
class ProtocolExtensions {
    
    // MARK: - Basic Protocol Extensions
    
    /**
     * Protocol for objects that can be validated
     * 
     * This protocol demonstrates basic protocol definition
     */
    protocol Validatable {
        func validate() -> Bool
        func validate(completion: @escaping (Bool) -> Void)
    }
    
    /**
     * Protocol extension providing default implementation
     * 
     * This extension demonstrates default implementations for protocol methods
     */
    extension Validatable {
        func validate(completion: @escaping (Bool) -> Void) {
            DispatchQueue.global(qos: .userInitiated).async {
                let isValid = self.validate()
                DispatchQueue.main.async {
                    completion(isValid)
                }
            }
        }
    }
    
    /**
     * Protocol for objects that can be formatted
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Formattable {
        associatedtype FormatType
        
        func format() -> FormatType
        func format(with options: FormatOptions) -> FormatType
    }
    
    /**
     * Format options for formatting
     * 
     * This struct demonstrates configuration for protocol methods
     */
    struct FormatOptions {
        let includeTimestamp: Bool
        let includeMetadata: Bool
        let precision: Int
        
        init(includeTimestamp: Bool = false, includeMetadata: Bool = false, precision: Int = 2) {
            self.includeTimestamp = includeTimestamp
            self.includeMetadata = includeMetadata
            self.precision = precision
        }
    }
    
    /**
     * Protocol extension with default implementation
     * 
     * This extension demonstrates default implementations with parameters
     */
    extension Formattable {
        func format() -> String {
            return format(with: FormatOptions())
        }
    }
    
    // MARK: - Protocol Extensions with Constraints
    
    /**
     * Protocol for objects that can be compared
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Comparable {
        associatedtype ComparableType: Swift.Comparable
        
        func compare(to other: ComparableType) -> ComparisonResult
    }
    
    /**
     * Protocol extension with type constraints
     * 
     * This extension demonstrates protocol extensions with where clauses
     */
    extension Comparable where ComparableType == Self {
        func isEqual(to other: Self) -> Bool {
            return compare(to: other) == .orderedSame
        }
        
        func isLessThan(_ other: Self) -> Bool {
            return compare(to: other) == .orderedAscending
        }
        
        func isGreaterThan(_ other: Self) -> Bool {
            return compare(to: other) == .orderedDescending
        }
    }
    
    /**
     * Protocol for objects that can be converted to strings
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol StringConvertible {
        associatedtype StringType: CustomStringConvertible
        
        func toString() -> StringType
    }
    
    /**
     * Protocol extension with type constraints
     * 
     * This extension demonstrates protocol extensions with where clauses
     */
    extension StringConvertible where StringType == String {
        func description() -> String {
            return toString()
        }
    }
    
    // MARK: - Protocol Extensions with Computed Properties
    
    /**
     * Protocol for objects that have dimensions
     * 
     * This protocol demonstrates protocol with properties
     */
    protocol Dimensional {
        var width: Double { get }
        var height: Double { get }
    }
    
    /**
     * Protocol extension with computed properties
     * 
     * This extension demonstrates computed properties in protocol extensions
     */
    extension Dimensional {
        var area: Double {
            return width * height
        }
        
        var perimeter: Double {
            return 2 * (width + height)
        }
        
        var aspectRatio: Double {
            return width / height
        }
    }
    
    /**
     * Protocol for objects that can be positioned
     * 
     * This protocol demonstrates protocol with properties
     */
    protocol Positionable {
        var x: Double { get }
        var y: Double { get }
    }
    
    /**
     * Protocol extension with computed properties
     * 
     * This extension demonstrates computed properties in protocol extensions
     */
    extension Positionable {
        var position: CGPoint {
            return CGPoint(x: x, y: y)
        }
        
        func distance(to other: Positionable) -> Double {
            let dx = x - other.x
            let dy = y - other.y
            return sqrt(dx * dx + dy * dy)
        }
    }
    
    // MARK: - Protocol Extensions with Static Methods
    
    /**
     * Protocol for objects that can be created from data
     * 
     * This protocol demonstrates protocol with static methods
     */
    protocol Creatable {
        associatedtype DataType
        
        static func create(from data: DataType) -> Self?
        static func create(from data: DataType, completion: @escaping (Self?) -> Void)
    }
    
    /**
     * Protocol extension with static methods
     * 
     * This extension demonstrates static methods in protocol extensions
     */
    extension Creatable {
        static func create(from data: DataType, completion: @escaping (Self?) -> Void) {
            DispatchQueue.global(qos: .userInitiated).async {
                let result = create(from: data)
                DispatchQueue.main.async {
                    completion(result)
                }
            }
        }
    }
}

// MARK: - Protocol Composition

/**
 * Demonstrates protocol composition patterns used in production iOS apps
 * 
 * This class covers:
 * - Protocol composition with & operator
 * - Protocol composition in function parameters
 * - Protocol composition in type definitions
 * - Protocol composition with constraints
 */
class ProtocolComposition {
    
    // MARK: - Basic Protocol Composition
    
    /**
     * Protocol for objects that can be drawn
     * 
     * This protocol demonstrates basic protocol definition
     */
    protocol Drawable {
        func draw()
    }
    
    /**
     * Protocol for objects that can be animated
     * 
     * This protocol demonstrates basic protocol definition
     */
    protocol Animatable {
        func animate()
    }
    
    /**
     * Protocol for objects that can be positioned
     * 
     * This protocol demonstrates basic protocol definition
     */
    protocol Positionable {
        var position: CGPoint { get set }
    }
    
    /**
     * Protocol for objects that can be sized
     * 
     * This protocol demonstrates basic protocol definition
     */
    protocol Sizable {
        var size: CGSize { get set }
    }
    
    /**
     * Function that takes multiple protocol types
     * 
     * - Parameter drawable: Object that conforms to Drawable protocol
     * 
     * This function demonstrates protocol composition in function parameters
     */
    func drawObject(_ drawable: Drawable) {
        drawable.draw()
    }
    
    /**
     * Function that takes multiple protocol types
     * 
     * - Parameter drawable: Object that conforms to Drawable protocol
     * - Parameter animatable: Object that conforms to Animatable protocol
     * 
     * This function demonstrates multiple protocol parameters
     */
    func drawAndAnimate(_ drawable: Drawable, _ animatable: Animatable) {
        drawable.draw()
        animatable.animate()
    }
    
    /**
     * Function that takes protocol composition
     * 
     * - Parameter object: Object that conforms to both Drawable and Animatable protocols
     * 
     * This function demonstrates protocol composition with & operator
     */
    func drawAndAnimateObject(_ object: Drawable & Animatable) {
        object.draw()
        object.animate()
    }
    
    /**
     * Function that takes complex protocol composition
     * 
     * - Parameter object: Object that conforms to multiple protocols
     * 
     * This function demonstrates complex protocol composition
     */
    func drawAnimatedPositionedObject(_ object: Drawable & Animatable & Positionable) {
        print("Drawing at position: \(object.position)")
        object.draw()
        object.animate()
    }
    
    // MARK: - Protocol Composition in Type Definitions
    
    /**
     * Type alias for protocol composition
     * 
     * This type alias demonstrates protocol composition in type definitions
     */
    typealias DrawableAnimatable = Drawable & Animatable
    
    /**
     * Type alias for complex protocol composition
     * 
     * This type alias demonstrates complex protocol composition
     */
    typealias DrawableAnimatablePositionable = Drawable & Animatable & Positionable
    
    /**
     * Function that uses type alias for protocol composition
     * 
     * - Parameter object: Object that conforms to DrawableAnimatable protocol
     * 
     * This function demonstrates using type aliases for protocol composition
     */
    func processDrawableAnimatable(_ object: DrawableAnimatable) {
        object.draw()
        object.animate()
    }
    
    /**
     * Function that uses complex type alias for protocol composition
     * 
     * - Parameter object: Object that conforms to DrawableAnimatablePositionable protocol
     * 
     * This function demonstrates using complex type aliases for protocol composition
     */
    func processDrawableAnimatablePositionable(_ object: DrawableAnimatablePositionable) {
        print("Processing at position: \(object.position)")
        object.draw()
        object.animate()
    }
    
    // MARK: - Protocol Composition with Constraints
    
    /**
     * Protocol for objects that can be compared
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Comparable {
        associatedtype ComparableType: Swift.Comparable
        
        func compare(to other: ComparableType) -> ComparisonResult
    }
    
    /**
     * Protocol for objects that can be hashed
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Hashable {
        associatedtype HashableType: Swift.Hashable
        
        func hashValue() -> Int
    }
    
    /**
     * Function that takes protocol composition with constraints
     * 
     * - Parameter object: Object that conforms to both Comparable and Hashable protocols
     * 
     * This function demonstrates protocol composition with where clauses
     */
    func processComparableHashable<T>(_ object: T) where T: Comparable, T: Hashable, T.ComparableType == T, T.HashableType == T {
        print("Hash value: \(object.hashValue())")
        print("Comparing to self: \(object.compare(to: object))")
    }
}

// MARK: - Protocol Inheritance

/**
 * Demonstrates protocol inheritance patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic protocol inheritance
 * - Protocol inheritance with method overriding
 * - Protocol inheritance with property overriding
 * - Protocol inheritance with associated types
 */
class ProtocolInheritance {
    
    // MARK: - Basic Protocol Inheritance
    
    /**
     * Base protocol for all vehicles
     * 
     * This protocol demonstrates basic protocol definition
     */
    protocol Vehicle {
        var speed: Double { get set }
        var maxSpeed: Double { get }
        
        func start()
        func stop()
        func accelerate(by amount: Double)
        func decelerate(by amount: Double)
    }
    
    /**
     * Protocol for land vehicles
     * 
     * This protocol demonstrates protocol inheritance
     */
    protocol LandVehicle: Vehicle {
        var wheels: Int { get }
        var fuelType: String { get }
        
        func refuel()
        func checkTirePressure()
    }
    
    /**
     * Protocol for water vehicles
     * 
     * This protocol demonstrates protocol inheritance
     */
    protocol WaterVehicle: Vehicle {
        var displacement: Double { get }
        var hullType: String { get }
        
        func anchor()
        func raiseAnchor()
    }
    
    /**
     * Protocol for air vehicles
     * 
     * This protocol demonstrates protocol inheritance
     */
    protocol AirVehicle: Vehicle {
        var altitude: Double { get set }
        var maxAltitude: Double { get }
        
        func takeOff()
        func land()
        func climb(by amount: Double)
        func descend(by amount: Double)
    }
    
    // MARK: - Protocol Inheritance with Method Overriding
    
    /**
     * Protocol for electric vehicles
     * 
     * This protocol demonstrates protocol inheritance with method overriding
     */
    protocol ElectricVehicle: LandVehicle {
        var batteryLevel: Double { get set }
        var maxBatteryLevel: Double { get }
        
        func charge()
        func checkBatteryLevel()
    }
    
    /**
     * Protocol extension providing default implementation for ElectricVehicle
     * 
     * This extension demonstrates default implementations in protocol inheritance
     */
    extension ElectricVehicle {
        func refuel() {
            charge()
        }
        
        func checkTirePressure() {
            print("Checking tire pressure for electric vehicle")
        }
    }
    
    /**
     * Protocol for hybrid vehicles
     * 
     * This protocol demonstrates protocol inheritance with multiple protocols
     */
    protocol HybridVehicle: LandVehicle {
        var electricMode: Bool { get set }
        var fuelLevel: Double { get set }
        var maxFuelLevel: Double { get }
        
        func switchToElectric()
        func switchToFuel()
        func checkFuelLevel()
    }
    
    /**
     * Protocol extension providing default implementation for HybridVehicle
     * 
     * This extension demonstrates default implementations in protocol inheritance
     */
    extension HybridVehicle {
        func refuel() {
            if electricMode {
                print("Charging battery")
            } else {
                print("Refueling with gasoline")
            }
        }
        
        func checkTirePressure() {
            print("Checking tire pressure for hybrid vehicle")
        }
    }
    
    // MARK: - Protocol Inheritance with Associated Types
    
    /**
     * Protocol for data sources
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol DataSource {
        associatedtype DataType
        
        func fetchData() -> [DataType]
        func fetchData(completion: @escaping ([DataType]) -> Void)
    }
    
    /**
     * Protocol for local data sources
     * 
     * This protocol demonstrates protocol inheritance with associated types
     */
    protocol LocalDataSource: DataSource {
        func saveData(_ data: [DataType])
        func deleteData(at index: Int)
        func updateData(at index: Int, with newData: DataType)
    }
    
    /**
     * Protocol for remote data sources
     * 
     * This protocol demonstrates protocol inheritance with associated types
     */
    protocol RemoteDataSource: DataSource {
        var baseURL: URL { get }
        var apiKey: String { get }
        
        func fetchData(from endpoint: String, completion: @escaping (Result<[DataType], Error>) -> Void)
        func postData(_ data: [DataType], to endpoint: String, completion: @escaping (Result<Void, Error>) -> Void)
    }
    
    /**
     * Protocol for cached data sources
     * 
     * This protocol demonstrates protocol inheritance with multiple protocols
     */
    protocol CachedDataSource: LocalDataSource, RemoteDataSource {
        var cacheExpirationTime: TimeInterval { get }
        var lastCacheUpdate: Date? { get }
        
        func isCacheValid() -> Bool
        func clearCache()
        func refreshCache(completion: @escaping (Result<Void, Error>) -> Void)
    }
}

// MARK: - Advanced Protocol Patterns

/**
 * Demonstrates advanced protocol patterns used in production iOS apps
 * 
 * This class covers:
 * - Protocol with associated types and constraints
 * - Protocol with generic requirements
 * - Protocol with conditional conformance
 * - Protocol with existential types
 */
class AdvancedProtocolPatterns {
    
    // MARK: - Protocol with Associated Types and Constraints
    
    /**
     * Protocol for collections that can be searched
     * 
     * This protocol demonstrates associated types with constraints
     */
    protocol SearchableCollection {
        associatedtype Element: Equatable
        associatedtype Index: Comparable
        
        var count: Int { get }
        var isEmpty: Bool { get }
        
        func index(of element: Element) -> Index?
        func contains(_ element: Element) -> Bool
        func first(where predicate: (Element) -> Bool) -> Element?
    }
    
    /**
     * Protocol for collections that can be sorted
     * 
     * This protocol demonstrates associated types with constraints
     */
    protocol SortableCollection {
        associatedtype Element: Comparable
        associatedtype Index: Comparable
        
        var count: Int { get }
        var isEmpty: Bool { get }
        
        mutating func sort()
        func sorted() -> Self
        func min() -> Element?
        func max() -> Element?
    }
    
    /**
     * Protocol for collections that can be both searched and sorted
     * 
     * This protocol demonstrates protocol composition with associated types
     */
    protocol SearchableSortableCollection: SearchableCollection, SortableCollection {
        associatedtype Element: Comparable & Equatable
    }
    
    // MARK: - Protocol with Generic Requirements
    
    /**
     * Protocol for objects that can be converted to other types
     * 
     * This protocol demonstrates generic requirements
     */
    protocol Convertible {
        associatedtype TargetType
        
        func convert() -> TargetType
        func convert(completion: @escaping (TargetType) -> Void)
    }
    
    /**
     * Protocol extension providing default implementation for Convertible
     * 
     * This extension demonstrates default implementations with generics
     */
    extension Convertible {
        func convert(completion: @escaping (TargetType) -> Void) {
            DispatchQueue.global(qos: .userInitiated).async {
                let result = self.convert()
                DispatchQueue.main.async {
                    completion(result)
                }
            }
        }
    }
    
    /**
     * Protocol for objects that can be transformed
     * 
     * This protocol demonstrates generic requirements with constraints
     */
    protocol Transformable {
        associatedtype SourceType
        associatedtype TargetType
        
        func transform(from source: SourceType) -> TargetType
        func transform(from source: SourceType, completion: @escaping (TargetType) -> Void)
    }
    
    /**
     * Protocol extension providing default implementation for Transformable
     * 
     * This extension demonstrates default implementations with generics
     */
    extension Transformable {
        func transform(from source: SourceType, completion: @escaping (TargetType) -> Void) {
            DispatchQueue.global(qos: .userInitiated).async {
                let result = self.transform(from: source)
                DispatchQueue.main.async {
                    completion(result)
                }
            }
        }
    }
    
    // MARK: - Protocol with Conditional Conformance
    
    /**
     * Protocol for objects that can be serialized
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Serializable {
        associatedtype DataType
        
        func serialize() -> DataType
        static func deserialize(from data: DataType) -> Self?
    }
    
    /**
     * Protocol for objects that can be encoded
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Encodable {
        associatedtype DataType
        
        func encode() -> DataType
    }
    
    /**
     * Protocol for objects that can be decoded
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Decodable {
        associatedtype DataType
        
        static func decode(from data: DataType) -> Self?
    }
    
    /**
     * Protocol for objects that can be both encoded and decoded
     * 
     * This protocol demonstrates protocol composition with associated types
     */
    protocol Codable: Encodable, Decodable {
        associatedtype DataType
    }
    
    // MARK: - Protocol with Existential Types
    
    /**
     * Protocol for objects that can be observed
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol Observable {
        associatedtype ObserverType: AnyObject
        
        func addObserver(_ observer: ObserverType)
        func removeObserver(_ observer: ObserverType)
        func notifyObservers()
    }
    
    /**
     * Protocol for objects that can be observed with specific events
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol EventObservable {
        associatedtype EventType
        associatedtype ObserverType: AnyObject
        
        func addObserver(_ observer: ObserverType, for event: EventType)
        func removeObserver(_ observer: ObserverType, for event: EventType)
        func notifyObservers(of event: EventType)
    }
    
    /**
     * Protocol for objects that can be observed with multiple events
     * 
     * This protocol demonstrates protocol with associated types
     */
    protocol MultiEventObservable {
        associatedtype EventType: Hashable
        associatedtype ObserverType: AnyObject
        
        func addObserver(_ observer: ObserverType, for events: Set<EventType>)
        func removeObserver(_ observer: ObserverType, for events: Set<EventType>)
        func notifyObservers(of events: Set<EventType>)
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the protocol patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateProtocols() {
    print("=== Swift Protocols Demonstration ===\n")
    
    // Protocol Fundamentals
    let fundamentalExample = ProtocolFundamentals()
    
    print("--- Protocol Fundamentals ---")
    let circle = ProtocolFundamentals.Circle(radius: 5.0, center: CGPoint(x: 10, y: 10))
    circle.draw()
    circle.draw(at: CGPoint(x: 20, y: 20))
    circle.draw(in: CGRect(x: 0, y: 0, width: 100, height: 100))
    
    let rectangle = ProtocolFundamentals.Rectangle(width: 10.0, height: 20.0, origin: CGPoint(x: 5, y: 5))
    rectangle.draw()
    rectangle.draw(at: CGPoint(x: 15, y: 15))
    rectangle.draw(in: CGRect(x: 0, y: 0, width: 200, height: 200))
    
    let animatedCircle = ProtocolFundamentals.AnimatedCircle(radius: 3.0, center: CGPoint(x: 0, y: 0))
    animatedCircle.draw()
    animatedCircle.startAnimation()
    animatedCircle.pauseAnimation()
    animatedCircle.resumeAnimation()
    animatedCircle.stopAnimation()
    
    // Protocol Extensions
    let extensionExample = ProtocolExtensions()
    
    print("\n--- Protocol Extensions ---")
    // Note: In a real implementation, you would have concrete types conforming to these protocols
    // For demonstration purposes, we're showing the protocol definitions and extensions
    
    // Protocol Composition
    let compositionExample = ProtocolComposition()
    
    print("\n--- Protocol Composition ---")
    // Note: In a real implementation, you would have concrete types conforming to these protocols
    // For demonstration purposes, we're showing the protocol composition patterns
    
    // Protocol Inheritance
    let inheritanceExample = ProtocolInheritance()
    
    print("\n--- Protocol Inheritance ---")
    // Note: In a real implementation, you would have concrete types conforming to these protocols
    // For demonstration purposes, we're showing the protocol inheritance patterns
    
    // Advanced Protocol Patterns
    let advancedExample = AdvancedProtocolPatterns()
    
    print("\n--- Advanced Protocol Patterns ---")
    // Note: In a real implementation, you would have concrete types conforming to these protocols
    // For demonstration purposes, we're showing the advanced protocol patterns
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateProtocols()
