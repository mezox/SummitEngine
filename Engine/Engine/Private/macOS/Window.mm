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
            
            windowDelegate = [[SummitWindowDelegate alloc] initWithWindow:window signals:&mSignals];
            //mainView = [[NSView alloc] init];
            
            view = [[SummitRenderView alloc] initWithBounds:NSMakeRect(0, 0, width, height)];
            view.wantsLayer = YES;
            view.layer.backgroundColor = NSColor.blueColor.CGColor;
            
            [window setContentView:view];
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
        NSView* mainView = nil;
        SummitRenderView* view = nil;
        WindowSignalsBase mSignals;
    };
    
    Window::Window(const std::string& title, const uint16_t width, const uint16_t height)
        : mWidth(width)
        , mHeight(height)
        , mNativeWindow(new Application::Window::NativeWindow(title, width, height))
    {
        auto viewPtr = std::make_unique<Renderer::View>(width, height, (void*)CFBridgingRetain(mNativeWindow->view));
        mNativeWindow->mSignals.WindowResized.connect([view = viewPtr.get()](const uint16_t width, const uint16_t height){
            view->OnResize(width, height);
        });
        
        mViews.push_back(std::move(viewPtr));
    }
    
    void Window::Update()
    {
        for(auto& view : mViews)
        {
            view->Update();
        }
    }
    
    void Window::SetTitle(const std::string& title)
    {
        [mNativeWindow->window setTitle:[NSString stringWithUTF8String:title.c_str()]];
    }
    
    Window::~Window()
    {
        // TODO: Release native window
    }
}
