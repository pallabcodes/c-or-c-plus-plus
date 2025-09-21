/*
 * Advanced Swift: Generics
 * 
 * This file demonstrates production-grade generic programming patterns in Swift
 * suitable for top-tier companies like Apple, Spotify, and Twitch.
 * 
 * Key Learning Objectives:
 * - Master generic functions and type-safe programming
 * - Understand generic types and custom data structures
 * - Implement generic constraints and where clauses
 * - Apply associated types for protocol-oriented programming
 * 
 * Author: Production iOS Engineer
 * Target: SDE-2 iOS Engineers
 * Standards: Apple/Spotify/Twitch Production Code Quality
 */

import Foundation

// MARK: - Generic Functions

/**
 * Demonstrates generic function patterns used in production iOS apps
 * 
 * This class covers:
 * - Basic generic function syntax
 * - Generic constraints and type requirements
 * - Generic function overloading
 * - Performance considerations for generic functions
 */
class GenericFunctions {
    
    // MARK: - Basic Generic Functions
    
    /**
     * Generic function to swap two values of any type
     * 
     * - Parameters:
     *   - a: First value to swap
     *   - b: Second value to swap
     * 
     * This function demonstrates the most basic generic function pattern
     * where the type parameter T is inferred from the arguments
     */
    func swapValues<T>(_ a: inout T, _ b: inout T) {
        let temp = a
        a = b
        b = temp
    }
    
    /**
     * Generic function to find the maximum of two values
     * 
     * - Parameters:
     *   - a: First value to compare
     *   - b: Second value to compare
     * - Returns: The maximum of the two values
     * 
     * This function demonstrates generic constraints where T must conform to Comparable
     */
    func findMax<T: Comparable>(_ a: T, _ b: T) -> T {
        return a > b ? a : b
    }
    
    /**
     * Generic function to find the maximum value in an array
     * 
     * - Parameter array: Array of values to search
     * - Returns: The maximum value in the array, or nil if empty
     * 
     * This function demonstrates generic constraints with optional return types
     */
    func findMaxInArray<T: Comparable>(_ array: [T]) -> T? {
        guard !array.isEmpty else { return nil }
        
        var max = array[0]
        for element in array.dropFirst() {
            if element > max {
                max = element
            }
        }
        return max
    }
    
    /**
     * Generic function to find the minimum value in an array
     * 
     * - Parameter array: Array of values to search
     * - Returns: The minimum value in the array, or nil if empty
     * 
     * This function demonstrates generic constraints with optional return types
     */
    func findMinInArray<T: Comparable>(_ array: [T]) -> T? {
        guard !array.isEmpty else { return nil }
        
        var min = array[0]
        for element in array.dropFirst() {
            if element < min {
                min = element
            }
        }
        return min
    }
    
    // MARK: - Generic Functions with Multiple Type Parameters
    
    /**
     * Generic function to create a pair of values
     * 
     * - Parameters:
     *   - first: First value of the pair
     *   - second: Second value of the pair
     * - Returns: A tuple containing both values
     * 
     * This function demonstrates multiple type parameters in a generic function
     */
    func createPair<T, U>(_ first: T, _ second: U) -> (T, U) {
        return (first, second)
    }
    
    /**
     * Generic function to transform an array of one type to another
     * 
     * - Parameters:
     *   - array: Array of source type
     *   - transform: Transformation function
     * - Returns: Array of transformed values
     * 
     * This function demonstrates generic functions with closure parameters
     */
    func transformArray<T, U>(_ array: [T], transform: (T) -> U) -> [U] {
        return array.map(transform)
    }
    
    /**
     * Generic function to filter an array based on a predicate
     * 
     * - Parameters:
     *   - array: Array to filter
     *   - predicate: Predicate function to apply
     * - Returns: Filtered array
     * 
     * This function demonstrates generic functions with predicate closures
     */
    func filterArray<T>(_ array: [T], predicate: (T) -> Bool) -> [T] {
        return array.filter(predicate)
    }
    
    // MARK: - Generic Functions with Constraints
    
    /**
     * Generic function to find the index of an element in an array
     * 
     * - Parameters:
     *   - array: Array to search
     *   - element: Element to find
     * - Returns: Index of the element, or nil if not found
     * 
     * This function demonstrates generic constraints with Equatable
     */
    func findIndex<T: Equatable>(_ array: [T], of element: T) -> Int? {
        for (index, value) in array.enumerated() {
            if value == element {
                return index
            }
        }
        return nil
    }
    
    /**
     * Generic function to check if an array contains an element
     * 
     * - Parameters:
     *   - array: Array to search
     *   - element: Element to find
     * - Returns: True if element is found, false otherwise
     * 
     * This function demonstrates generic constraints with Equatable
     */
    func contains<T: Equatable>(_ array: [T], element: T) -> Bool {
        return findIndex(array, of: element) != nil
    }
    
    /**
     * Generic function to remove duplicates from an array
     * 
     * - Parameter array: Array to remove duplicates from
     * - Returns: Array with duplicates removed
     * 
     * This function demonstrates generic constraints with Hashable
     */
    func removeDuplicates<T: Hashable>(_ array: [T]) -> [T] {
        var seen = Set<T>()
        return array.filter { seen.insert($0).inserted }
    }
    
    // MARK: - Generic Functions with Where Clauses
    
    /**
     * Generic function to find common elements between two arrays
     * 
     * - Parameters:
     *   - array1: First array
     *   - array2: Second array
     * - Returns: Array of common elements
     * 
     * This function demonstrates where clauses for complex type constraints
     */
    func findCommonElements<T>(_ array1: [T], _ array2: [T]) -> [T] where T: Hashable, T: Equatable {
        let set1 = Set(array1)
        let set2 = Set(array2)
        return Array(set1.intersection(set2))
    }
    
    /**
     * Generic function to sort an array with custom comparison
     * 
     * - Parameters:
     *   - array: Array to sort
     *   - comparison: Comparison function
     * - Returns: Sorted array
     * 
     * This function demonstrates where clauses with custom comparison functions
     */
    func sortArray<T>(_ array: [T], comparison: (T, T) -> Bool) -> [T] {
        return array.sorted(by: comparison)
    }
    
    /**
     * Generic function to group array elements by a key
     * 
     * - Parameters:
     *   - array: Array to group
     *   - keySelector: Function to extract grouping key
     * - Returns: Dictionary grouped by key
     * 
     * This function demonstrates where clauses with key extraction functions
     */
    func groupBy<T, K: Hashable>(_ array: [T], keySelector: (T) -> K) -> [K: [T]] {
        var groups: [K: [T]] = [:]
        for element in array {
            let key = keySelector(element)
            groups[key, default: []].append(element)
        }
        return groups
    }
}

// MARK: - Generic Types

/**
 * Demonstrates generic type patterns used in production iOS apps
 * 
 * This class covers:
 * - Generic struct and class definitions
 * - Generic type constraints and requirements
 * - Generic type inheritance and protocols
 * - Performance considerations for generic types
 */
class GenericTypes {
    
    // MARK: - Generic Stack Implementation
    
    /**
     * Generic stack data structure
     * 
     * This struct demonstrates a generic type with basic operations
     * and proper memory management
     */
    struct Stack<Element> {
        private var elements: [Element] = []
        
        /**
         * Returns the number of elements in the stack
         */
        var count: Int {
            return elements.count
        }
        
        /**
         * Returns true if the stack is empty
         */
        var isEmpty: Bool {
            return elements.isEmpty
        }
        
        /**
         * Returns the top element without removing it
         * 
         * - Returns: The top element, or nil if stack is empty
         */
        var top: Element? {
            return elements.last
        }
        
        /**
         * Pushes an element onto the stack
         * 
         * - Parameter element: Element to push
         */
        mutating func push(_ element: Element) {
            elements.append(element)
        }
        
        /**
         * Pops an element from the stack
         * 
         * - Returns: The popped element, or nil if stack is empty
         */
        mutating func pop() -> Element? {
            return elements.popLast()
        }
        
        /**
         * Peeks at the top element without removing it
         * 
         * - Returns: The top element, or nil if stack is empty
         */
        func peek() -> Element? {
            return elements.last
        }
        
        /**
         * Removes all elements from the stack
         */
        mutating func clear() {
            elements.removeAll()
        }
    }
    
    // MARK: - Generic Queue Implementation
    
    /**
     * Generic queue data structure
     * 
     * This struct demonstrates a generic type with FIFO operations
     * and efficient memory management
     */
    struct Queue<Element> {
        private var elements: [Element] = []
        
        /**
         * Returns the number of elements in the queue
         */
        var count: Int {
            return elements.count
        }
        
        /**
         * Returns true if the queue is empty
         */
        var isEmpty: Bool {
            return elements.isEmpty
        }
        
        /**
         * Returns the front element without removing it
         * 
         * - Returns: The front element, or nil if queue is empty
         */
        var front: Element? {
            return elements.first
        }
        
        /**
         * Enqueues an element to the back of the queue
         * 
         * - Parameter element: Element to enqueue
         */
        mutating func enqueue(_ element: Element) {
            elements.append(element)
        }
        
        /**
         * Dequeues an element from the front of the queue
         * 
         * - Returns: The dequeued element, or nil if queue is empty
         */
        mutating func dequeue() -> Element? {
            guard !elements.isEmpty else { return nil }
            return elements.removeFirst()
        }
        
        /**
         * Peeks at the front element without removing it
         * 
         * - Returns: The front element, or nil if queue is empty
         */
        func peek() -> Element? {
            return elements.first
        }
        
        /**
         * Removes all elements from the queue
         */
        mutating func clear() {
            elements.removeAll()
        }
    }
    
    // MARK: - Generic Binary Tree Implementation
    
    /**
     * Generic binary tree node
     * 
     * This class demonstrates a generic type with recursive structure
     * and proper memory management
     */
    class BinaryTreeNode<T: Comparable> {
        var value: T
        var left: BinaryTreeNode<T>?
        var right: BinaryTreeNode<T>?
        
        init(value: T) {
            self.value = value
        }
        
        /**
         * Inserts a value into the binary tree
         * 
         * - Parameter value: Value to insert
         */
        func insert(_ value: T) {
            if value < self.value {
                if let left = left {
                    left.insert(value)
                } else {
                    left = BinaryTreeNode(value: value)
                }
            } else if value > self.value {
                if let right = right {
                    right.insert(value)
                } else {
                    right = BinaryTreeNode(value: value)
                }
            }
        }
        
        /**
         * Searches for a value in the binary tree
         * 
         * - Parameter value: Value to search for
         * - Returns: True if value is found, false otherwise
         */
        func search(_ value: T) -> Bool {
            if value == self.value {
                return true
            } else if value < self.value {
                return left?.search(value) ?? false
            } else {
                return right?.search(value) ?? false
            }
        }
        
        /**
         * Performs in-order traversal of the tree
         * 
         * - Parameter visit: Function to call for each node
         */
        func inOrderTraversal(visit: (T) -> Void) {
            left?.inOrderTraversal(visit: visit)
            visit(value)
            right?.inOrderTraversal(visit: visit)
        }
        
        /**
         * Performs pre-order traversal of the tree
         * 
         * - Parameter visit: Function to call for each node
         */
        func preOrderTraversal(visit: (T) -> Void) {
            visit(value)
            left?.preOrderTraversal(visit: visit)
            right?.preOrderTraversal(visit: visit)
        }
        
        /**
         * Performs post-order traversal of the tree
         * 
         * - Parameter visit: Function to call for each node
         */
        func postOrderTraversal(visit: (T) -> Void) {
            left?.postOrderTraversal(visit: visit)
            right?.postOrderTraversal(visit: visit)
            visit(value)
        }
    }
    
    // MARK: - Generic Binary Tree Wrapper
    
    /**
     * Generic binary tree wrapper
     * 
     * This class provides a clean interface for the binary tree
     * and handles the root node management
     */
    class BinaryTree<T: Comparable> {
        private var root: BinaryTreeNode<T>?
        
        /**
         * Returns the number of elements in the tree
         */
        var count: Int {
            var count = 0
            root?.inOrderTraversal { _ in count += 1 }
            return count
        }
        
        /**
         * Returns true if the tree is empty
         */
        var isEmpty: Bool {
            return root == nil
        }
        
        /**
         * Inserts a value into the tree
         * 
         * - Parameter value: Value to insert
         */
        func insert(_ value: T) {
            if let root = root {
                root.insert(value)
            } else {
                root = BinaryTreeNode(value: value)
            }
        }
        
        /**
         * Searches for a value in the tree
         * 
         * - Parameter value: Value to search for
         * - Returns: True if value is found, false otherwise
         */
        func search(_ value: T) -> Bool {
            return root?.search(value) ?? false
        }
        
        /**
         * Performs in-order traversal of the tree
         * 
         * - Parameter visit: Function to call for each node
         */
        func inOrderTraversal(visit: (T) -> Void) {
            root?.inOrderTraversal(visit: visit)
        }
        
        /**
         * Performs pre-order traversal of the tree
         * 
         * - Parameter visit: Function to call for each node
         */
        func preOrderTraversal(visit: (T) -> Void) {
            root?.preOrderTraversal(visit: visit)
        }
        
        /**
         * Performs post-order traversal of the tree
         * 
         * - Parameter visit: Function to call for each node
         */
        func postOrderTraversal(visit: (T) -> Void) {
            root?.postOrderTraversal(visit: visit)
        }
        
        /**
         * Returns an array of all values in the tree
         * 
         * - Returns: Array of all values in sorted order
         */
        func toArray() -> [T] {
            var result: [T] = []
            inOrderTraversal { result.append($0) }
            return result
        }
    }
}

// MARK: - Generic Constraints and Where Clauses

/**
 * Demonstrates generic constraints and where clauses used in production iOS apps
 * 
 * This class covers:
 * - Type constraints and requirements
 * - Where clauses for complex constraints
 * - Protocol conformance requirements
 * - Performance implications of constraints
 */
class GenericConstraints {
    
    // MARK: - Basic Type Constraints
    
    /**
     * Generic function with basic type constraints
     * 
     * - Parameter value: Value to process
     * - Returns: Processed value
     * 
     * This function demonstrates basic type constraints with Comparable
     */
    func processComparable<T: Comparable>(_ value: T) -> T {
        return value
    }
    
    /**
     * Generic function with multiple type constraints
     * 
     * - Parameter value: Value to process
     * - Returns: Processed value
     * 
     * This function demonstrates multiple type constraints
     */
    func processHashableAndComparable<T: Hashable & Comparable>(_ value: T) -> T {
        return value
    }
    
    /**
     * Generic function with protocol conformance constraint
     * 
     * - Parameter value: Value to process
     * - Returns: Processed value
     * 
     * This function demonstrates protocol conformance constraints
     */
    func processCustomProtocol<T: CustomStringConvertible>(_ value: T) -> T {
        print("Processing: \(value)")
        return value
    }
    
    // MARK: - Where Clauses
    
    /**
     * Generic function with where clause constraint
     * 
     * - Parameters:
     *   - array: Array to process
     *   - value: Value to find
     * - Returns: True if value is found, false otherwise
     * 
     * This function demonstrates where clauses for complex constraints
     */
    func findInArray<T>(_ array: [T], value: T) -> Bool where T: Equatable {
        return array.contains(value)
    }
    
    /**
     * Generic function with multiple where clause constraints
     * 
     * - Parameters:
     *   - array: Array to process
     *   - value: Value to find
     * - Returns: True if value is found, false otherwise
     * 
     * This function demonstrates multiple where clause constraints
     */
    func findInArrayWithMultipleConstraints<T>(_ array: [T], value: T) -> Bool where T: Equatable, T: Hashable {
        let set = Set(array)
        return set.contains(value)
    }
    
    /**
     * Generic function with associated type constraints
     * 
     * - Parameter collection: Collection to process
     * - Returns: Number of elements
     * 
     * This function demonstrates associated type constraints
     */
    func countElements<T: Collection>(_ collection: T) -> Int where T.Element: Equatable {
        return collection.count
    }
    
    // MARK: - Generic Type Constraints
    
    /**
     * Generic struct with type constraints
     * 
     * This struct demonstrates generic type constraints
     */
    struct ConstrainedContainer<T: Comparable> {
        private var elements: [T] = []
        
        /**
         * Adds an element to the container
         * 
         * - Parameter element: Element to add
         */
        mutating func add(_ element: T) {
            elements.append(element)
        }
        
        /**
         * Sorts the elements in the container
         */
        mutating func sort() {
            elements.sort()
        }
        
        /**
         * Returns the sorted elements
         * 
         * - Returns: Array of sorted elements
         */
        func getSortedElements() -> [T] {
            return elements.sorted()
        }
    }
    
    /**
     * Generic class with protocol conformance constraints
     * 
     * This class demonstrates protocol conformance constraints
     */
    class ProtocolConstrainedContainer<T: CustomStringConvertible> {
        private var elements: [T] = []
        
        /**
         * Adds an element to the container
         * 
         * - Parameter element: Element to add
         */
        func add(_ element: T) {
            elements.append(element)
        }
        
        /**
         * Returns a description of all elements
         * 
         * - Returns: String description of all elements
         */
        func description() -> String {
            return elements.map { $0.description }.joined(separator: ", ")
        }
    }
}

// MARK: - Associated Types

/**
 * Demonstrates associated types used in production iOS apps
 * 
 * This class covers:
 * - Protocol with associated types
 * - Generic type conformance to protocols
 * - Associated type constraints
 * - Performance implications of associated types
 */
class AssociatedTypes {
    
    // MARK: - Protocol with Associated Types
    
    /**
     * Protocol for containers that can store and retrieve elements
     * 
     * This protocol demonstrates associated types for generic containers
     */
    protocol Container {
        associatedtype Element
        
        var count: Int { get }
        var isEmpty: Bool { get }
        
        mutating func add(_ element: Element)
        mutating func remove() -> Element?
        func peek() -> Element?
    }
    
    /**
     * Protocol for collections that can be iterated
     * 
     * This protocol demonstrates associated types for iterable collections
     */
    protocol Iterable {
        associatedtype Element
        associatedtype Iterator: IteratorProtocol where Iterator.Element == Element
        
        func makeIterator() -> Iterator
    }
    
    /**
     * Protocol for data sources that can provide data
     * 
     * This protocol demonstrates associated types for data sources
     */
    protocol DataSource {
        associatedtype DataType
        
        func fetchData() -> [DataType]
        func fetchData(completion: @escaping ([DataType]) -> Void)
    }
    
    // MARK: - Generic Type Conformance
    
    /**
     * Generic stack that conforms to Container protocol
     * 
     * This struct demonstrates generic type conformance to protocols
     */
    struct GenericStack<Element>: Container {
        private var elements: [Element] = []
        
        var count: Int {
            return elements.count
        }
        
        var isEmpty: Bool {
            return elements.isEmpty
        }
        
        mutating func add(_ element: Element) {
            elements.append(element)
        }
        
        mutating func remove() -> Element? {
            return elements.popLast()
        }
        
        func peek() -> Element? {
            return elements.last
        }
    }
    
    /**
     * Generic queue that conforms to Container protocol
     * 
     * This struct demonstrates generic type conformance to protocols
     */
    struct GenericQueue<Element>: Container {
        private var elements: [Element] = []
        
        var count: Int {
            return elements.count
        }
        
        var isEmpty: Bool {
            return elements.isEmpty
        }
        
        mutating func add(_ element: Element) {
            elements.append(element)
        }
        
        mutating func remove() -> Element? {
            guard !elements.isEmpty else { return nil }
            return elements.removeFirst()
        }
        
        func peek() -> Element? {
            return elements.first
        }
    }
    
    /**
     * Generic array that conforms to Iterable protocol
     * 
     * This struct demonstrates generic type conformance to protocols
     */
    struct GenericArray<Element>: Iterable {
        private var elements: [Element]
        
        init(elements: [Element] = []) {
            self.elements = elements
        }
        
        func makeIterator() -> IndexingIterator<[Element]> {
            return elements.makeIterator()
        }
    }
    
    // MARK: - Associated Type Constraints
    
    /**
     * Protocol with associated type constraints
     * 
     * This protocol demonstrates associated type constraints
     */
    protocol ConstrainedContainer {
        associatedtype Element: Comparable
        
        var count: Int { get }
        mutating func add(_ element: Element)
        mutating func remove() -> Element?
        func find(_ element: Element) -> Bool
    }
    
    /**
     * Generic set that conforms to ConstrainedContainer protocol
     * 
     * This struct demonstrates generic type conformance with constraints
     */
    struct GenericSet<Element: Comparable>: ConstrainedContainer {
        private var elements: Set<Element>
        
        init() {
            self.elements = Set<Element>()
        }
        
        var count: Int {
            return elements.count
        }
        
        mutating func add(_ element: Element) {
            elements.insert(element)
        }
        
        mutating func remove() -> Element? {
            return elements.popFirst()
        }
        
        func find(_ element: Element) -> Bool {
            return elements.contains(element)
        }
    }
}

// MARK: - Usage Examples

/**
 * Demonstrates how to use all the generic patterns
 * 
 * This function shows practical usage of all the concepts covered
 */
func demonstrateGenerics() {
    print("=== Swift Generics Demonstration ===\n")
    
    // Generic Functions
    let functionExample = GenericFunctions()
    
    print("--- Generic Functions ---")
    var a = 5
    var b = 10
    functionExample.swapValues(&a, &b)
    print("Swapped values: a=\(a), b=\(b)")
    
    print("Max of 5 and 10: \(functionExample.findMax(5, 10))")
    print("Max in array: \(functionExample.findMaxInArray([1, 5, 3, 9, 2]) ?? -1)")
    
    let pair = functionExample.createPair("Hello", 42)
    print("Pair: \(pair)")
    
    let transformed = functionExample.transformArray([1, 2, 3, 4, 5]) { $0 * 2 }
    print("Transformed array: \(transformed)")
    
    let filtered = functionExample.filterArray([1, 2, 3, 4, 5]) { $0 % 2 == 0 }
    print("Filtered array: \(filtered)")
    
    print("Index of 3: \(functionExample.findIndex([1, 2, 3, 4, 5], of: 3) ?? -1)")
    print("Contains 3: \(functionExample.contains([1, 2, 3, 4, 5], element: 3))")
    
    let unique = functionExample.removeDuplicates([1, 2, 2, 3, 3, 3, 4])
    print("Unique elements: \(unique)")
    
    let common = functionExample.findCommonElements([1, 2, 3, 4], [3, 4, 5, 6])
    print("Common elements: \(common)")
    
    let grouped = functionExample.groupBy(["apple", "banana", "cherry", "date"]) { $0.first! }
    print("Grouped by first letter: \(grouped)")
    
    // Generic Types
    let typeExample = GenericTypes()
    
    print("\n--- Generic Types ---")
    var stack = GenericTypes.Stack<Int>()
    stack.push(1)
    stack.push(2)
    stack.push(3)
    print("Stack count: \(stack.count)")
    print("Stack top: \(stack.top ?? -1)")
    print("Popped: \(stack.pop() ?? -1)")
    
    var queue = GenericTypes.Queue<String>()
    queue.enqueue("first")
    queue.enqueue("second")
    queue.enqueue("third")
    print("Queue count: \(queue.count)")
    print("Queue front: \(queue.front ?? "empty")")
    print("Dequeued: \(queue.dequeue() ?? "empty")")
    
    let tree = GenericTypes.BinaryTree<Int>()
    tree.insert(5)
    tree.insert(3)
    tree.insert(7)
    tree.insert(1)
    tree.insert(9)
    print("Tree count: \(tree.count)")
    print("Tree contains 3: \(tree.search(3))")
    print("Tree contains 6: \(tree.search(6))")
    
    var inOrder: [Int] = []
    tree.inOrderTraversal { inOrder.append($0) }
    print("In-order traversal: \(inOrder)")
    
    // Generic Constraints
    let constraintExample = GenericConstraints()
    
    print("\n--- Generic Constraints ---")
    print("Processed comparable: \(constraintExample.processComparable(42))")
    print("Processed hashable and comparable: \(constraintExample.processHashableAndComparable(42))")
    print("Processed custom protocol: \(constraintExample.processCustomProtocol(42))")
    
    print("Find in array: \(constraintExample.findInArray([1, 2, 3, 4, 5], value: 3))")
    print("Find with multiple constraints: \(constraintExample.findInArrayWithMultipleConstraints([1, 2, 3, 4, 5], value: 3))")
    print("Count elements: \(constraintExample.countElements([1, 2, 3, 4, 5]))")
    
    var container = GenericConstraints.ConstrainedContainer<Int>()
    container.add(3)
    container.add(1)
    container.add(4)
    container.add(1)
    container.sort()
    print("Sorted container: \(container.getSortedElements())")
    
    let protocolContainer = GenericConstraints.ProtocolConstrainedContainer<Int>()
    protocolContainer.add(1)
    protocolContainer.add(2)
    protocolContainer.add(3)
    print("Protocol container description: \(protocolContainer.description())")
    
    // Associated Types
    let associatedExample = AssociatedTypes()
    
    print("\n--- Associated Types ---")
    var stack = AssociatedTypes.GenericStack<Int>()
    stack.add(1)
    stack.add(2)
    stack.add(3)
    print("Stack count: \(stack.count)")
    print("Stack peek: \(stack.peek() ?? -1)")
    print("Stack remove: \(stack.remove() ?? -1)")
    
    var queue = AssociatedTypes.GenericQueue<String>()
    queue.add("first")
    queue.add("second")
    queue.add("third")
    print("Queue count: \(queue.count)")
    print("Queue peek: \(queue.peek() ?? "empty")")
    print("Queue remove: \(queue.remove() ?? "empty")")
    
    let array = AssociatedTypes.GenericArray(elements: [1, 2, 3, 4, 5])
    var iterator = array.makeIterator()
    while let element = iterator.next() {
        print("Array element: \(element)")
    }
    
    var set = AssociatedTypes.GenericSet<Int>()
    set.add(1)
    set.add(2)
    set.add(3)
    set.add(1) // Duplicate
    print("Set count: \(set.count)")
    print("Set contains 2: \(set.find(2))")
    print("Set contains 5: \(set.find(5))")
}

// MARK: - Run Demonstration

// Uncomment to run the demonstration
// demonstrateGenerics()
