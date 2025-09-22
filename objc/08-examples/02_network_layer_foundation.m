// Objective-C Examples: Foundation Network Layer with DI
#import <Foundation/Foundation.h>

@protocol HTTPClient <NSObject>
- (void)get:(NSURL *)url completion:(void(^)(NSData *data, NSError *error))completion;
@end

@interface URLSessionHTTPClient : NSObject <HTTPClient>@end
@implementation URLSessionHTTPClient
- (void)get:(NSURL *)url completion:(void (^)(NSData *, NSError *))completion { [[[NSURLSession sharedSession] dataTaskWithURL:url completionHandler:^(NSData *d, NSURLResponse *r, NSError *e){ completion(d,e);} ] resume]; [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.3]]; }
@end

@interface APIService : NSObject
@property (nonatomic, strong) id<HTTPClient> client;
- (instancetype)initWithClient:(id<HTTPClient>)client; - (void)fetch:(NSString *)path;
@end

@implementation APIService
- (instancetype)initWithClient:(id<HTTPClient>)client { if ((self=[super init])){ _client=client; } return self; }
- (void)fetch:(NSString *)path { NSURL *u=[NSURL URLWithString:[@"https://httpbin.org" stringByAppendingString:path]]; [self.client get:u completion:^(NSData *data, NSError *error){ NSLog(@"bytes=%lu err=%@", (unsigned long)data.length, error); }]; }
@end

int main(int argc, const char * argv[]) { @autoreleasepool { APIService *api=[[APIService alloc] initWithClient:[URLSessionHTTPClient new]]; [api fetch:@"/get"]; } return 0; }
