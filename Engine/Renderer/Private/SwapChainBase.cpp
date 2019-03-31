#include <Renderer/SwapChain.h>

using namespace Renderer;

SwapChainBase::SwapChainBase(DeviceObject&& deviceObject)
    :  DeviceResource(std::move(deviceObject))
{}
