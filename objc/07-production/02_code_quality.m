// Objective-C Production: Code Quality Utilities
#import <Foundation/Foundation.h>

#define OCLOG(fmt, ...) NSLog((@"[APP] %s:%d " fmt), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define OCASSERT(cond, msg) do { if(!(cond)){ OCLOG(@"ASSERT: %s - %@", #cond, msg); abort(); } } while(0)

int main(int argc, const char * argv[]) { @autoreleasepool { OCLOG(@"hello"); OCASSERT(1==1, @"ok"); } return 0; }
