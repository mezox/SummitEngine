#import <Cocoa/Cocoa.h>

#include <Engine/Window.h>
#include <Event/Signal.h>

#include <cstdint>

@interface SummitWindowDelegate : NSObject
{
    NSWindow* window;
    Application::WindowSignalsBase* signals;
}

- (instancetype)initWithWindow:(NSWindow*)window signals:(Application::WindowSignalsBase*)winsig;
@end

@interface SummitWindow : NSWindow
@end
