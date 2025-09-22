// Objective-C Examples: Runtime Plugin Loader
#import <Foundation/Foundation.h>
#import <objc/runtime.h>

@protocol Plugin <NSObject>
+ (NSString *)identifier; - (void)run;
@end

@interface HelloPlugin : NSObject <Plugin>@end
@implementation HelloPlugin
+ (NSString *)identifier { return @"hello"; }
- (void)run { NSLog(@"Hello plugin run"); }
@end

static NSArray<Class> *discoverPlugins(void) {
    int count = objc_getClassList(NULL, 0); Class *classes = (__unsafe_unretained Class *)malloc(sizeof(Class)*count); objc_getClassList(classes, count);
    NSMutableArray *result = [NSMutableArray array];
    Protocol *p = @protocol(Plugin);
    for (int i=0;i<count;i++){ Class c = classes[i]; if (class_conformsToProtocol(c, p)) [result addObject:c]; }
    free(classes); return result;
}

int main(int argc, const char * argv[]) { @autoreleasepool { for (Class c in discoverPlugins()){ id<Plugin> plugin = [c new]; [plugin run]; } } return 0; }
