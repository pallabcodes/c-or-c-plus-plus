// Objective-C Frameworks: Swift Bridging Notes
// Use bridging header to expose ObjC to Swift; annotate nullability and generics for better Swift APIs.

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface Box<__covariant T> : NSObject
@property (nonatomic, strong, readonly) T value;
- (instancetype)initWithValue:(T)value;
@end

@implementation Box
- (instancetype)initWithValue:(id)value { if ((self = [super init])) { _value = value; } return self; }
@end

@interface Greeter : NSObject
- (NSString *)greet:(NSString *)name error:(NSError * __autoreleasing _Nullable * _Nullable)error;
@end

@implementation Greeter
- (NSString *)greet:(NSString *)name error:(NSError * __autoreleasing _Nullable * _Nullable)error { return [NSString stringWithFormat:@"Hello, %@", name]; }
@end

NS_ASSUME_NONNULL_END

int main(int argc, const char * argv[]) { @autoreleasepool { Greeter *g=[Greeter new]; NSLog(@"%@", [g greet:@"Swift" error:nil]); } return 0; }
