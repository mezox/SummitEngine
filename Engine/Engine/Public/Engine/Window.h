#pragma once

#include "WindowEvent.h"

#include <string>
#include <cstdint>

#include "EngineBase.h"
#include <Engine/SwapChain.h>
#include <Event/EventHandler.h>
#include "Renderer.h"

namespace Renderer
{
	class RendererResource;
    class IRenderer;
}

namespace Application
{
    class ENGINE_API Window
    {
        class NativeWindow;
        
    public:
        Window(Renderer::IRenderer& renderer, const std::string& title, const uint16_t width, const uint16_t height);
        virtual ~Window();

		void Update();
        
        void SetTitle(const std::string& title);
        
        Renderer::ImageFormat GetImageFormat() const;
        
    private:
        void OnWindowWillClose(const WindowWillCloseEvent& event);
            
    private:
        uint16_t mWidth{ 1920 };
        uint16_t mHeight{ 1080 };
        Renderer::SwapChain mSwapChain;
        
        std::unique_ptr<NativeWindow> mNativeWindow;
        
        Event::EventHandlerFunc<Window, WindowWillCloseEvent> mWindowWillClose;
    };
}
