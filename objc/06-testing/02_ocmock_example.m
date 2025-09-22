// Objective-C Testing: OCMock example (pseudocode outline)
// Requires OCMock library when running in project
#import <Foundation/Foundation.h>

@protocol GreeterServicing <NSObject>
- (NSString *)greet:(NSString *)name;
@end

@interface GreeterClient : NSObject
@property (nonatomic, strong) id<GreeterServicing> service;
- (NSString *)hello:(NSString *)name;
@end

@implementation GreeterClient
- (NSString *)hello:(NSString *)name { return [self.service greet:name]; }
@end

// In a real test:
// id mock = OCMProtocolMock(@protocol(GreeterServicing));
// OCMStub([mock greet:@"Ada"]).andReturn(@"Hello, Ada");
// GreeterClient *c=[GreeterClient new]; c.service=mock; XCTAssertEqualObjects([c hello:@"Ada"], @"Hello, Ada");
