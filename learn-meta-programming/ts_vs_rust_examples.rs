/**
 * TypeScript vs Rust: Side-by-Side Code Examples
 *
 * This file demonstrates equivalent code in TypeScript and Rust,
 * showing the differences and similarities between the two languages.
 */

// =============================================================================
// 1. GENERICS - Basic Example
// =============================================================================

// Rust: Generic function with trait bounds
fn max_value<T: PartialOrd>(a: T, b: T) -> T {
    if a > b { a } else { b }
}

// TypeScript equivalent:
// function maxValue<T>(a: T, b: T): T {
//     return a > b ? a : b;
// }

fn demonstrate_generics() {
    println!("\n=== Generics ===");
    
    let int_max = max_value(10, 20);
    let float_max = max_value(3.14, 2.71);
    
    println!("max_value(10, 20) = {}", int_max);
    println!("max_value(3.14, 2.71) = {}", float_max);
    
    // Rust: Monomorphizes (generates code for each type)
    // TypeScript: Erases types at runtime
}

// =============================================================================
// 2. OWNERSHIP (RUST ONLY - UNIQUE FEATURE)
// =============================================================================

fn demonstrate_ownership() {
    println!("\n=== Ownership (Rust Only) ===");
    
    let s = String::from("hello");
    
    // Move ownership
    let s2 = s;  // s moved to s2
    // println!("{}", s);  // ERROR: s no longer valid
    
    // Borrow (reference)
    let s3 = String::from("world");
    let len = calculate_length(&s3);  // Borrow s3
    println!("Length of '{}' is {}", s3, len);  // s3 still valid
    
    // TypeScript equivalent:
    // const s = "hello";
    // const s2 = s;  // Both reference same string
    // console.log(s);  // OK: GC manages memory
}

fn calculate_length(s: &String) -> usize {
    s.len()
}

// =============================================================================
// 3. OPTION<T> VS NULL (RUST'S NO NULL)
// =============================================================================

fn find_user(id: i32) -> Option<String> {
    if id > 0 {
        Some(format!("User {}", id))
    } else {
        None
    }
}

fn demonstrate_option() {
    println!("\n=== Option<T> vs Null ===");
    
    match find_user(42) {
        Some(user) => println!("Found: {}", user),
        None => println!("User not found"),
    }
    
    // Must handle None case - compile-time guarantee!
    
    // TypeScript equivalent:
    // function findUser(id: number): string | null {
    //     return id > 0 ? `User ${id}` : null;
    // }
    // const user = findUser(42);
    // if (user !== null) {
    //     console.log("Found:", user);
    // }
    // // But can forget to check null!
}

// =============================================================================
// 4. RESULT<T, E> VS EXCEPTIONS
// =============================================================================

fn divide(a: i32, b: i32) -> Result<i32, String> {
    if b == 0 {
        Err("Division by zero".to_string())
    } else {
        Ok(a / b)
    }
}

fn demonstrate_result() {
    println!("\n=== Result<T, E> vs Exceptions ===");
    
    match divide(10, 2) {
        Ok(result) => println!("Result: {}", result),
        Err(e) => println!("Error: {}", e),
    }
    
    // Must handle error - compile-time guarantee!
    
    // TypeScript equivalent:
    // function divide(a: number, b: number): number {
    //     if (b === 0) {
    //         throw new Error("Division by zero");
    //     }
    //     return a / b;
    // }
    // try {
    //     const result = divide(10, 2);
    //     console.log("Result:", result);
    // } catch (e) {
    //     console.log("Error:", e);
    // }
    // // But can forget try-catch!
}

// =============================================================================
// 5. PATTERN MATCHING
// =============================================================================

enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(i32, i32, i32),
}

fn demonstrate_pattern_matching() {
    println!("\n=== Pattern Matching ===");
    
    let msg = Message::Move { x: 10, y: 20 };
    
    match msg {
        Message::Quit => println!("Quit"),
        Message::Move { x, y } => println!("Move to ({}, {})", x, y),
        Message::Write(s) => println!("Write: {}", s),
        Message::ChangeColor(r, g, b) => println!("Color: RGB({}, {}, {})", r, g, b),
    }
    
    // Exhaustive checking - compiler ensures all cases covered!
    
    // TypeScript equivalent:
    // type Message =
    //     | { kind: "quit" }
    //     | { kind: "move"; x: number; y: number }
    //     | { kind: "write"; text: string }
    //     | { kind: "changeColor"; r: number; g: number; b: number };
    // 
    // function process(msg: Message): void {
    //     switch (msg.kind) {
    //         case "quit":
    //             console.log("Quit");
    //             break;
    //         case "move":
    //             console.log(`Move to (${msg.x}, ${msg.y})`);
    //             break;
    //         // Can forget cases!
    //     }
    // }
}

// =============================================================================
// 6. TRAITS VS INTERFACES
// =============================================================================

trait Animal {
    fn name(&self) -> &str;
    fn speak(&self) {
        println!("{} makes a sound", self.name());
    }
}

trait Fly {
    fn fly(&self);
}

struct Bird {
    name: String,
}

impl Animal for Bird {
    fn name(&self) -> &str {
        &self.name
    }
}

impl Fly for Bird {
    fn fly(&self) {
        println!("{} is flying", self.name);
    }
}

fn demonstrate_traits() {
    println!("\n=== Traits vs Interfaces ===");
    
    let bird = Bird { name: "Eagle".to_string() };
    bird.speak();
    bird.fly();
    
    // Traits can have default implementations
    // Can implement multiple traits
    
    // TypeScript equivalent:
    // interface Animal {
    //     name(): string;
    //     speak(): void;
    // }
    // interface Fly {
    //     fly(): void;
    // }
    // class Bird implements Animal, Fly {
    //     constructor(private name: string) {}
    //     name(): string { return this.name; }
    //     speak(): void { console.log(`${this.name} makes a sound`); }
    //     fly(): void { console.log(`${this.name} is flying`); }
    // }
}

// =============================================================================
// 7. CONST GENERICS (RUST ONLY)
// =============================================================================

struct Array<T, const N: usize> {
    data: [T; N],
}

impl<T, const N: usize> Array<T, N> {
    fn new() -> Self {
        // N is known at compile-time
        Array {
            data: unsafe { std::mem::zeroed() },
        }
    }
    
    fn size(&self) -> usize {
        N  // Compile-time constant
    }
}

fn demonstrate_const_generics() {
    println!("\n=== Const Generics (Rust Only) ===");
    
    let arr: Array<i32, 10> = Array::new();
    println!("Array size: {}", arr.size());
    
    // Different sizes = different types!
    let arr2: Array<i32, 5> = Array::new();
    
    // TypeScript: Can't use values as type parameters
    // Can only use types
}

// =============================================================================
// 8. ITERATORS (ZERO-COST ABSTRACTIONS)
// =============================================================================

fn demonstrate_iterators() {
    println!("\n=== Iterators (Zero-Cost Abstractions) ===");
    
    let sum: i32 = (1..10)
        .map(|x| x * 2)
        .filter(|x| x > 5)
        .sum();
    
    println!("Sum: {}", sum);
    
    // Compiles to efficient code, no overhead!
    
    // TypeScript equivalent:
    // const sum = Array.from({ length: 10 }, (_, i) => i + 1)
    //     .map(x => x * 2)
    //     .filter(x => x > 5)
    //     .reduce((a, b) => a + b, 0);
    // 
    // Runtime overhead (though optimized by engines)
}

// =============================================================================
// 9. LIFETIME PARAMETERS (RUST ONLY)
// =============================================================================

fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}

fn demonstrate_lifetimes() {
    println!("\n=== Lifetime Parameters (Rust Only) ===");
    
    let string1 = String::from("long string");
    let string2 = "xyz";
    
    let result = longest(string1.as_str(), string2);
    println!("Longest: {}", result);
    
    // Compiler ensures returned reference is valid
    
    // TypeScript: No lifetime concept (GC manages)
    // function longest(x: string, y: string): string {
    //     return x.length > y.length ? x : y;
    // }
}

// =============================================================================
// 10. STRUCTURAL VS NOMINAL TYPING
// =============================================================================

trait Duck {
    fn quack(&self);
}

struct MyDuck {
    name: String,
}

impl Duck for MyDuck {
    fn quack(&self) {
        println!("{} quacks!", self.name);
    }
}

fn make_quack(duck: &impl Duck) {
    duck.quack();
}

fn demonstrate_typing() {
    println!("\n=== Structural vs Nominal Typing ===");
    
    let duck = MyDuck { name: "Donald".to_string() };
    make_quack(&duck);
    
    // Rust: Must explicitly implement trait (nominal)
    // TypeScript: Structural typing (duck typing)
    // 
    // TypeScript equivalent:
    // interface Duck {
    //     quack(): void;
    // }
    // function makeQuack(duck: Duck): void {
    //     duck.quack();
    // }
    // // Any object with quack() works!
    // makeQuack({ quack: () => console.log("Quack!") });
}

// =============================================================================
// MAIN FUNCTION
// =============================================================================

fn main() {
    println!("TypeScript vs Rust: Side-by-Side Examples");
    println!("==========================================");

    demonstrate_generics();
    demonstrate_ownership();
    demonstrate_option();
    demonstrate_result();
    demonstrate_pattern_matching();
    demonstrate_traits();
    demonstrate_const_generics();
    demonstrate_iterators();
    demonstrate_lifetimes();
    demonstrate_typing();

    println!("\n=== Key Differences Summary ===");
    println!("1. Rust: Ownership system (unique!)");
    println!("2. Rust: No null (Option<T>)");
    println!("3. Rust: No exceptions (Result<T, E>)");
    println!("4. Rust: Pattern matching (exhaustive)");
    println!("5. Rust: Traits (more powerful than interfaces)");
    println!("6. Rust: Lifetime parameters");
    println!("7. Rust: Const generics");
    println!("8. TypeScript: Structural typing (duck typing)");
    println!("9. TypeScript: Better reflection");
    println!("10. TypeScript: Mapped types, conditional types");
    println!("11. Rust: Memory safety at compile-time");
    println!("12. TypeScript: GC manages memory");
}
