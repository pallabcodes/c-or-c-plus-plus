// Objective-C Fundamentals: Basics (class, methods, functions, control flow)
// This file explains Objective-C syntax for a C developer.
// Key concepts: @interface/@implementation/@end (class declaration/definition terminators),
// methods marked with '-' (instance) and '+' (class), inheritance with ':' (Subclass : Superclass),
// protocols (like interfaces), categories (extend classes), literals (@"", @[], @{}),
// message sending [receiver message:arg], and Apple typedef helpers (NS_ENUM).
//
// C → Objective‑C Glossary (quick reference)
// ------------------------------------------------------------
// C/C++            | Objective‑C
// -----------------|-------------------------------------------
// struct/class     | @interface Class : SuperClass ... @end
// method (C++)     | - (ret)inst:(arg);  + (ret)classMethod;
// static method    | + (ret)classMethod;  ("class" method)
// virtual call     | [receiver selector:arg] (objc_msgSend)
// interface        | @protocol Proto <NSObject> ... @end
// extend class     | @interface Class (Category) ... @end
// enum             | typedef NS_ENUM(NSInteger, Name) { ... };
// nullptr/NULL     | nil (objects) / NULL (C pointers) / Nil (Class)
// bool/true/false  | BOOL / YES / NO
// std::string      | NSString* (immutable) / NSMutableString*
// vector/map       | NSArray*/NSDictionary* (mutable variants exist)
// lambda           | ^returnType(params) { ... }  (block)
// header/impl      | .h (usually) / .m (implementation file)
// ------------------------------------------------------------

#import <Foundation/Foundation.h>

// ===== Functions and Declarations =====
// Function declaration (prototype) — identical concept to C, but often uses Foundation typedefs
// like NSInteger (signed long on 64-bit Apple platforms) for API consistency.
static NSInteger add(NSInteger a, NSInteger b);

// Typical header-style declaration vs definition:
// In a header (.h):  int increment(int x);
// In a source  (.m):  int increment(int x) { return x + 1; }
static int increment(int x); // declaration
static int increment(int x) { return x + 1; } // definition

// Recursion example (factorial)
static unsigned long factorial(unsigned int n) { return (n <= 1) ? 1 : n * factorial(n - 1); }

// Pass-by-pointer (in/out parameter) — identical to C semantics
static void addInPlace(int *dst, int value) { if (dst) { *dst += value; } }

// Typedefs, struct, and enum declarations
// OCPoint is a plain C struct; you can use dot initializers and pass by value
// as in standard C. Objective-C interops seamlessly with C constructs.
typedef struct {
    double x;
    double y;
} OCPoint;

// NS_ENUM is a macro that produces a typed enum usable from Swift and with
// better debug info. It is still an integer-backed enum.
typedef NS_ENUM(NSInteger, OCState) {
    OCStateIdle,
    OCStateRunning,
    OCStateFinished
};

// ===== Protocols (like interfaces in other languages) =====
// A protocol declares a set of methods a class promises to implement.
@protocol Printable <NSObject>
- (NSString *)pretty; // Objective-C method: returns NSString*, takes no extra args
@end

// ===== Classes =====
// @interface declares a class: name, superclass (NSObject), adopted protocols, properties, methods.
// Note the ':' which declares inheritance (Person inherits from NSObject).
@interface Person : NSObject <Printable>
// Properties generate getter/setter methods and ivars under ARC.
// Attributes:
//  - copy: defensive copy on set (important for NSString, blocks)
//  - assign: plain assign for scalars (NSInteger)
@property (nonatomic, copy) NSString *name;
@property (nonatomic, assign) NSInteger age;

// init methods follow the pattern -(instancetype)initWith... and return self
- (instancetype)initWithName:(NSString *)name age:(NSInteger)age; // '-' means instance method
- (BOOL)isAdult; // instance method

+ (NSString *)species; // '+' means class method (like a static method in C++)
@end // @end terminates the @interface block

// @implementation defines the method bodies. self and super work like in C++/ObjC.
@implementation Person
- (instancetype)initWithName:(NSString *)name age:(NSInteger)age {
    // Designated initializer pattern
    if ((self = [super init])) { _name = [name copy]; _age = age; }
    return self;
}

- (BOOL)isAdult { return self.age >= 18; }

// Implement protocol method declared in Printable
- (NSString *)pretty { return [NSString stringWithFormat:@"%@ (%ld)", self.name, (long)self.age]; }

+ (NSString *)species { return @"H. sapiens"; }
@end // @end terminates the @implementation block

// ===== Subclassing (Inheritance) =====
// Employee inherits from Person (Employee : Person). It overrides an instance method and
// adds its own property. It can also override/extend class methods.
@interface Employee : Person
@property (nonatomic, copy) NSString *role;
@end

@implementation Employee
- (NSString *)pretty { // override instance method
    // super refers to the superclass (Person). You can call [super pretty] if desired.
    return [NSString stringWithFormat:@"%@ - %@ (%ld)", self.name, self.role, (long)self.age];
}
+ (NSString *)species { // override class method
    return @"H. sapiens (Employee)";
}
@end

// ===== Categories =====
// A category adds methods to an existing class without subclassing.
@interface NSString (Reversed)
- (NSString *)oc_reversed;
@end

@implementation NSString (Reversed)
- (NSString *)oc_reversed {
    NSUInteger len = self.length; unichar buf[len];
    [self getCharacters:buf range:NSMakeRange(0, len)];
    for (NSUInteger i=0;i<len/2;i++){ unichar t=buf[i]; buf[i]=buf[len-1-i]; buf[len-1-i]=t; }
    return [NSString stringWithCharacters:buf length:len];
}
@end

// ===== C Function Definition =====
// Free function definition (matches prototype). You can mix C and Objective-C freely.
static NSInteger add(NSInteger a, NSInteger b) { return a + b; }

// Function pointer typedef and block typedef
// AdderFn is a C function pointer; AdderBlock is an Objective-C block (closure) type.
typedef NSInteger (*AdderFn)(NSInteger, NSInteger);
typedef NSInteger (^AdderBlock)(NSInteger, NSInteger);

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // ===== Primitive Data Types (C/ObjC) =====
        // C scalars (same layout/semantics). Prefer fixed-width types for binary protocols when needed.
        char c8 = 'A';
        short s16 = -1;
        int i32 = 42;
        long l = 1234567890L; // 64-bit on macOS/iOS
        long long ll = 9223372036854775807LL;
        float f = 3.14f;
        double d = 2.71828;
        BOOL flag = YES; // Objective-C BOOL (signed char); use YES/NO
        (void)c8; (void)s16; (void)ll;

        // Objective-C typedefs commonly seen in APIs
        NSInteger idx = 5;     // signed long on 64-bit
        NSUInteger uidx = 10;  // unsigned long on 64-bit
        (void)idx; (void)uidx;

        // ===== C Arrays vs ObjC Collections =====
        // C arrays (fixed-size, no bounds checks)
        int a1[3] = {1,2,3};
        int a2[]  = {4,5,6}; // size deduced
        // 2D array (array-of-arrays)
        int a2d[2][3] = {{1,2,3},{4,5,6}};
        // Iterate C arrays
        for (size_t i=0;i<sizeof a1/sizeof a1[0];++i){ a1[i] += 1; }
        // ObjC collections (dynamic, retain elements, use literals @[]/@{})
        NSArray<NSNumber *> *nums = @[@1, @2, @3];
        NSMutableArray<NSNumber *> *mnums = [nums mutableCopy];
        [mnums addObject:@4];
        (void)a2; (void)a2d;

        // ===== C Strings vs NSString =====
        const char *cstr = "hello";               // C string (UTF-8 here)
        NSString *ns = @"hello";                   // ObjC string literal
        NSString *nsFromC = [NSString stringWithUTF8String:cstr];
        const char *backToC = [ns UTF8String]; (void)backToC; (void)nsFromC;

        // ===== Functions, Function Pointers, and Blocks =====
        int inc = increment(i32); // plain C function
        NSInteger sum1 = add(i32, a1[0]);
        AdderFn fn = &add; NSInteger sum2 = fn(5, 6);
        AdderBlock ablock = ^NSInteger(NSInteger x, NSInteger y){ return x + y; };
        NSInteger sum3 = ablock(9, 1);
        unsigned long fact5 = factorial(5);
        addInPlace(&i32, 10); // pass-by-pointer to modify caller's variable

        NSLog(@"inc=%d sums=%ld/%ld/%ld fact5=%lu i32=%d", inc, (long)sum1, (long)sum2, (long)sum3, fact5, i32);

        // ===== Control Flow =====
        // Identical to C: if/else, switch, for, while, do-while. Supports break/continue.
        if (i32 < 100) {
            NSLog(@"i32<100");
        } else if (i32 == 100) {
            NSLog(@"i32==100");
        } else {
            NSLog(@"i32>100");
        }

        int code = 2;
        switch (code) {
            case 1: NSLog(@"one"); break;
            case 2: // fall through example
            default: NSLog(@"two or other"); break; // default runs if no case matched or we fell through
        }

        for (int k=0; k<5; k++) {
            if (k == 2) continue; // skip this iteration
            if (k == 4) break;    // exit loop
            NSLog(@"k=%d", k);
        }

        int n = 0; while (n < 2) { n++; }
        do { n--; } while (n > 0);
        NSLog(@"n=%d", n);

        // ===== Object Creation, Messages, Inheritance =====
        Person *p = [[Person alloc] initWithName:@"Taylor" age:21]; // '-' instance init
        NSLog(@"%@ adult=%@", [p pretty], [p isAdult]?@"YES":@"NO");
        // '+' class (static-like) method
        NSLog(@"species(person)=%@", [Person species]);

        // Subclass usage
        Employee *e = [[Employee alloc] initWithName:@"Alex" age:30];
        e.role = @"Engineer";
        NSLog(@"%@", [e pretty]);
        NSLog(@"species(employee)=%@", [Employee species]);

        // Category method call
        NSLog(@"rev=%@", [@"hello" oc_reversed]);

        // Suppress unused warnings (demo-only values)
        (void)nums; (void)mnums; (void)ns;
    }
    return 0;
}
