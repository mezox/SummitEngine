#pragma once

#include <Renderer/RendererBase.h>
#include <Renderer/Resources/DeviceResource.h>
#include <Event/Event.h>

#include <cstdint>
#include <memory>

namespace Renderer
{
    class SwapChainBase;
    
    class RENDERER_API View : public DeviceResource
    {
    public:
        View(uint16_t width, uint16_t height, void* nativeView);
        virtual ~View();
        
        void OnResize(uint16_t x, uint16_t y);
        bool AcquireImage();
        
        [[nodiscard]] const uint32_t GetWidth() const noexcept;
        [[nodiscard]] const uint32_t GetHeight() const noexcept;
        
        SwapChainBase* GetSwapChain() { return mSwapChain.get(); }
        
    public:
        Core::Event<Core::MouseEvent&> MouseEvent;
        
    private:
        void* mNativeViewHandle{ nullptr };
        std::unique_ptr<SwapChainBase> mSwapChain;
    };
}
