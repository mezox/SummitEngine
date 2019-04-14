#import <Cocoa/Cocoa.h>

#include <Event/Event.h>
#include <cstdint>


@interface SummitRenderView : NSView
- (instancetype)initWithBounds:(NSRect)bounds
                 withCursorSignals:(sigslot::signal<Core::MouseEvent&>*)cursorSignals
                  withResizeSignal:(sigslot::signal<uint32_t, uint32_t>*)resizeSignal;
@end
