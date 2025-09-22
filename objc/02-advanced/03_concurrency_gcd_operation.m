// Objective-C Advanced: Concurrency (GCD & NSOperation)
// Groups, semaphores, and Operation dependencies

#import <Foundation/Foundation.h>

static void gcdAdvanced(void) {
    dispatch_group_t group = dispatch_group_create();
    dispatch_queue_t q = dispatch_get_global_queue(QOS_CLASS_UTILITY, 0);
    __block NSString *a, *b;
    dispatch_group_async(group, q, ^{ a = @"A"; });
    dispatch_group_async(group, q, ^{ b = @"B"; });
    dispatch_group_notify(group, dispatch_get_main_queue(), ^{ NSLog(@"%@%@", a, b); });

    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    dispatch_async(q, ^{ sleep(1); dispatch_semaphore_signal(sem); });
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
}

@interface FetchOp : NSOperation
@property (atomic, strong) NSString *result;
@end
@implementation FetchOp
- (void)main { [NSThread sleepForTimeInterval:0.1]; self.result = @"DATA"; }
@end

@interface ParseOp : NSOperation
@property (nonatomic, strong) FetchOp *dep;
@end
@implementation ParseOp
- (void)main { NSLog(@"parsed=%@", self.dep.result); }
@end

static void operationAdvanced(void) {
    NSOperationQueue *queue = [NSOperationQueue new];
    FetchOp *fetch = [FetchOp new];
    ParseOp *parse = [ParseOp new];
    parse.dep = fetch;
    [parse addDependency:fetch];
    [queue addOperations:@[fetch, parse] waitUntilFinished:YES];
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        gcdAdvanced();
        operationAdvanced();
    }
    return 0;
}
