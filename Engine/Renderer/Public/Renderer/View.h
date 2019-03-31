#pragma once

#include <Renderer/RendererBase.h>

#include <cstdint>
#include <memory>

namespace Renderer
{
    class SwapChainBase;
    
    class Layer
    {
    public:
        virtual ~Layer() = default;
    };
    
    class RENDERER_API View
    {
    public:
        View(uint16_t width, uint16_t height, void* nativeView);
        virtual ~View();
        
        void Update();
        void OnResize(const uint16_t x, const uint16_t y);
        
    private:
        void* mNativeViewHandle{ nullptr };
        
        uint16_t mWidth{ 1920 };
        uint16_t mHeight{ 1080 };
        std::unique_ptr<SwapChainBase> mSwapChain;
        //std::vector<std::unique_ptr<Layer>> mLayers;
    };
}
