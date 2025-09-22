// Objective-C Fundamentals: Blocks and GCD
// Capture semantics, memory, QoS, dispatch patterns

#import <Foundation/Foundation.h>

typedef void (^OCBlock)(void);

static void runBlocks(void) {
    __block int counter = 0; // mutable capture
    OCBlock b = ^{ counter++; };
    b(); b();
    NSLog(@"counter=%d", counter);
}

static void gcdPatterns(void) {
    dispatch_queue_t q = dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0);
    dispatch_async(q, ^{
        // Work
        NSString *result = @"done";
        dispatch_async(dispatch_get_main_queue(), ^{ NSLog(@"UI update: %@", result); });
    });

    // Barrier on concurrent queue
    dispatch_queue_t cq = dispatch_queue_create("com.example.cq", DISPATCH_QUEUE_CONCURRENT);
    dispatch_async(cq, ^{ /* R */ });
    dispatch_async(cq, ^{ /* R */ });
    dispatch_barrier_async(cq, ^{ /* W */ });
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        runBlocks();
        gcdPatterns();
    }
    return 0;
}
