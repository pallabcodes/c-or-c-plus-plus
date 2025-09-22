// Objective-C Frameworks: Foundation Utilities
#import <Foundation/Foundation.h>

static void jsonDemo(void) {
    NSDictionary *obj = @{ @"name": @"ObjC", @"v": @1 };
    NSData *data = [NSJSONSerialization dataWithJSONObject:obj options:0 error:nil];
    NSDictionary *back = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
    NSLog(@"JSON back=%@", back);
}

static void urlSessionDemo(void) {
    NSURL *url = [NSURL URLWithString:@"https://httpbin.org/get"];
    [[[NSURLSession sharedSession] dataTaskWithURL:url completionHandler:^(NSData *d, NSURLResponse *r, NSError *e){ NSLog(@"net bytes=%lu", (unsigned long)d.length); } ] resume];
    [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.5]];
}

static void fileIODemo(void) {
    NSString *path = [NSTemporaryDirectory() stringByAppendingPathComponent:@"sample.txt"];
    [@"payload" writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
    NSString *read = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
    NSLog(@"file=%@", read);
}

int main(int argc, const char * argv[]) { @autoreleasepool { jsonDemo(); urlSessionDemo(); fileIODemo(); } return 0; }
