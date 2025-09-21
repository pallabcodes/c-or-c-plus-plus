/*
 * Swift Fundamentals: Memory Management
 * 
 * This file demonstrates production-grade memory management patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master Automatic Reference Counting (ARC) and retain cycles
 * - Understand weak, unowned, and strong reference patterns
 * - Implement proper memory management for closures and delegates
 * - Apply memory optimization techniques for performance
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - ARC Fundamentals

/**
 * Demonstrates Automatic Reference Counting (ARC) patterns
 * 
 * This class covers:
 * - Strong reference cycles and retain cycles
 * - Weak and unowned references
 * - Memory management best practices
 * - Performance implications of different reference types
 */
class ARCFundamentals {
    
    // MARK: - Strong Reference Cycle Example
    
    /**
     * Represents a person in the system
     * 
     * This class demonstrates strong reference cycles and how to avoid them
     */
    class Person {
        let name: String
        var apartment: Apartment?
        
        init(name: String) {
            self.name = name
            print("Person \(name) is being initialized")
        }
        
        deinit {
            print("Person \(name) is being deinitialized")
        }
    }
    
    /**
     * Represents an apartment in the system
     * 
     * This class demonstrates strong reference cycles and how to avoid them
     */
    class Apartment {
        let unit: String
        var tenant: Person?
        
        init(unit: String) {
            self.unit = unit
            print("Apartment \(unit) is being initialized")
        }
        
        deinit {
            print("Apartment \(unit) is being deinitialized")
        }
    }
    
    /**
     * Demonstrates strong reference cycle creation
     * 
     * This method creates a retain cycle that prevents deinitialization
     */
    func createStrongReferenceCycle() {
        let john = Person(name: "John")
        let unit4A = Apartment(unit: "4A")
        
        // Create strong reference cycle
        john.apartment = unit4A
        unit4A.tenant = john
        
        // Both objects will not be deinitialized due to retain cycle
        print("Strong reference cycle created - objects will not be deinitialized")
    }
    
    /**
     * Demonstrates weak reference to break retain cycle
     * 
     * This method shows how to use weak references to prevent retain cycles
     */
    func createWeakReference() {
        let john = Person(name: "John")
        let unit4A = Apartment(unit: "4A")
        
        // Create weak reference to break cycle
        john.apartment = unit4A
        unit4A.tenant = john
        
        // Objects will be deinitialized when references go out of scope
        print("Weak reference used - objects will be deinitialized")
    }
    
    // MARK: - Weak Reference Example
    
    /**
     * Represents an apartment with weak reference to tenant
     * 
     * This class demonstrates proper use of weak references
     */
    class ApartmentWithWeakTenant {
        let unit: String
        weak var tenant: Person?
        
        init(unit: String) {
            self.unit = unit
            print("Apartment \(unit) is being initialized")
        }
        
        deinit {
            print("Apartment \(unit) is being deinitialized")
        }
    }
    
    /**
     * Demonstrates weak reference usage
     * 
     * This method shows how weak references prevent retain cycles
     */
    func demonstrateWeakReference() {
        let john = Person(name: "John")
        let unit4A = ApartmentWithWeakTenant(unit: "4A")
        
        // Create weak reference (no retain cycle)
        john.apartment = unit4A
        unit4A.tenant = john
        
        // Objects will be deinitialized when references go out of scope
        print("Weak reference demonstration - objects will be deinitialized")
    }
    
    // MARK: - Unowned Reference Example
    
    /**
     * Represents a customer in the system
     * 
     * This class demonstrates unowned references
     */
    class Customer {
        let name: String
        var card: CreditCard?
        
        init(name: String) {
            self.name = name
            print("Customer \(name) is being initialized")
        }
        
        deinit {
            print("Customer \(name) is being deinitialized")
        }
    }
    
    /**
     * Represents a credit card with unowned reference to customer
     * 
     * This class demonstrates proper use of unowned references
     */
    class CreditCard {
        let number: UInt64
        unowned let customer: Customer
        
        init(number: UInt64, customer: Customer) {
            self.number = number
            self.customer = customer
            print("CreditCard #\(number) is being initialized")
        }
        
        deinit {
            print("CreditCard #\(number) is being deinitialized")
        }
    }
    
    /**
     * Demonstrates unowned reference usage
     * 
     * This method shows how unowned references prevent retain cycles
     */
    func demonstrateUnownedReference() {
        let john = Customer(name: "John")
        let card = CreditCard(number: 1234_5678_9012_3456, customer: john)
        
        john.card = card
        
        // Objects will be deinitialized when references go out of scope
        print("Unowned reference demonstration - objects will be deinitialized")
    }
}

// MARK: - Closure Memory Management

/**
 * Demonstrates memory management patterns for closures
 * 
 * This class covers:
 * - Closure capture semantics
 * - Weak and unowned capture lists
 * - Escaping vs non-escaping closures
 * - Memory optimization for closures
 */
class ClosureMemoryManagement {
    
    // MARK: - Closure Capture Semantics
    
    /**
     * Demonstrates closure capture semantics
     * 
     * - Parameter multiplier: Multiplier value
     * - Returns: Closure that multiplies by the given value
     */
    func createMultiplier(_ multiplier: Int) -> () -> Int {
        var counter = 0
        
        // Closure captures 'multiplier' and 'counter' by value
        let closure = {
            counter += 1
            return counter * multiplier
        }
        
        return closure
    }
    
    /**
     * Demonstrates closure with reference capture
     * 
     * - Parameter multiplier: Multiplier value
     * - Returns: Closure that multiplies by the given value
     */
    func createMultiplierWithReference(_ multiplier: Int) -> () -> Int {
        var counter = 0
        
        // Closure captures 'multiplier' by value and 'counter' by reference
        let closure = { [multiplier] in
            counter += 1
            return counter * multiplier
        }
        
        return closure
    }
    
    // MARK: - Weak Capture Lists
    
    /**
     * Demonstrates weak capture in closures
     * 
     * - Parameter completion: Completion closure
     */
    func performAsyncOperation(completion: @escaping (String) -> Void) {
        // Simulate async operation
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            guard let self = self else {
                completion("Operation cancelled - self deallocated")
                return
            }
            
            let result = self.processData()
            
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    /**
     * Demonstrates unowned capture in closures
     * 
     * - Parameter completion: Completion closure
     */
    func performGuaranteedOperation(completion: @escaping (String) -> Void) {
        // Simulate async operation
        DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
            let result = self.processData()
            
            DispatchQueue.main.async {
                completion(result)
            }
        }
    }
    
    /**
     * Helper method for data processing
     * 
     * - Returns: Processed data string
     */
    private func processData() -> String {
        return "Processed data at \(Date())"
    }
    
    // MARK: - Memory-Efficient Closures
    
    /**
     * Demonstrates memory-efficient closure patterns
     * 
     * - Parameter numbers: Array of numbers to process
     * - Returns: Array of processed numbers
     */
    func processNumbersEfficiently(_ numbers: [Int]) -> [Int] {
        // Use map with closure that doesn't capture self
        return numbers.map { number in
            return number * 2
        }
    }
    
    /**
     * Demonstrates closure with minimal capture
     * 
     * - Parameter numbers: Array of numbers to process
     * - Parameter multiplier: Multiplier value
     * - Returns: Array of processed numbers
     */
    func processNumbersWithMultiplier(_ numbers: [Int], multiplier: Int) -> [Int] {
        // Capture only what's needed
        return numbers.map { [multiplier] number in
            return number * multiplier
        }
    }
}

// MARK: - Delegate Pattern Memory Management

/**
 * Demonstrates proper memory management for delegate patterns
 * 
 * This class covers:
 * - Weak delegate references
 * - Protocol conformance
 * - Memory-safe delegate patterns
 */
class DelegateMemoryManagement {
    
    // MARK: - Delegate Protocol
    
    /**
     * Protocol for data source delegates
     * 
     * This protocol demonstrates proper delegate pattern design
     */
    protocol DataSourceDelegate: AnyObject {
        func dataSourceDidUpdate(_ dataSource: DataSource)
        func dataSource(_ dataSource: DataSource, didFailWithError error: Error)
    }
    
    // MARK: - Data Source Class
    
    /**
     * Represents a data source with delegate
     * 
     * This class demonstrates proper delegate memory management
     */
    class DataSource {
        weak var delegate: DataSourceDelegate?
        private var data: [String] = []
        
        init() {
            print("DataSource initialized")
        }
        
        deinit {
            print("DataSource deinitialized")
        }
        
        /**
         * Updates the data and notifies delegate
         * 
         * - Parameter newData: New data to set
         */
        func updateData(_ newData: [String]) {
            data = newData
            delegate?.dataSourceDidUpdate(self)
        }
        
        /**
         * Simulates a data fetch operation
         * 
         * - Parameter completion: Completion closure
         */
        func fetchData(completion: @escaping (Result<[String], Error>) -> Void) {
            // Simulate async operation
            DispatchQueue.global(qos: .userInitiated).async { [weak self] in
                guard let self = self else {
                    completion(.failure(DataSourceError.deallocated))
                    return
                }
                
                // Simulate network delay
                Thread.sleep(forTimeInterval: 1.0)
                
                // Simulate success
                let newData = ["Item 1", "Item 2", "Item 3"]
                self.data = newData
                
                DispatchQueue.main.async {
                    self.delegate?.dataSourceDidUpdate(self)
                    completion(.success(newData))
                }
            }
        }
        
        /**
         * Gets the current data
         * 
         * - Returns: Current data array
         */
        func getData() -> [String] {
            return data
        }
    }
    
    // MARK: - Delegate Implementation
    
    /**
     * Represents a view controller that implements delegate
     * 
     * This class demonstrates proper delegate implementation
     */
    class ViewController: DataSourceDelegate {
        private let dataSource: DataSource
        
        init() {
            self.dataSource = DataSource()
            self.dataSource.delegate = self
            print("ViewController initialized")
        }
        
        deinit {
            print("ViewController deinitialized")
        }
        
        /**
         * Starts data fetching
         */
        func startDataFetch() {
            dataSource.fetchData { result in
                switch result {
                case .success(let data):
                    print("Data fetched successfully: \(data)")
                case .failure(let error):
                    print("Data fetch failed: \(error.localizedDescription)")
                }
            }
        }
        
        // MARK: - DataSourceDelegate
        
        func dataSourceDidUpdate(_ dataSource: DataSource) {
            print("Data source updated: \(dataSource.getData())")
        }
        
        func dataSource(_ dataSource: DataSource, didFailWithError error: Error) {
            print("Data source failed: \(error.localizedDescription)")
        }
    }
    
    // MARK: - Error Types
    
    /**
     * Custom error types for data source operations
     */
    enum DataSourceError: Error, LocalizedError {
        case deallocated
        case networkError
        case invalidData
        
        var errorDescription: String? {
            switch self {
            case .deallocated:
                return "Data source was deallocated"
            case .networkError:
                return "Network error occurred"
            case .invalidData:
                return "Invalid data received"
            }
        }
    }
}

// MARK: - Memory Optimization Techniques

/**
 * Demonstrates memory optimization techniques for production apps
 * 
 * This class covers:
 * - Lazy loading and initialization
 * - Memory-efficient data structures
 * - Object pooling patterns
 * - Memory profiling and optimization
 */
class MemoryOptimization {
    
    // MARK: - Lazy Loading
    
    /**
     * Demonstrates lazy loading for expensive operations
     * 
     * This class shows how to use lazy properties for memory efficiency
     */
    class LazyLoadingExample {
        private let data: [Int]
        
        init(data: [Int]) {
            self.data = data
            print("LazyLoadingExample initialized")
        }
        
        deinit {
            print("LazyLoadingExample deinitialized")
        }
        
        /**
         * Lazy property that's only computed when accessed
         * 
         * This property demonstrates lazy loading for expensive computations
         */
        lazy var expensiveComputation: [Int] = {
            print("Performing expensive computation...")
            return data.map { $0 * $0 }
        }()
        
        /**
         * Lazy property with complex initialization
         * 
         * This property demonstrates lazy loading with complex setup
         */
        lazy var complexObject: ComplexObject = {
            print("Creating complex object...")
            return ComplexObject(data: data)
        }()
    }
    
    /**
     * Represents a complex object for lazy loading demonstration
     */
    class ComplexObject {
        let data: [Int]
        let processedData: [Int]
        
        init(data: [Int]) {
            self.data = data
            self.processedData = data.map { $0 * 2 }
            print("ComplexObject initialized with \(data.count) items")
        }
        
        deinit {
            print("ComplexObject deinitialized")
        }
    }
    
    // MARK: - Object Pooling
    
    /**
     * Demonstrates object pooling for memory efficiency
     * 
     * This class shows how to reuse objects to reduce memory allocation
     */
    class ObjectPool<T> {
        private var pool: [T] = []
        private let createObject: () -> T
        private let resetObject: (T) -> Void
        private let maxSize: Int
        
        init(createObject: @escaping () -> T, resetObject: @escaping (T) -> Void, maxSize: Int = 10) {
            self.createObject = createObject
            self.resetObject = resetObject
            self.maxSize = maxSize
        }
        
        /**
         * Gets an object from the pool or creates a new one
         * 
         * - Returns: Object from pool or newly created
         */
        func getObject() -> T {
            if let object = pool.popLast() {
                return object
            } else {
                return createObject()
            }
        }
        
        /**
         * Returns an object to the pool
         * 
         * - Parameter object: Object to return to pool
         */
        func returnObject(_ object: T) {
            guard pool.count < maxSize else {
                return
            }
            
            resetObject(object)
            pool.append(object)
        }
    }
    
    /**
     * Demonstrates object pooling usage
     * 
     * This method shows how to use object pooling effectively
     */
    func demonstrateObjectPooling() {
        let pool = ObjectPool<DataProcessor>(
            createObject: { DataProcessor() },
            resetObject: { $0.reset() },
            maxSize: 5
        )
        
        // Use objects from pool
        let processor1 = pool.getObject()
        let processor2 = pool.getObject()
        
        // Process data
        let result1 = processor1.process([1, 2, 3, 4, 5])
        let result2 = processor2.process([6, 7, 8, 9, 10])
        
        print("Processed data 1: \(result1)")
        print("Processed data 2: \(result2)")
        
        // Return objects to pool
        pool.returnObject(processor1)
        pool.returnObject(processor2)
    }
    
    /**
     * Represents a data processor for object pooling demonstration
     */
    class DataProcessor {
        private var processedCount: Int = 0
        
        init() {
            print("DataProcessor created")
        }
        
        deinit {
            print("DataProcessor deinitialized")
        }
        
        /**
         * Processes an array of integers
         * 
         * - Parameter data: Array of integers to process
         * - Returns: Processed array
         */
        func process(_ data: [Int]) -> [Int] {
            processedCount += 1
            return data.map { $0 * 2 }
        }
        
        /**
         * Resets the processor for reuse
         */
        func reset() {
            processedCount = 0
        }
    }
    
    // MARK: - Memory-Efficient Data Structures
    
    /**
     * Demonstrates memory-efficient data structures
     * 
     * This method shows how to use appropriate data structures for memory efficiency
     */
    func demonstrateMemoryEfficientStructures() {
        // Use Set for unique values instead of Array with duplicates
        let uniqueNumbers = Set([1, 2, 3, 4, 5, 1, 2, 3])
        print("Unique numbers: \(uniqueNumbers)")
        
        // Use Dictionary for key-value lookups instead of Array searches
        let userLookup = Dictionary(uniqueKeysWithValues: [
            ("user1", "Alice"),
            ("user2", "Bob"),
            ("user3", "Charlie")
        ])
        print("User lookup: \(userLookup)")
        
        // Use lazy collections for large datasets
        let largeDataset = (1...1000000).lazy.map { $0 * 2 }
        let firstTen = Array(largeDataset.prefix(10))
        print("First ten processed numbers: \(firstTen)")
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the memory management patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateMemoryManagement() {
    print("=== Swift Memory Management Demonstration ===\n")
    
    // ARC Fundamentals
    let arcExample = ARCFundamentals()
    
    print("--- Strong Reference Cycle ---")
    arcExample.createStrongReferenceCycle()
    
    print("\n--- Weak Reference ---")
    arcExample.createWeakReference()
    
    print("\n--- Unowned Reference ---")
    arcExample.demonstrateUnownedReference()
    
    // Closure Memory Management
    let closureExample = ClosureMemoryManagement()
    
    print("\n--- Closure Capture Semantics ---")
    let multiplier = closureExample.createMultiplier(5)
    print("Multiplier result 1: \(multiplier())")
    print("Multiplier result 2: \(multiplier())")
    
    let multiplierWithRef = closureExample.createMultiplierWithReference(3)
    print("Multiplier with reference result 1: \(multiplierWithRef())")
    print("Multiplier with reference result 2: \(multiplierWithRef())")
    
    print("\n--- Async Operations ---")
    closureExample.performAsyncOperation { result in
        print("Async operation result: \(result)")
    }
    
    // Delegate Memory Management
    let delegateExample = DelegateMemoryManagement()
    
    print("\n--- Delegate Pattern ---")
    let viewController = DelegateMemoryManagement.ViewController()
    viewController.startDataFetch()
    
    // Memory Optimization
    let optimizationExample = MemoryOptimization()
    
    print("\n--- Lazy Loading ---")
    let lazyExample = MemoryOptimization.LazyLoadingExample(data: [1, 2, 3, 4, 5])
    print("Accessing expensive computation: \(lazyExample.expensiveComputation)")
    print("Accessing complex object: \(lazyExample.complexObject.processedData)")
    
    print("\n--- Object Pooling ---")
    optimizationExample.demonstrateObjectPooling()
    
    print("\n--- Memory-Efficient Structures ---")
    optimizationExample.demonstrateMemoryEfficientStructures()
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateMemoryManagement()
