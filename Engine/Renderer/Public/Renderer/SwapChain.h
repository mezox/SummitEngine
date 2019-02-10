#pragma once

#include "RendererBase.h"

#include <cstdint>
#include <Renderer/DeviceObject.h>

namespace Renderer
{
    enum class ImageFormat
    {
        DefaultSwapChain
    };
    
    class SwapChainResource;
    
    class RENDERER_API SwapChain final
    {
    public:
        ~SwapChain() = default;
        
        void Initialize(const uint32_t width, const uint32_t height);
        void Destroy();
        void SwapBuffers();
        
        ImageFormat GetImageFormat() const;
        
    public:
        std::unique_ptr<SwapChainResource> mSwapChainResource;
        DeviceObject mDeviceObject;
        
    private:
        ImageFormat mImageFormat;
    };
}
