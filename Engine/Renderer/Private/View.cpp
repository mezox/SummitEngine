#include <Renderer/View.h>
#include <Renderer/Renderer.h>
#include <Renderer/SwapChain.h>

#include <Logging/LoggingService.h>

using namespace Renderer;
using namespace PAL::RenderAPI;

View::View(uint16_t width, uint16_t height, void* nativeView)
    : DeviceResource(RendererLocator::GetRenderer().CreateSurface(nativeView))
    , mWidth(width)
    , mHeight(height)
    , mNativeViewHandle(nativeView)
{}

View::~View()
{
    // Destroy surface
}

void View::OnResize(uint16_t width, uint16_t height)
{
//    if(mSwapChain)
//    {
//        RendererLocator::GetRenderer().CreateSwapChain(mSwapChain, GetDeviceObject(), width, height);
//    }
}

bool View::AcquireImage()
{
    return mSwapChain->AcquireImage();
}

const uint32_t View::GetWidth() const noexcept
{
    return mWidth;
}

const uint32_t View::GetHeight() const noexcept
{
    return mHeight;
}

void View::SetSwapChain(std::unique_ptr<SwapChainBase> swapChain)
{
    mSwapChain = std::move(swapChain);
}
