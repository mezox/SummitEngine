#import <Cocoa/Cocoa.h>

@interface SummitRenderView : NSView
{
    NSTrackingArea* trackingArea;
}
- (instancetype)initWithBounds:(NSRect)bounds;
@end
