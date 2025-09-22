// Objective-C Performance: Memory/CPU/UI patterns
#import <Foundation/Foundation.h>

static NSArray<NSString *> *processBatch(NSArray<NSString *> *input) {
    NSMutableArray *out = [NSMutableArray arrayWithCapacity:input.count];
    for (NSString *s in input) {
        @autoreleasepool { // bound temporaries within tight loop
            NSString *t = [s.uppercaseString stringByAppendingString:@"_"]; // temp
            [out addObject:t];
        }
    }
    return out;
}

int main(int argc, const char * argv[]) { @autoreleasepool { NSArray *in=@[@"a", @"b", @"c"]; NSLog(@"%@", processBatch(in)); } return 0; }
