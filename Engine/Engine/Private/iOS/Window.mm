#include "Window.h"
#include "Renderer.h"

#import "SummitView.h"

namespace App
{
    class Window::NativeWindow
    {
    public:
        NativeWindow(const std::string& title, const uint16_t width, const uint16_t height)
        {
            view = [[SummitRenderView alloc] init];
        }
        
        ~NativeWindow()
        {
            
        }
        
    public:
        id window = nil;
        id windowDelegate = nil;
        SummitRenderView* view = nil;
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
