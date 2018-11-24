#import "SummitView.h"

#include <Logging/LoggingService.h>

#ifdef LOGGER_ID
#undef LOGGER_ID
#define LOGGER_ID "Application"
#endif

@implementation SummitRenderView
- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)wantsUpdateLayer
{
    return YES;
}

- (void)mouseDragged:(NSEvent *)event
{
}

- (void)mouseUp:(NSEvent *)event
{
}

- (void)mouseMoved:(NSEvent *)event
{
    LOG_INFORMATION("Mouse moved")
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
    LOG_INFORMATION("Key down")
}
@end
