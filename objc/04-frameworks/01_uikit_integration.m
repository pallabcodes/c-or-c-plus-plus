// Objective-C Frameworks: UIKit Integration (console-friendly patterns)
#import <Foundation/Foundation.h>

@interface TableDataSource : NSObject
@property (nonatomic, copy) NSArray<NSString *> *items;
- (NSInteger)numberOfRows; - (NSString *)itemAt:(NSInteger)row;
@end
@implementation TableDataSource
- (NSInteger)numberOfRows { return (NSInteger)self.items.count; }
- (NSString *)itemAt:(NSInteger)row { return self.items[(NSUInteger)row]; }
@end

@interface ViewController : NSObject
@property (nonatomic, strong) TableDataSource *ds;
- (void)viewDidLoad; - (void)viewWillAppear; - (void)render;
@end
@implementation ViewController
- (void)viewDidLoad { NSLog(@"viewDidLoad"); }
- (void)viewWillAppear { NSLog(@"viewWillAppear"); [self render]; }
- (void)render { for (NSInteger i=0;i<self.ds.numberOfRows;i++){ NSLog(@"cell[%ld]=%@", (long)i, [self.ds itemAt:i]); } }
@end

int main(int argc, const char * argv[]) { @autoreleasepool { TableDataSource *ds=[TableDataSource new]; ds.items=@[@"A", @"B"]; ViewController *vc=[ViewController new]; vc.ds=ds; [vc viewDidLoad]; [vc viewWillAppear]; } return 0; }
