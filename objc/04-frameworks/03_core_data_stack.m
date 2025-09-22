// Objective-C Frameworks: Core Data Stack (minimal)
#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

static NSPersistentContainer *MakeContainer(void) {
    NSPersistentContainer *container = [[NSPersistentContainer alloc] initWithName:@"Model"]; // requires xcdatamodel in app; example shows API shape
    [container loadPersistentStoresWithCompletionHandler:^(NSPersistentStoreDescription *d, NSError *e){ if(e){ NSLog(@"CoreData error=%@", e); } }];
    return container;
}

int main(int argc, const char * argv[]) { @autoreleasepool { NSPersistentContainer *c=MakeContainer(); (void)c; NSLog(@"Core Data container ready (if model present)"); } return 0; }
