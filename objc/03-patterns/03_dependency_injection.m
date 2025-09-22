// Objective-C Patterns: Dependency Injection
#import <Foundation/Foundation.h>

@protocol NetworkServicing <NSObject>
- (void)get:(NSString *)path completion:(void(^)(NSData *data))completion;
@end

@interface NetworkService : NSObject <NetworkServicing>
@end
@implementation NetworkService
- (void)get:(NSString *)path completion:(void (^)(NSData * _Nonnull))completion { completion([path dataUsingEncoding:NSUTF8StringEncoding]); }
@end

@interface Repository : NSObject
@property (nonatomic, strong) id<NetworkServicing> service; // property injection
- (instancetype)initWithService:(id<NetworkServicing>)service; // initializer DI
- (void)fetch;
@end

@implementation Repository
- (instancetype)initWithService:(id<NetworkServicing>)service { if ((self = [super init])) { _service = service; } return self; }
- (void)fetch { [self.service get:@"/items" completion:^(NSData *data) { NSLog(@"Fetched: %lu bytes", (unsigned long)data.length); }]; }
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        id<NetworkServicing> svc = [NetworkService new];
        Repository *repo = [[Repository alloc] initWithService:svc];
        [repo fetch];
    }
    return 0;
}
