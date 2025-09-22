// Objective-C Fundamentals: Syntax and Types (C-friendly commentary)
// Focus: literals (@"", @[], @{}), NSString/NSMutableString, NSNumber boxing,
// NSArray/NSDictionary (immutable/mutable), NS_ENUM, id vs instancetype, nil/NULL/Nil.

#import <Foundation/Foundation.h>

// NS_ENUM creates a typed enum (backed by NSInteger). Better for bridging & type-safety.
typedef NS_ENUM(NSInteger, OCMode) {
    OCModeOff = 0,
    OCModeOn  = 1
};

// id vs instancetype:
//  - id: dynamic object type (like void* for objects). The compiler doesn't know the class.
//  - instancetype: return type that means "an instance of the receiving class"; better than id for inits.
@interface OCSyntax : NSObject
+ (instancetype)factory; // preferred over + (id)factory
@end

@implementation OCSyntax
+ (instancetype)factory { return [self new]; }
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // ===== NSString and NSMutableString =====
        // @"..." is an Objective-C string literal (UTF-16, toll-free bridged to CFString).
        NSString *greeting = @"Hello, Objective-C";
        // NSMutableString is a mutable subclass with in-place edits.
        NSMutableString *mutable = [greeting mutableCopy];
        [mutable appendString:@"! "]; // appends to the same buffer
        [mutable appendFormat:@"year=%d", 2025];

        // ===== NSNumber (boxing/unboxing) =====
        // @42 boxes a primitive into an NSNumber*.
        NSNumber *n = @42; // same as [NSNumber numberWithInt:42]
        NSInteger iv = n.integerValue; // unboxing (convert back to primitive)
        double dv = @3.5.doubleValue; // inline boxing then unboxing

        // Booleans and chars also box: @YES, @'a'
        NSNumber *flag = @YES; (void)flag;

        // ===== NSArray / NSDictionary (immutable) =====
        // @[] and @{} are Objective-C literal syntax for arrays/dictionaries.
        NSArray<NSNumber *> *nums = @[@1, @2, @3];
        NSDictionary<NSString *, id> *dict = @{ @"key": @"value", @"count": @3 };

        // Accessing elements:
        NSNumber *first = nums[0]; // objectAtIndex:
        NSString *val = dict[@"key"]; // objectForKey:

        // ===== Mutable counterparts =====
        NSMutableArray<NSString *> *marr = [@[@"a", @"b"] mutableCopy];
        [marr addObject:@"c"]; // mutate in place
        NSMutableDictionary<NSString *, NSNumber *> *md = [@{ @"k": @1 } mutableCopy];
        md[@"k2"] = @2; // setObject:forKey:

        // ===== nil, NULL, Nil =====
        // nil: null object pointer (id) — preferred for Objective-C objects
        // NULL: null C pointer (void*, int*, etc.)
        // Nil: null class pointer (Class)
        id obj = nil; void *p = NULL; Class c = Nil; (void)obj; (void)p; (void)c;

        // ===== NSNull =====
        // Use NSNull to represent "null" inside collections (since nil can't be stored in NSArray/NSDictionary).
        NSArray *withNull = @[ [NSNull null] ]; (void)withNull;

        // ===== Fast enumeration (for-each) =====
        for (NSNumber *x in nums) { NSLog(@"x=%@", x); }

        // ===== Modeled enum usage =====
        OCMode mode = OCModeOn; if (mode == OCModeOn) { NSLog(@"mode=On"); }

        // ===== Logging and format specifiers =====
        // %@ for Objective-C objects, %ld for long/NSInteger, %.2f for double/float
        NSLog(@"%@ %@ iv=%ld dv=%.2f first=%@ val=%@", greeting, mutable, (long)iv, dv, first, val);
    }
    return 0;
}
