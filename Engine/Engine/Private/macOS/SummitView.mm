#import "SummitView.h"
#import <QuartzCore/CAMetalLayer.h>

#include <Dispatcher/SummitDispatcher.h>
#include <Renderer/View.h>

using namespace Core;

@implementation SummitRenderView
{
    NSTrackingArea* trackingArea;
    sigslot::signal<Core::MouseEvent&>* mMouseEventSignal;
    sigslot::signal<uint32_t, uint32_t>* mResizeEventSignal;
    
    CGPoint mLastPoint;
}

- (instancetype)initWithBounds:(NSRect)bounds
                 withCursorSignals:(sigslot::signal<Core::MouseEvent&>*)cursorSignals
                  withResizeSignal:(sigslot::signal<uint32_t, uint32_t>*)resizeSignal;
{
    self = [super init];
    if (self != nil)
    {
        [self setPostsFrameChangedNotifications:YES];
        
        [[NSNotificationCenter defaultCenter] addObserver:self
                   selector:@selector(frameDidChangeNotification:)
                       name:NSViewFrameDidChangeNotification
                     object:self];
        
        trackingArea = nil;
        [self setFrame:bounds];
        [self setBounds:bounds];
        [self updateTrackingAreas];
        
        self->mMouseEventSignal = cursorSignals;
        self->mResizeEventSignal = resizeSignal;
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

- (BOOL) isFlipped
{
    return NO;
}

- (void)frameDidChangeNotification:(NSNotification *)notification
{
    NSRect rect = [self frame];
    
    Core::DispatcherService::Service().Post([self, rect]() {
        (*mResizeEventSignal)(rect.size.width, rect.size.height);
    });
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

- (void)mouseDown:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    
    Core::DispatcherService::Service().Post([point, self](){
        MouseEvent event;
        event.type = MouseEventType::LeftPress;
        (*mMouseEventSignal)(event);
    });
}

- (void)mouseUp:(NSEvent *)event
{
    Core::DispatcherService::Service().Post([self]() {
        MouseEvent event;
        event.type = MouseEventType::LeftRelease;
        (*mMouseEventSignal)(event);
    });
}

- (void)mouseMoved:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    
    uint16_t x = point.x;
    uint16_t y = self.frame.size.height - point.y;
    
    Core::DispatcherService::Service().Post([x, y, self](){
        MouseEvent event;
        event.type = MouseEventType::Move;
        event.x = x;
        event.y = y;
        
        (*mMouseEventSignal)(event);
    });
    
    mLastPoint = CGPointMake((float)x, (float)y);
}

- (void)mouseDragged:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    
    uint16_t x = point.x;
    uint16_t y = self.frame.size.height - point.y;
    
    Core::DispatcherService::Service().Post([x, y, self](){
        MouseEvent event;
        event.type = MouseEventType::LeftDrag;
        event.x = x;
        event.y = y;
        
        (*mMouseEventSignal)(event);
    });
}

- (void)rightMouseDown:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    
    Core::DispatcherService::Service().Post([point, self](){
        MouseEvent event;
        event.type = MouseEventType::RightPress;
        (*mMouseEventSignal)(event);
    });
}

- (void)rightMouseDragged:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    
    uint16_t x = point.x;
    uint16_t y = self.frame.size.height - point.y;
    
    Core::DispatcherService::Service().Post([x, y, lastPoint = self->mLastPoint, self](){
        MouseEvent event;
        event.type = MouseEventType::RightDrag;
        event.x = x;
        event.y = y;
        
        const auto diffX = lastPoint.x - (float)x;
        const auto diffY = lastPoint.y - (float)y;
        
        event.offsetY = diffY;
        event.offsetX = diffX;
        
        (*mMouseEventSignal)(event);
    });
    
    mLastPoint = CGPointMake((float)x, (float)y);
}

- (void)rightMouseUp:(NSEvent *)event
{
    CGPoint point = [event locationInWindow];
    
    Core::DispatcherService::Service().Post([point, self](){
        MouseEvent event;
        event.type = MouseEventType::RightRelease;
        (*mMouseEventSignal)(event);
    });
}

- (void)keyDown:(NSEvent *)event
{
}

- (void)scrollWheel:(NSEvent *)event
{
}

/** Returns a Metal-compatible layer. */
+ (Class)layerClass {
    return [CAMetalLayer class];
}

/** If the wantsLayer property is set to YES, this method will be invoked to return a layer instance. */
- (CALayer*)makeBackingLayer {
    CALayer* layer = [self.class.layerClass layer];
    CGSize viewScale = [self convertSizeToBacking:CGSizeMake(1.0, 1.0)];
    layer.contentsScale = MIN(1.0, 1.0);
    return layer;
}
@end
