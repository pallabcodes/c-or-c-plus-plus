// Objective-C Production: Security
#import <Foundation/Foundation.h>
#import <Security/Security.h>

static void keychainStore(void) {
    NSData *data = [@"secret" dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary *query = @{ (__bridge id)kSecClass: (__bridge id)kSecClassGenericPassword,
                             (__bridge id)kSecAttrAccount: @"token",
                             (__bridge id)kSecValueData: data };
    SecItemDelete((__bridge CFDictionaryRef)query);
    OSStatus s = SecItemAdd((__bridge CFDictionaryRef)query, NULL);
    NSLog(@"keychain status=%d", (int)s);
}

static BOOL isDebuggerAttached(void) {
    int mib[4]; struct kinfo_proc info; size_t size = sizeof(info);
    info.kp_proc.p_flag = 0; mib[0]=CTL_KERN; mib[1]=KERN_PROC; mib[2]=KERN_PROC_PID; mib[3]=getpid();
    sysctl(mib, 4, &info, &size, NULL, 0);
    return ((info.kp_proc.p_flag & P_TRACED) != 0);
}

int main(int argc, const char * argv[]) { @autoreleasepool { keychainStore(); NSLog(@"debugger=%@", isDebuggerAttached()?@"YES":@"NO"); } return 0; }
