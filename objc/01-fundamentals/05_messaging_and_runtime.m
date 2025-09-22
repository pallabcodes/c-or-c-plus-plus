// Objective-C Fundamentals: Messaging & Runtime
// objc_msgSend, categories, dynamic behavior

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

@interface Greeter : NSObject
- (NSString *)greet:(NSString *)name;
@end
@implementation Greeter
- (NSString *)greet:(NSString *)name { return [NSString stringWithFormat:@"Hello, %@", name]; }
@end

@interface Greeter (Polite)
- (NSString *)politeGreet:(NSString *)name;
@end
@implementation Greeter (Polite)
- (NSString *)politeGreet:(NSString *)name { return [NSString stringWithFormat:@"Good day, %@", name]; }
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        Greeter *g = [Greeter new];
        // Dynamic messaging
        SEL sel = @selector(greet:);
        NSString *(*send)(id, SEL, NSString *) = (void *)objc_msgSend;
        NSString *msg = send(g, sel, @"Engineer");
        NSLog(@"%@", msg);
        NSLog(@"%@", [g politeGreet:@"Reviewer"]);
    }
    return 0;
}
