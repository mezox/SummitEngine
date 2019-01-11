#import "SummitView.h"
#import <QuartzCore/CAMetalLayer.h>

@implementation SummitRenderView
+ (Class)layerClass {
    return [CAMetalLayer class];
}
@end
