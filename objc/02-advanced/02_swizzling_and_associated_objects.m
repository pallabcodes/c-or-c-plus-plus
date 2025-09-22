// Objective-C Advanced: Swizzling & Associated Objects
// Safe swizzling with dispatch_once and method_exchangeImplementations

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

static void Swizzle(Class c, SEL original, SEL replacement) {
    Method m1 = class_getInstanceMethod(c, original);
    Method m2 = class_getInstanceMethod(c, replacement);
    if (class_addMethod(c, original, method_getImplementation(m2), method_getTypeEncoding(m2))) {
        class_replaceMethod(c, replacement, method_getImplementation(m1), method_getTypeEncoding(m1));
    } else {
        method_exchangeImplementations(m1, m2);
    }
}

@interface NSURLSession (Logging)
@end

@implementation NSURLSession (Logging)

+ (void)load {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{ Swizzle(self, @selector(dataTaskWithURL:), @selector(xxx_dataTaskWithURL:)); });
}

- (NSURLSessionDataTask *)xxx_dataTaskWithURL:(NSURL *)url {
    NSLog(@"[Swizzle] dataTaskWithURL: %@", url);
    return [self xxx_dataTaskWithURL:url]; // calls original due to exchange
}

@end

// Associated objects example
@interface NSObject (Tag)
@property (nonatomic, strong) NSString *oc_tag;
@end

@implementation NSObject (Tag)
static const void *kOCTagKey = &kOCTagKey;
- (void)setOc_tag:(NSString *)oc_tag { objc_setAssociatedObject(self, kOCTagKey, oc_tag, OBJC_ASSOCIATION_RETAIN_NONATOMIC); }
- (NSString *)oc_tag { return objc_getAssociatedObject(self, kOCTagKey); }
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSObject *obj = [NSObject new];
        obj.oc_tag = @"payload";
        NSLog(@"tag=%@", obj.oc_tag);
        [[[NSURLSession sharedSession] dataTaskWithURL:[NSURL URLWithString:@"https://apple.com"]] resume];
        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.1]];
    }
    return 0;
}
