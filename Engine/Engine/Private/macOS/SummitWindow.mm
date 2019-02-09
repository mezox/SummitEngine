#import "SummitWindow.h"

#include <Event/EventService.h>
#include <Engine/WindowEvent.h>

using namespace Event;

@implementation SummitWindowDelegate
- (instancetype)initWithWindow:(NSWindow*)window
{
    self = [super init];
    self->window = window;
    
    return self;
}

- (BOOL)windowShouldClose:(id)sender
{
    return YES;
}

- (BOOL)releasedWhenClosed:(id)sender
{
    return YES;
}

- (void)windowDidResize:(NSNotification *)notification
{
    NSRect rect = [window.contentView frame];
    //EventServiceLocator::Service().FireEvent(Application::WindowResizeEvent(rect.size.width, rect.size.height));
}

- (void)windowDidMove:(NSNotification *)notification
{
    NSRect rect = [window frame];
    EventServiceLocator::Service().FireEvent(Application::WindowMoveEvent(rect.origin.x , rect.origin.y));
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    
}

- (void)windowWillClose:(NSNotification *)notification
{
    EventServiceLocator::Service().FireEvent(Application::WindowWillCloseEvent());
}
@end

@implementation SummitWindow
- (BOOL)canBecomeKeyWindow
{
    // Required for NSWindowStyleMaskBorderless windows
    return YES;
}

- (BOOL)canBecomeMainWindow
{
    return YES;
}

- (void)mouseMoved:(NSEvent *)event
{
//    //LOG_INFORMATION("Mouse moved")
//    NSPoint locationInView = [event locationInWindow];
//    NSLog(@"mouseMoved: %f %f", locationInView.x, locationInView.y);
}

@end
