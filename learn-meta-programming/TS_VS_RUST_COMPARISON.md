# TypeScript vs Rust: Comprehensive Comparison

## Table of Contents
1. [Generics Comparison](#generics-comparison)
2. [What TypeScript Has That Rust Doesn't](#what-typescript-has-that-rust-doesnt)
3. [What Rust Has That TypeScript Doesn't](#what-rust-has-that-typescript-doesnt)
4. [Feature-by-Feature Comparison](#feature-by-feature-comparison)
5. [When to Use Each](#when-to-use-each)

## Generics Comparison

### Rust Generics
```rust
// Function generics
fn max<T: PartialOrd>(a: T, b: T) -> T {
    if a > b { a } else { b }
}

// Struct generics
struct Vector<T> {
    data: Vec<T>,
    size: usize,
}

// Trait bounds (constraints)
fn process<T: Clone + Debug>(value: T) {
    // T must implement Clone and Debug traits
}
```

### TypeScript Generics
```typescript
// Generic function
function max<T>(a: T, b: T): T {
    return a > b ? a : b;
}

// Generic class
class Vector<T> {
    private data: T[];
    private size: number;
}

// Generic constraints
function process<T extends Clone & Debug>(value: T): void {
    // T must have Clone and Debug properties
}
```

### Key Differences

| Feature | Rust Generics | TypeScript Generics |
|---------|---------------|---------------------|
| **Constraints** | Traits (`T: Clone + Debug`) | `extends` keyword |
| **When evaluated** | Compile-time | Type-checking time |
| **Runtime presence** | Monomorphized (code generated) | Erased at runtime |
| **Performance** | Zero overhead | No runtime cost |
| **Trait bounds** | ✅ Powerful trait system | ⚠️ Limited to interfaces |
| **Lifetime parameters** | ✅ `<'a>` | ❌ Not available |

## What TypeScript Has That Rust Doesn't

### 1. **Structural Typing (Duck Typing)**
```typescript
// TypeScript: If it has the right shape, it works
interface Duck {
    quack(): void;
}

function makeQuack(duck: Duck) {
    duck.quack();
}

// Any object with quack() works
makeQuack({ quack: () => console.log("Quack!") });
```

```rust
// Rust: Nominal typing - must explicitly implement trait
trait Duck {
    fn quack(&self);
}

fn make_quack(duck: &impl Duck) {
    duck.quack();
}

// Must explicitly implement trait
struct MyDuck;
impl Duck for MyDuck {
    fn quack(&self) {
        println!("Quack!");
    }
}
```

### 2. **Union Types (Native)**
```typescript
// TypeScript: Native union types
type StringOrNumber = string | number;

function process(value: StringOrNumber) {
    if (typeof value === 'string') {
        return value.toUpperCase();
    } else {
        return value * 2;
    }
}
```

```rust
// Rust: Must use enums
enum StringOrNumber {
    String(String),
    Number(i32),
}

fn process(value: StringOrNumber) {
    match value {
        StringOrNumber::String(s) => println!("{}", s.to_uppercase()),
        StringOrNumber::Number(n) => println!("{}", n * 2),
    }
}
```

### 3. **Intersection Types**
```typescript
// TypeScript: Intersection types
type A = { a: number };
type B = { b: string };
type C = A & B;  // { a: number, b: string }
```

```rust
// Rust: Must use multiple trait bounds or composition
trait A {
    fn a(&self) -> i32;
}

trait B {
    fn b(&self) -> String;
}

// Use multiple trait bounds
fn process<T: A + B>(value: T) {
    // value implements both A and B
}
```

### 4. **Mapped Types**
```typescript
// TypeScript: Transform types
type Readonly<T> = {
    readonly [P in keyof T]: T[P];
};

type Partial<T> = {
    [P in keyof T]?: T[P];
};

type Pick<T, K extends keyof T> = {
    [P in K]: T[P];
};
```

```rust
// Rust: No direct equivalent
// Must manually create new structs or use macros
```

### 5. **Template Literal Types**
```typescript
// TypeScript: String manipulation at type level
type EventName = "click" | "scroll" | "hover";
type HandlerName<T extends EventName> = `on${Capitalize<T>}`;
// Result: "onClick" | "onScroll" | "onHover"
```

```rust
// Rust: Not available - strings are runtime values
// Can use const generics for some cases but limited
```

### 6. **Conditional Types**
```typescript
// TypeScript: Type-level conditionals
type IsArray<T> = T extends any[] ? true : false;
type Flatten<T> = T extends (infer U)[] ? U : T;
type NonNullable<T> = T extends null | undefined ? never : T;
```

```rust
// Rust: Use trait bounds and associated types
trait IsArray {
    type Output;
}

impl<T> IsArray for Vec<T> {
    type Output = T;
}
```

### 7. **Type Guards**
```typescript
// TypeScript: Runtime type narrowing
function isString(value: unknown): value is string {
    return typeof value === 'string';
}

function process(value: unknown) {
    if (isString(value)) {
        // TypeScript knows value is string here
        console.log(value.toUpperCase());
    }
}
```

```rust
// Rust: Pattern matching (more powerful)
fn process(value: &dyn std::any::Any) {
    if let Some(s) = value.downcast_ref::<String>() {
        println!("{}", s.to_uppercase());
    }
}
```

### 8. **Null Safety (Optional Chaining)**
```typescript
// TypeScript: Optional chaining
const result = obj?.property?.method?.();
const value = arr?.[0]?.name;
```

```rust
// Rust: Option<T> with pattern matching
let result = obj
    .and_then(|o| o.property)
    .and_then(|p| p.method());
```

### 9. **Better Reflection**
```typescript
// TypeScript: Excellent reflection support
type Keys<T> = keyof T;
type Values<T> = T[keyof T];
type Entries<T> = { [K in keyof T]: [K, T[K]] }[keyof T][];

interface Person {
    name: string;
    age: number;
}

type PersonKeys = Keys<Person>;  // "name" | "age"
```

```rust
// Rust: Limited reflection (use macros or manual work)
// No built-in keyof equivalent
```

### 10. **Dynamic Types**
```typescript
// TypeScript: Can use `any` or `unknown`
function process(value: any) {
    // Can do anything with value
}

function safeProcess(value: unknown) {
    // Must check type before using
    if (typeof value === 'string') {
        console.log(value);
    }
}
```

```rust
// Rust: Must use trait objects or Any
use std::any::Any;

fn process(value: &dyn Any) {
    // Must downcast to use
    if let Some(s) = value.downcast_ref::<String>() {
        println!("{}", s);
    }
}
```

## What Rust Has That TypeScript Doesn't

### 1. **Ownership and Borrowing (UNIQUE TO RUST)**
```rust
// Rust: Ownership system prevents memory issues
fn take_ownership(s: String) {
    println!("{}", s);
}  // s is dropped here

fn borrow(s: &String) {
    println!("{}", s);
}  // s is NOT dropped, just borrowed

let s = String::from("hello");
take_ownership(s);  // s moved, can't use after
// println!("{}", s);  // ERROR: s no longer valid

let s2 = String::from("world");
borrow(&s2);  // s2 borrowed, still valid
println!("{}", s2);  // OK: s2 still valid
```

```typescript
// TypeScript: Garbage collected, no ownership concept
function takeOwnership(s: string): void {
    console.log(s);
}

const s = "hello";
takeOwnership(s);
console.log(s);  // OK: s still valid (GC manages memory)
```

### 2. **Pattern Matching**
```rust
// Rust: Powerful pattern matching
enum Option<T> {
    Some(T),
    None,
}

fn process(opt: Option<i32>) {
    match opt {
        Option::Some(value) => println!("Got {}", value),
        Option::None => println!("Nothing"),
    }
    
    // Destructuring
    if let Option::Some(x) = opt {
        println!("Value is {}", x);
    }
}
```

```typescript
// TypeScript: Limited pattern matching
type Option<T> = { kind: "some"; value: T } | { kind: "none" };

function process(opt: Option<number>): void {
    if (opt.kind === "some") {
        console.log("Got", opt.value);
    } else {
        console.log("Nothing");
    }
}
```

### 3. **Traits (More Powerful Than Interfaces)**
```rust
// Rust: Traits with default implementations
trait Animal {
    fn name(&self) -> &str;
    fn speak(&self) {
        println!("{} makes a sound", self.name());
    }
}

trait Fly {
    fn fly(&self);
}

// Multiple trait implementation
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
```

```typescript
// TypeScript: Interfaces (less powerful)
interface Animal {
    name(): string;
    speak(): void;
}

interface Fly {
    fly(): void;
}

class Bird implements Animal, Fly {
    constructor(private name: string) {}
    
    name(): string {
        return this.name;
    }
    
    speak(): void {
        console.log(`${this.name()} makes a sound`);
    }
    
    fly(): void {
        console.log(`${this.name()} is flying`);
    }
}
```

### 4. **Zero-Cost Abstractions**
```rust
// Rust: Zero-cost abstractions
trait Iterator {
    type Item;
    fn next(&mut self) -> Option<Self::Item>;
}

// This compiles to efficient code, no overhead
let sum: i32 = (1..10)
    .map(|x| x * 2)
    .filter(|x| x > 5)
    .sum();
```

```typescript
// TypeScript: Runtime overhead (though optimized by engines)
const sum = Array.from({ length: 10 }, (_, i) => i + 1)
    .map(x => x * 2)
    .filter(x => x > 5)
    .reduce((a, b) => a + b, 0);
```

### 5. **No Null (Option<T>)**
```rust
// Rust: No null, use Option<T>
fn find_user(id: i32) -> Option<User> {
    if id > 0 {
        Some(User { id })
    } else {
        None
    }
}

// Must handle None case
match find_user(42) {
    Some(user) => println!("Found user {}", user.id),
    None => println!("User not found"),
}
```

```typescript
// TypeScript: Can use null/undefined
function findUser(id: number): User | null {
    return id > 0 ? { id } : null;
}

// Can forget to check null
const user = findUser(42);
console.log(user.id);  // Runtime error if null!
```

### 6. **No Exceptions (Result<T, E>)**
```rust
// Rust: Result<T, E> instead of exceptions
fn divide(a: i32, b: i32) -> Result<i32, String> {
    if b == 0 {
        Err("Division by zero".to_string())
    } else {
        Ok(a / b)
    }
}

// Must handle errors explicitly
match divide(10, 2) {
    Ok(result) => println!("Result: {}", result),
    Err(e) => println!("Error: {}", e),
}
```

```typescript
// TypeScript: Exceptions (can be forgotten)
function divide(a: number, b: number): number {
    if (b === 0) {
        throw new Error("Division by zero");
    }
    return a / b;
}

// Can forget try-catch
const result = divide(10, 2);  // Might throw!
```

### 7. **Lifetime Parameters**
```rust
// Rust: Explicit lifetimes
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() {
        x
    } else {
        y
    }
}

// Ensures returned reference is valid
```

```typescript
// TypeScript: No lifetime concept (GC manages)
function longest(x: string, y: string): string {
    return x.length > y.length ? x : y;
}
```

### 8. **Macros (More Powerful)**
```rust
// Rust: Declarative and procedural macros
macro_rules! vec {
    ($($x:expr),*) => {
        {
            let mut temp_vec = Vec::new();
            $(temp_vec.push($x);)*
            temp_vec
        }
    };
}

let v = vec![1, 2, 3];  // Macro expands at compile-time
```

```typescript
// TypeScript: No macros (use build tools like Babel)
// Can't define custom syntax
```

### 9. **Algebraic Data Types**
```rust
// Rust: Enums with data (ADTs)
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(i32, i32, i32),
}

fn process(msg: Message) {
    match msg {
        Message::Quit => println!("Quit"),
        Message::Move { x, y } => println!("Move to ({}, {})", x, y),
        Message::Write(s) => println!("Write: {}", s),
        Message::ChangeColor(r, g, b) => println!("Color: RGB({}, {}, {})", r, g, b),
    }
}
```

```typescript
// TypeScript: Discriminated unions (similar but less ergonomic)
type Message =
    | { kind: "quit" }
    | { kind: "move"; x: number; y: number }
    | { kind: "write"; text: string }
    | { kind: "changeColor"; r: number; g: number; b: number };

function process(msg: Message): void {
    switch (msg.kind) {
        case "quit":
            console.log("Quit");
            break;
        case "move":
            console.log(`Move to (${msg.x}, ${msg.y})`);
            break;
        // ...
    }
}
```

### 10. **Memory Safety Without GC**
```rust
// Rust: Memory safety at compile-time
fn no_dangling_pointer() {
    let r;
    {
        let x = 5;
        r = &x;  // ERROR: x doesn't live long enough
    }
    println!("{}", r);  // ERROR: r is invalid
}
```

```typescript
// TypeScript: GC manages memory (runtime overhead)
function noDanglingPointer(): void {
    let r: number | undefined;
    {
        const x = 5;
        r = x;  // OK: GC will manage
    }
    console.log(r);  // OK: GC keeps value alive
}
```

### 11. **Const Generics**
```rust
// Rust: Const generics (Rust 1.51+)
struct Array<T, const N: usize> {
    data: [T; N],
}

impl<T, const N: usize> Array<T, N> {
    fn new() -> Self {
        // N is known at compile-time
    }
}

let arr: Array<i32, 10> = Array::new();
```

```typescript
// TypeScript: Can't use values as type parameters
// Can only use types
```

### 12. **Unsafe Code**
```rust
// Rust: Can opt into unsafe when needed
unsafe fn dangerous() {
    // Can do unsafe operations
    let raw_ptr = 0x1234 as *const i32;
    // Must be explicit about unsafe
}
```

```typescript
// TypeScript: No unsafe code (everything is safe)
// Can't do low-level operations
```

## Feature-by-Feature Comparison

### Type System

| Feature | Rust | TypeScript |
|---------|------|------------|
| **Static typing** | ✅ | ✅ |
| **Type inference** | ✅ (Excellent) | ✅ (Excellent) |
| **Structural typing** | ❌ (Nominal) | ✅ (Duck typing) |
| **Union types** | ✅ (Enums) | ✅ (Native) |
| **Intersection types** | ✅ (Trait bounds) | ✅ (Native) |
| **Null safety** | ✅ (Option<T>) | ✅ (Strict null checks) |
| **Lifetime tracking** | ✅ (Unique!) | ❌ |

### Generics

| Feature | Rust | TypeScript |
|---------|------|------------|
| **Generics** | ✅ | ✅ |
| **Type parameters** | ✅ | ✅ |
| **Value parameters** | ✅ (Const generics) | ❌ |
| **Constraints** | ✅ (Traits) | ✅ (extends) |
| **Lifetime parameters** | ✅ (`<'a>`) | ❌ |
| **Associated types** | ✅ | ❌ |
| **Code generation** | ✅ (Monomorphization) | ❌ (Erased) |

### Memory Management

| Feature | Rust | TypeScript |
|---------|------|------------|
| **Garbage collection** | ❌ | ✅ (JS runtime) |
| **Ownership** | ✅ (Unique!) | ❌ |
| **Borrowing** | ✅ (Unique!) | ❌ |
| **RAII** | ✅ | ❌ |
| **Move semantics** | ✅ | ❌ |
| **Memory safety** | ✅ (Compile-time) | ✅ (Runtime GC) |

### Error Handling

| Feature | Rust | TypeScript |
|---------|------|------------|
| **Exceptions** | ❌ | ✅ |
| **Result type** | ✅ (`Result<T, E>`) | ⚠️ (Manual) |
| **Option type** | ✅ (`Option<T>`) | ⚠️ (`T | null`) |
| **Must handle errors** | ✅ (Compile-time) | ❌ (Optional) |

### Pattern Matching

| Feature | Rust | TypeScript |
|---------|------|------------|
| **Pattern matching** | ✅ (Excellent) | ⚠️ (Limited) |
| **Destructuring** | ✅ | ✅ |
| **Exhaustiveness checking** | ✅ | ⚠️ (Limited) |
| **Match expressions** | ✅ | ❌ (switch only) |

### Metaprogramming

| Feature | Rust | TypeScript |
|---------|------|------------|
| **Macros** | ✅ (Powerful) | ❌ |
| **Compile-time computation** | ✅ (const fn) | ❌ |
| **Type-level programming** | ⚠️ (Limited) | ✅ (Excellent) |
| **Reflection** | ⚠️ (Limited) | ✅ (Excellent) |

## When to Use Each

### Use Rust When:
- **Systems programming** - Operating systems, drivers, embedded
- **Performance critical** - Zero-cost abstractions, no GC
- **Memory safety required** - Ownership prevents memory bugs
- **Concurrency** - Fearless concurrency with ownership
- **WebAssembly** - Compile to WASM for web performance
- **Blockchain** - Many blockchain projects use Rust

### Use TypeScript When:
- **Web development** - Browser APIs, Node.js
- **Rapid prototyping** - Faster development cycle
- **Type safety with flexibility** - Structural typing, unions
- **Better reflection** - Type introspection, mapped types
- **Developer experience** - Better error messages, tooling
- **Existing JavaScript codebase** - Gradual migration path

## Summary

### Rust Strengths:
- ✅ **Memory safety**: Ownership prevents bugs at compile-time
- ✅ **Performance**: Zero-cost abstractions, no GC overhead
- ✅ **Concurrency**: Fearless concurrency with ownership
- ✅ **Pattern matching**: Powerful and exhaustive
- ✅ **Error handling**: Must handle errors (Result<T, E>)
- ✅ **No null**: Option<T> prevents null pointer errors

### TypeScript Strengths:
- ✅ **Type system**: Structural typing, unions, intersections
- ✅ **Reflection**: Excellent type introspection
- ✅ **Developer experience**: Better tooling, error messages
- ✅ **Flexibility**: Runtime type checks, dynamic types
- ✅ **Ecosystem**: Huge npm ecosystem
- ✅ **Web**: Native browser/Node.js support

### Key Insight:
- **Rust**: Memory safety + Performance (no GC, ownership system)
- **TypeScript**: Developer experience + Type safety (GC, structural typing)

Both are excellent languages but serve different purposes:
- **Rust**: Systems programming, performance, memory safety
- **TypeScript**: Web development, rapid development, type safety
