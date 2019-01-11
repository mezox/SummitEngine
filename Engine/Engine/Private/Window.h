#pragma once

#include <string>
#include <cstdint>

#include <Event/Event.h>

namespace Renderer
{
	class IRenderer;
	class RendererResource;
}

namespace App
{
    class Window
    {
        class NativeWindow;
        
    public:
        Window(const std::string& title, const uint16_t width, const uint16_t height);
        virtual ~Window();

		void Update();
            
    private:
        std::string mTitle;
        uint16_t mWidth{ 1920 };
        uint16_t mHeight{ 1080 };
        
        std::unique_ptr<NativeWindow> mNativeWindow;
		std::unique_ptr<Renderer::RendererResource> mSurface;
		std::unique_ptr<Renderer::RendererResource> mSwapChain;
    };
}
