// Objective-C Fundamentals: Memory Management (ARC)
// Production-focused patterns: ARC, autorelease pools, bridging, ownership

#import <Foundation/Foundation.h>

@interface OCMM : NSObject
@property (nonatomic, strong) NSString *strongProp;      // owns
@property (nonatomic, weak) NSObject *weakDelegate;      // non-owning
@property (nonatomic, copy) NSString *copyProp;          // value semantics for NSString/blocks
@end

@implementation OCMM
@end

static void bridgingDemo(void) {
    CFStringRef cf = CFStringCreateWithCString(kCFAllocatorDefault, "Hello", kCFStringEncodingUTF8);
    // Toll-free bridging to ARC-managed NSString
    NSString *bridged = CFBridgingRelease(cf); // transfers ownership to ARC
    NSLog(@"Bridged: %@", bridged);
}

static void autoreleasePoolDemo(void) {
    @autoreleasepool {
        NSMutableArray *tmp = [NSMutableArray array]; // autoreleased creator
        for (int i = 0; i < 10000; i++) {
            @autoreleasepool { // inner pool to bound temporary lifetimes
                NSString *s = [NSString stringWithFormat:@"%d", i];
                [tmp addObject:s];
            }
        }
        NSLog(@"Count %lu", (unsigned long)tmp.count);
    }
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        OCMM *obj = [OCMM new];
        obj.strongProp = @"Owned";
        obj.copyProp = [@"mutable" mutableCopy];
        bridgingDemo();
        autoreleasePoolDemo();
        NSLog(@"Props: %@ %@", obj.strongProp, obj.copyProp);
    }
    return 0;
}
