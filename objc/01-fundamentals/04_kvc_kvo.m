// Objective-C Fundamentals: KVC and KVO
// Safe observing patterns

#import <Foundation/Foundation.h>

@interface Account : NSObject
@property (nonatomic) double balance;
@end
@implementation Account @end

@interface KVOController : NSObject
@property (nonatomic, strong) Account *account;
@end

@implementation KVOController

- (instancetype)init {
    if ((self = [super init])) {
        _account = [Account new];
        [_account addObserver:self forKeyPath:@"balance" options:NSKeyValueObservingOptionNew context:NULL];
    }
    return self;
}

- (void)dealloc {
    @try { [_account removeObserver:self forKeyPath:@"balance"]; } @catch(__unused NSException *e) {}
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context {
    if ([keyPath isEqualToString:@"balance"]) {
        NSLog(@"Balance updated: %@", change[NSKeyValueChangeNewKey]);
    } else {
        [super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
    }
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        KVOController *ctl = [KVOController new];
        [ctl.account setValue:@(100.0) forKey:@"balance"]; // KVC
        ctl.account.balance = 250.0; // property (KVC compliant)
    }
    return 0;
}
