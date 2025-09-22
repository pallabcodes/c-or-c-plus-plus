// Objective-C Testing: XCTest example (outline)
#import <XCTest/XCTest.h>

@interface MathTests : XCTestCase
@end

@implementation MathTests
- (void)testAddition { XCTAssertEqual(2+2, 4); }
- (void)testAsyncExpectation {
    XCTestExpectation *exp = [self expectationWithDescription:@"async"]; dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{ [exp fulfill]; });
    [self waitForExpectationsWithTimeout:1 handler:nil];
}
@end
