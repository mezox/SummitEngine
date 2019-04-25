#include "VulkanSwapChainImpl.h"
#include <PAL/RenderAPI/Vulkan/VulkanDevice.h>
#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include "VulkanRendererImpl.h"

using namespace Renderer;
using namespace PAL::RenderAPI;


VulkanSwapChain::VulkanSwapChain(std::shared_ptr<PAL::RenderAPI::VulkanDevice>device, DeviceObject&& swapChain)
    : SwapChainBase(std::move(swapChain))
    , mDevice(std::move(device))
{}

VulkanSwapChain::~VulkanSwapChain()
{
    Destroy();
}

void VulkanSwapChain::Destroy()
{
    DestroyVisitor visitor(mDevice);
    
    auto& swapDeviceObject = GetDeviceObject();
    swapDeviceObject.Accept(visitor);
}

bool VulkanSwapChain::AcquireImage()
{
    const auto& swapChainDeviceObject = GetDeviceObject();
    
    VulkanSwapChainVisitor visitor;
    swapChainDeviceObject.Accept(visitor);
    
    const auto status = mDevice->AcquireNextImageKHR(visitor.swapChain,
                                                     std::numeric_limits<uint64_t>::max(),
                                                     visitor.imgAvailableSemaphore,
                                                     VK_NULL_HANDLE,
                                                     &mAcquiredImageIndex);
    
    mDevice->WaitForFences(1, &visitor.frameFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    mDevice->ResetFences(1, &visitor.frameFence);
    
    return (status != VK_SUCCESS) ? false : true;
}

void VulkanSwapChain::SwapBuffers()
{
    const auto& swapChainDeviceObject = GetDeviceObject();
    
    VulkanSwapChainVisitor visitor;
    swapChainDeviceObject.Accept(visitor);
    
    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &visitor.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &visitor.swapChain;
    presentInfo.pImageIndices = &mAcquiredImageIndex;
    presentInfo.pResults = nullptr; // Optional
    
    VkQueue presentQueue{ VK_NULL_HANDLE };
    mDevice->GetDeviceQueue(0, 0, &presentQueue);
    
    mDevice->QueuePresentKHR(presentQueue, &presentInfo);
}
