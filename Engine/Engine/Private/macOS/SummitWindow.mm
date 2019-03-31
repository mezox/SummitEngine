#import "SummitWindow.h"

#include <Renderer/View.h>
#include <Dispatcher/SummitDispatcher.h>

using namespace Renderer;

@implementation SummitWindowDelegate

- (instancetype)initWithWindow:(NSWindow*)window signals:(Application::WindowSignalsBase*)winsig
{
    self = [super init];
    self->window = window;
    self->signals = winsig;
    
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
    
    Core::DispatcherService::Service().Post([self, &rect](){
        signals->WindowResized(rect.size.width, rect.size.height);
    });
}

- (void)windowDidMove:(NSNotification *)notification
{
    NSRect rect = [window frame];
    
    Core::DispatcherService::Service().Post([self, &rect](){
        signals->WindowMoved(rect.origin.x , rect.origin.y);
    });
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    
}

- (void)windowWillClose:(NSNotification *)notification
{
    //EventServiceLocator::Service().FireEvent(Application::WindowWillCloseEvent());
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
}

@end
