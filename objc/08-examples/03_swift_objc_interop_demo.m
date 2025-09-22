// Objective-C Examples: Swift/ObjC Interop Demo
#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN
@interface Cache<KeyType, ObjectType> : NSObject
- (void)setObject:(ObjectType)obj forKey:(KeyType<NSCopying>)key;
- (nullable ObjectType)objectForKey:(KeyType)key;
@end

@implementation Cache { NSCache *_cache; }
- (instancetype)init { if ((self=[super init])){ _cache=[NSCache new]; } return self; }
- (void)setObject:(id)obj forKey:(id<NSCopying>)key { [_cache setObject:obj forKey:key]; }
- (id)objectForKey:(id)key { return [_cache objectForKey:key]; }
@end
NS_ASSUME_NONNULL_END

int main(int argc, const char * argv[]) { @autoreleasepool { Cache<NSString *, NSData *> *c=[Cache new]; [c setObject:[@"x" dataUsingEncoding:NSUTF8StringEncoding] forKey:@"k"]; NSLog(@"has=%@", [c objectForKey:@"k"]?@"YES":@"NO"); } return 0; }
