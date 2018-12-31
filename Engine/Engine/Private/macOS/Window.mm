#include "Window.h"
#include "Renderer.h"

#import <Cocoa/Cocoa.h>
#import "SummitWindow.h"
#import "SummitView.h"

namespace App
{
    class Window::NativeWindow
    {
    public:
        NativeWindow(const std::string& title, const uint16_t width, const uint16_t height)
        {
            NSRect contentRect = NSMakeRect(0, 0, width, height);
            window = [[SummitWindow alloc] initWithContentRect:contentRect
                                                     styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskResizable
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO];
            
            windowDelegate = [[SummitWindowDelegate alloc] init];
            mainView = [[NSView alloc] init];
            
            view = [[SummitRenderView alloc] initWithBounds:NSMakeRect(0, 0, width, height)];
            view.wantsLayer = YES;
            view.layer.backgroundColor = NSColor.blueColor.CGColor;
            
            [window setContentView:mainView];
            //[window makeFirstResponder:mainView];
            [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
            [window setDelegate:windowDelegate];
            [window center];
            
//            view2 = [[SummitRenderView alloc] initWithBounds:NSMakeRect(640, 0, 640, 720)];
//            view2.wantsLayer = YES;
//            view2.layer.backgroundColor = NSColor.orangeColor.CGColor;
//            //[view2 becomeFirstResponder];
            [mainView addSubview:view];
            //[mainView addSubview:view2];
            
            [window setAcceptsMouseMovedEvents: YES];
            
            [window orderFront:nil];
        }
        
        ~NativeWindow()
        {
            
        }
        
    public:
        id window = nil;
        id windowDelegate = nil;
        NSView* mainView = nil;
        SummitRenderView* view = nil;
        //SummitRenderView* view2 = nil;
    };
    
    Window::Window(const std::string& title, const uint16_t width, const uint16_t height)
        : mTitle(title)
        , mWidth(width)
        , mHeight(height)
        , mNativeWindow(new App::Window::NativeWindow(title, width, height))
    {
        Renderer::RendererServiceLocator::Service().CreateWindowSurface(mSurface, (void*)CFBridgingRetain(mNativeWindow->view));
    }
    
    void Window::Update()
    {
        
    }
    
    Window::~Window()
    {
        // TODO: Release native window
    }
}
