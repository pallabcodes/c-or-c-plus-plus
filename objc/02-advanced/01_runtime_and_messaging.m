// Objective-C Advanced: Runtime & Messaging Deep Dive
// Dynamic method resolution, forwarding targets, full invocation forwarding

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

@interface DynamicResponder : NSObject
@end

@implementation DynamicResponder

+ (BOOL)resolveInstanceMethod:(SEL)sel {
    if (sel == @selector(dynamicSay:)) {
        IMP imp = imp_implementationWithBlock(^NSString *(id _self, NSString *name){
            return [NSString stringWithFormat:@"(dynamic) Hello, %@", name];
        });
        const char *types = "@@:@"; // returns id, self+_cmd+arg
        return class_addMethod(self, sel, imp, types);
    }
    return [super resolveInstanceMethod:sel];
}

- (id)forwardingTargetForSelector:(SEL)aSelector {
    // Could return a lightweight forward target for simple proxies
    return [super forwardingTargetForSelector:aSelector];
}

- (NSMethodSignature *)methodSignatureForSelector:(SEL)aSelector {
    if (aSelector == @selector(fullForward:)) {
        return [NSMethodSignature signatureWithObjCTypes:"v@:@"]; // void self:_cmd:arg
    }
    return [super methodSignatureForSelector:aSelector];
}

- (void)forwardInvocation:(NSInvocation *)invocation {
    SEL sel = invocation.selector;
    if (sel == @selector(fullForward:)) {
        __unsafe_unretained NSString *arg = nil;
        [invocation getArgument:&arg atIndex:2];
        NSLog(@"(forward) Received: %@", arg);
        return;
    }
    [super forwardInvocation:invocation];
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        DynamicResponder *d = [DynamicResponder new];
        // Dynamic method resolution
        NSString *msg = ((id (*)(id, SEL, id))objc_msgSend)(d, @selector(dynamicSay:), @"Engineer");
        NSLog(@"%@", msg);
        // Full forwarding path
        ((void (*)(id, SEL, id))objc_msgSend)(d, @selector(fullForward:), @"payload");
    }
    return 0;
}
