#pragma once

#include <cstdint>

namespace Renderer
{
    enum class ImageFormat
    {
        DefaultSwapChain
    };
    
    class SwapChainResource;
    
    class SwapChain final
    {
    public:
        ~SwapChain() = default;
        
        void Initialize(const uint32_t width, const uint32_t height);
        void Destroy();
        void SwapBuffers();
        
        ImageFormat GetImageFormat() const;
        
    public:
        std::unique_ptr<SwapChainResource> mSwapChainResource;
        
    private:
        ImageFormat mImageFormat;
    };
}
