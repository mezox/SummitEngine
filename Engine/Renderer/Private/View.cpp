#include <Renderer/View.h>
#include <Renderer/Renderer.h>
#include <Renderer/SwapChain.h>

#include <Logging/LoggingService.h>

using namespace Renderer;
using namespace PAL::RenderAPI;

View::View(uint16_t width, uint16_t height, void* nativeView)
    : DeviceResource(RendererLocator::GetRenderer().CreateSurface(nativeView))
    , mNativeViewHandle(nativeView)
{
    RendererLocator::GetRenderer().CreateSwapChain(mSwapChain, GetDeviceObject(), width, height);
}

View::~View()
{
    // Destroy surface
}

void View::OnResize(uint16_t width, uint16_t height)
{
    if(mSwapChain)
    {
        RendererLocator::GetRenderer().CreateSwapChain(mSwapChain, GetDeviceObject(), width, height);
    }
}

bool View::AcquireImage()
{
    return mSwapChain->AcquireImage();
}
