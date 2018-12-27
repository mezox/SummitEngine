#import "SummitWindow.h"

@implementation SummitWindowDelegate
- (BOOL)windowShouldClose:(id)sender
{
    return NO;
}

- (void)windowDidResize:(NSNotification *)notification
{
    
}

- (void)windowDidMove:(NSNotification *)notification
{
    
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    
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
