#include <Renderer/View.h>
#include <Renderer/Renderer.h>
#include "Vulkan/VulkanSwapChainImpl.h"

#include <Logging/LoggingService.h>

using namespace Renderer;

View::View(uint16_t width, uint16_t height, void* nativeView)
    : mNativeViewHandle(nativeView)
{
    RendererLocator::GetRenderer().CreateSwapChain(mSwapChain, nativeView, width, height);
}

View::~View()
{
}

void View::Update()
{
    mSwapChain->SwapBuffers();
}

void View::OnResize(const uint16_t width, const uint16_t height)
{    
    RendererLocator::GetRenderer().CreateSwapChain(mSwapChain, mNativeViewHandle, width, height);
}
