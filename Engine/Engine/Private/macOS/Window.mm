#include <Engine/Window.h>
#include <Renderer/SwapChain.h>
#include <Logging/LoggingService.h>

#import <Cocoa/Cocoa.h>
#import "SummitWindow.h"
#import "SummitView.h"

namespace Application
{
    class Window::NativeWindow
    {
    public:
        NativeWindow(const std::string& title, const uint16_t width, const uint16_t height)
        {
            NSRect contentRect = NSMakeRect(0, 0, width, height);
            window = [[SummitWindow alloc] initWithContentRect:contentRect
                                                     styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskBorderless
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO];
            
            windowDelegate = [[SummitWindowDelegate alloc] initWithWindow:window];
            [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
            [window setDelegate:windowDelegate];
            [window center];
            [window setAcceptsMouseMovedEvents: YES];
            [window orderFront:nil];
        }
        
        ~NativeWindow()
        {
            
        }
        
    public:
        SummitWindow* window = nil;
        SummitWindowDelegate* windowDelegate = nil;
        WindowSignalsBase mSignals;
        sigslot::signal<Core::MouseEvent&> MouseEvent;
        sigslot::signal<uint32_t, uint32_t> Resize;
    };
    
    Window::Window(const std::string& title, const uint16_t width, const uint16_t height)
        : mWidth(width)
        , mHeight(height)
        , mNativeWindow(std::make_unique<Window::NativeWindow>(title, width, height))
    {
    }
    
    void Window::SetTitle(const std::string& title)
    {
        [mNativeWindow->window setTitle:[NSString stringWithUTF8String:title.c_str()]];
    }
    
    HandleId Window::CreateView(uint16_t w, uint16_t h, uint16_t x, uint16_t y)
    {
        static HandleId sViewHandle{ 0 };
        
        SummitRenderView* view = [[SummitRenderView alloc] initWithBounds:NSMakeRect(0, 0, w, h)
                                                        withCursorSignals:&mNativeWindow->MouseEvent
                                                         withResizeSignal:&mNativeWindow->Resize];
        if(view != nil)
        {
            view.wantsLayer = YES;
            view.layer.backgroundColor = NSColor.blueColor.CGColor;
            
            [mNativeWindow->window setContentView:view];
            [view setFrame:NSMakeRect(x, y, w, h)];
            
            mView = std::make_unique<Renderer::View>(w, h, (void*)CFBridgingRetain(view));
            
            // Propagate signal to the view
            mNativeWindow->MouseEvent.connect([view = mView.get()](Core::MouseEvent& event) {
                view->MouseEvent(event);
            });
            
            mNativeWindow->Resize.connect([view = mView.get()](uint32_t x, uint32_t y) {
                view->OnResize(x, y);
            });
            
            return ++sViewHandle;
        }
        
        return 0;
    }
    
    Window::~Window()
    {
        // TODO: Release native window
    }
}
