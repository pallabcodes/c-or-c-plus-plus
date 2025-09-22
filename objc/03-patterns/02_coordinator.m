// Objective-C Patterns: Coordinator
#import <Foundation/Foundation.h>

@protocol Coordinating <NSObject>
- (void)start;
@end

@interface FlowCoordinator : NSObject <Coordinating>
@property (nonatomic, copy) void (^onFinish)(void);
@end
@implementation FlowCoordinator
- (void)start { NSLog(@"Coordinator start"); if (self.onFinish) self.onFinish(); }
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        FlowCoordinator *c = [FlowCoordinator new];
        c.onFinish = ^{ NSLog(@"Coordinator finished"); };
        [c start];
    }
    return 0;
}
