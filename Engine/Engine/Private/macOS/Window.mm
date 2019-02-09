#include <Engine/Window.h>
#include <Engine/Renderer.h>
#include <Engine/SwapChain.h>
#include <Event/EventService.h>
#include "VulkanSwapChainImpl.h"

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
            //mainView = [[NSView alloc] init];
            
            view = [[SummitRenderView alloc] initWithBounds:NSMakeRect(0, 0, width, height)];
            view.wantsLayer = YES;
            view.layer.backgroundColor = NSColor.blueColor.CGColor;
            
            [window setContentView:view];
            [window setTitle:[NSString stringWithUTF8String:title.c_str()]];
            [window setDelegate:windowDelegate];
            [window center];
        
//            [mainView addSubview:view];
            
            [window setAcceptsMouseMovedEvents: YES];
            [window orderFront:nil];
        }
        
        ~NativeWindow()
        {
            
        }
        
    public:
        SummitWindow* window = nil;
        id windowDelegate = nil;
        NSView* mainView = nil;
        SummitRenderView* view = nil;
    };
    
    Window::Window(Renderer::IRenderer& renderer, const std::string& title, const uint16_t width, const uint16_t height)
        : mWidth(width)
        , mHeight(height)
        , mNativeWindow(new Application::Window::NativeWindow(title, width, height))
    {
        mWindowWillClose = Event::EventHandlerFunc(true, this, &Window::OnWindowWillClose);
        Event::EventServiceLocator::Service().RegisterEventHandler(mWindowWillClose);
        
        renderer.CreateSwapChain(mSwapChain.mSwapChainResource, (void*)CFBridgingRetain(mNativeWindow->view), width, height);
    }
    
    void Window::Update()
    {
        ((Renderer::SwapChainVK*)mSwapChain.mSwapChainResource.get())->SwapBuffers();
    }
    
    void Window::SetTitle(const std::string& title)
    {
        [mNativeWindow->window setTitle:[NSString stringWithUTF8String:title.c_str()]];
    }
    
    Renderer::ImageFormat Window::GetImageFormat() const
    {
        return mSwapChain.GetImageFormat();
    }
    
    Window::~Window()
    {
        // TODO: Release native window
    }
    
    void Window::OnWindowWillClose(const WindowWillCloseEvent& event)
    {
    }
}
