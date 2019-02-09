#import <Cocoa/Cocoa.h>

#include <Engine/Window.h>

@interface SummitWindowDelegate : NSObject
{
    NSWindow* window;
}
- (instancetype)initWithWindow:(NSWindow*)window;
@end

@interface SummitWindow : NSWindow
@end
