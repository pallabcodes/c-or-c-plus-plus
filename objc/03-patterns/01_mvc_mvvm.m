// Objective-C Patterns: MVC and MVVM (conceptual, console-friendly)
#import <Foundation/Foundation.h>

// Model
@interface User : NSObject
@property (nonatomic, copy) NSString *name;
@property (nonatomic, assign) NSInteger age;
@end
@implementation User @end

// ViewModel
@interface UserViewModel : NSObject
@property (nonatomic, strong) User *user;
@property (nonatomic, copy) void (^onDisplayNameChanged)(NSString *displayName);
- (instancetype)initWithUser:(User *)user;
- (void)updateName:(NSString *)name;
@end

@implementation UserViewModel
- (instancetype)initWithUser:(User *)user { if ((self = [super init])) { _user = user; } return self; }
- (void)updateName:(NSString *)name { self.user.name = name; if (self.onDisplayNameChanged) { self.onDisplayNameChanged([self displayName]); } }
- (NSString *)displayName { return [NSString stringWithFormat:@"%@ (%ld)", self.user.name, (long)self.user.age]; }
@end

// Controller (acts as ViewController)
@interface UserController : NSObject
@property (nonatomic, strong) UserViewModel *vm;
- (instancetype)initWithViewModel:(UserViewModel *)vm;
- (void)viewDidLoad;
@end

@implementation UserController
- (instancetype)initWithViewModel:(UserViewModel *)vm { if ((self = [super init])) { _vm = vm; } return self; }
- (void)viewDidLoad {
    __weak typeof(self) weakSelf = self;
    self.vm.onDisplayNameChanged = ^(NSString *displayName) { NSLog(@"Render label: %@", displayName); (void)weakSelf; };
    // Initial render
    if (self.vm.onDisplayNameChanged) { self.vm.onDisplayNameChanged([self.vm displayName]); }
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        User *u = [User new]; u.name = @"Taylor"; u.age = 28;
        UserViewModel *vm = [[UserViewModel alloc] initWithUser:u];
        UserController *vc = [[UserController alloc] initWithViewModel:vm];
        [vc viewDidLoad];
        [vm updateName:@"Jordan"];
    }
    return 0;
}
