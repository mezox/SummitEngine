#import "SummitView.h"

#include <Logging/LoggingService.h>

#ifdef LOGGER_ID
#undef LOGGER_ID
#define LOGGER_ID "Application"
#endif

@implementation SummitRenderView
- (instancetype)initWithBounds:(NSRect)bounds
{
    self = [super init];
    if (self != nil)
    {
        trackingArea = nil;
        [self setFrame:bounds];
        [self setBounds:bounds];
        [self updateTrackingAreas];
    }
    
    return self;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)wantsUpdateLayer
{
    return YES;
}


//- (BOOL)acceptsFirstResponder
//{
//    return YES;
//}

- (void)updateTrackingAreas
{
    if (trackingArea != nil)
    {
        [self removeTrackingArea:trackingArea];
        //[trackingArea release];
    }
    
    const NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited |
    NSTrackingActiveInKeyWindow |
    NSTrackingEnabledDuringMouseDrag |
    NSTrackingMouseMoved |
    NSTrackingCursorUpdate |
    NSTrackingInVisibleRect |
    NSTrackingAssumeInside;
    
    auto bounds = [self bounds];
    trackingArea = [[NSTrackingArea alloc] initWithRect:bounds
                                                options:options
                                                  owner:self
                                               userInfo:nil];
    
    [self addTrackingArea:trackingArea];
    [super updateTrackingAreas];
}

- (void)mouseEntered:(NSEvent *)event
{
    NSLog(@"Mouse entered view");
}

- (void)mouseDragged:(NSEvent *)event
{
}

- (void)mouseUp:(NSEvent *)event
{
}

- (void)mouseMoved:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    CGRect rect = [self bounds];
    
    NSLog(@"mouseMoved: %f %f", point.x - rect.origin.x, point.y);
}

- (void)rightMouseDown:(NSEvent *)event
{
}

- (void)rightMouseDragged:(NSEvent *)event
{
}

- (void)rightMouseUp:(NSEvent *)event
{
}

- (void)keyDown:(NSEvent *)event
{
}
@end
