// Objective-C Patterns: Observer (KVO & NotificationCenter)
#import <Foundation/Foundation.h>

static NSString * const kUserDidLogin = @"UserDidLogin";

@interface Session : NSObject
@property (nonatomic, copy) NSString *token;
@end
@implementation Session @end

@interface ObserverDemo : NSObject
@property (nonatomic, strong) Session *session;
@end

@implementation ObserverDemo
- (instancetype)init { if ((self = [super init])) { _session = [Session new]; [_session addObserver:self forKeyPath:@"token" options:NSKeyValueObservingOptionNew context:NULL]; [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onLogin:) name:kUserDidLogin object:nil]; } return self; }
- (void)dealloc { @try { [_session removeObserver:self forKeyPath:@"token"]; } @catch(__unused NSException *e) {} [[NSNotificationCenter defaultCenter] removeObserver:self]; }
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary<NSKeyValueChangeKey,id> *)change context:(void *)context { if ([keyPath isEqualToString:@"token"]) { NSLog(@"KVO token=%@", change[NSKeyValueChangeNewKey]); } }
- (void)onLogin:(NSNotification *)n { NSLog(@"Notification login payload=%@", n.userInfo); }
@end

int main(int argc, const char * argv[]) { @autoreleasepool { ObserverDemo *d = [ObserverDemo new]; d.session.token = @"abc"; [[NSNotificationCenter defaultCenter] postNotificationName:kUserDidLogin object:nil userInfo:@{ @"u": @"id" }]; (void)d; } return 0; }
