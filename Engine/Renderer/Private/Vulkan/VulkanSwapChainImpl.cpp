#include "VulkanSwapChainImpl.h"
#include <PAL/RenderAPI/VulkanDevice.h>
#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include "VulkanRendererImpl.h"

using namespace Renderer;
using namespace PAL::RenderAPI;


VulkanSwapChain::VulkanSwapChain(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device, DeviceObject&& deviceObject, const VkQueue& presentationQueue)
    : SwapChainBase(std::move(deviceObject))
    , mDevice(std::move(device))
    , mPresentQueue(presentationQueue)
{}

VulkanSwapChain::~VulkanSwapChain()
{
    Destroy();
}

void VulkanSwapChain::Destroy()
{
    // Warning: Framebuffers might be destroyed only after rendering finished
    mDevice->WaitIdle();
    
//    for (auto& fb : mDeviceObject.framebuffers)
//    {
//        mDevice->DestroyFramebuffer(fb.framebuffer, nullptr);
//        mDevice->DestroyImageView(fb.imageView, nullptr);
//        //mDevice->DestroyImage(fb.image, nullptr);
//    }
//    
//    mDeviceObject.framebuffers.clear();
    
    VulkanSwapChainVisitor visitor;
    
    const auto& swapDeviceObject = GetDeviceObject();
    swapDeviceObject.Accept(visitor);
    
    mDevice->DestroySwapchainKHR(visitor.swapChain, nullptr);
}

void VulkanSwapChain::SwapBuffers()
{
    auto& renderer = (VulkanRenderer&)Renderer::RendererLocator::GetRenderer();
    const auto& frameSync = renderer.GetFrameSyncData();
    
    mDevice->WaitForFences(1, &frameSync.frameFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    
    VulkanSwapChainVisitor visitor;
    
    const auto& swapDeviceObject = GetDeviceObject();
    swapDeviceObject.Accept(visitor);
    
    const auto status = mDevice->AcquireNextImageKHR(visitor.swapChain, std::numeric_limits<uint64_t>::max(), frameSync.imageAvailableSemaphore, VK_NULL_HANDLE, &mImageIndex);
    
    renderer.NewFrame(mImageIndex);
    
    if(status != VK_SUCCESS)
    {
//        LOG(Debug) << "Swap chain window resize";
//        
//        mDevice->WaitIdle();
//        
//        mDevice->DestroySwapchainKHR(mDeviceObject.swapChain, nullptr);
//        
//        mImageIndex = 0;
//        
//        Initialize(1400, 750);
        return;
    }
    
    const auto& commandBuffers = renderer.GetCommandBuffers();
    
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &frameSync.imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[mImageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &frameSync.renderFinishedSemaphore;
    
    mDevice->ResetFences(1, &frameSync.frameFence);
    mDevice->QueueSubmit(mPresentQueue, 1, &submitInfo, frameSync.frameFence);
    
    // Present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &frameSync.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &visitor.swapChain;
    presentInfo.pImageIndices = &mImageIndex;
    presentInfo.pResults = nullptr; // Optional
    
    mDevice->QueuePresentKHR(mPresentQueue, &presentInfo);
}
