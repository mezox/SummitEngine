#include "VulkanSwapChainImpl.h"
#include <PAL/RenderAPI/VulkanDevice.h>
#include <Event/EventService.h>
#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include "VulkanRendererImpl.h"

using namespace Renderer;
using namespace PAL::RenderAPI;
using namespace Event;


void SwapChain::Initialize(const uint32_t width, const uint32_t height)
{
    mSwapChainResource->Initialize(width, height);
}

void SwapChain::Destroy()
{
    mSwapChainResource->Destroy();
}

void SwapChain::SwapBuffers()
{
    mSwapChainResource->SwapBuffers();
}
    
ImageFormat SwapChain::GetImageFormat() const
{
    return mImageFormat;
}

SwapChainVK::SwapChainVK(std::shared_ptr<VulkanDevice> device, const VkSurfaceKHR& surface)
    : mDevice(std::move(device))
    , mSurface(surface)
{
    //mWindowResizeHandler = EventHandlerFunc(true, this, &SwapChainVK::OnWindowResize);
    
    //EventServiceLocator::Service().RegisterEventHandler(mWindowResizeHandler);
}

SwapChainVK::~SwapChainVK()
{
    Destroy();
}

void SwapChainVK::Initialize(const uint32_t width, const uint32_t height)
{
    const auto& vulkanAPI = VulkanAPIServiceLocator::Service();
    const auto& physicalDevice = mDevice->GetPhysicalDevice();
    
    mCapabilities = vulkanAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, mSurface);
    mFormats = vulkanAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mSurface);
    mPresentationModes = vulkanAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, mSurface);
    
    const auto& format = mFormats.front();      // Get B8G8R8A8_unorm
    const auto& presentMode = mPresentationModes.front();   // Immeadiate
    
    mFormat = format.format;

    VkExtent2D screenExtent;
    screenExtent.width = width;
    screenExtent.height = height;

    auto imagesCount = mCapabilities.minImageCount + 1;

    if (1/*mEnableTrippleBuffering*/)
    {
        if (mCapabilities.maxImageCount > 0 && imagesCount > mCapabilities.maxImageCount)
        {
            imagesCount = mCapabilities.maxImageCount;
        }
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mSurface;
    createInfo.minImageCount = imagesCount;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageFormat = format.format;
    createInfo.imageExtent = screenExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //Use transfer if I want to render to other buffer and apply post process and then copy;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
    createInfo.preTransform = mCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VkSwapchainKHR{};

    mDevice->CreateSwapchainKHR(&createInfo, nullptr, &mHandle);
    
    mImages = mDevice->GetSwapchainImagesKHR(mHandle);

    mImageViews.resize(mImages.size());

    for (size_t idx{ 0 }; idx < mImageViews.size(); ++idx)
    {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = mImages[idx];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        mImageViews[idx] = mDevice->CreateImageView(&imageViewCreateInfo, nullptr);
    }
    
    
    auto& renderer = (VulkanRenderer&)Renderer::RendererLocator::GetRenderer();
    
    mFramebuffers.resize(mImages.size());

    for (size_t idx = 0; idx < mImages.size(); ++idx)
    {
        std::vector<VkImageView> attachments{ mImageViews[idx] };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderer.GetRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        mDevice->CreateFramebuffer(&framebufferInfo, nullptr, &mFramebuffers[idx]);
    }
}

void SwapChainVK::Destroy()
{
    // Warning: Framebuffers might be destroyed only after rendering finished
    DestroyFramebuffers();
    DestroyImageViews();
    DestroyImages();
    
    mDevice->DestroySwapchainKHR(mHandle, nullptr);
    
    VulkanAPIServiceLocator::Service().DestroySurface(mSurface);
    //EventServiceLocator::Service().UnRegisterEventHandler(mWindowResizeHandler);
}

void SwapChainVK::SetSemaphore(VkQueue present)
{
    mPresentQueue = present;
}

void SwapChainVK::SwapBuffers()
{
    auto& renderer = (VulkanRenderer&)Renderer::RendererLocator::GetRenderer();
    const auto& frameSync = renderer.GetFrameSyncData();
    
    mDevice->WaitForFences(1, &frameSync.frameFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    
    const auto status = mDevice->AcquireNextImageKHR(mHandle, std::numeric_limits<uint64_t>::max(), frameSync.imageAvailableSemaphore, VK_NULL_HANDLE, &mImageIndex);
    
    renderer.UpdateCamera(mImageIndex);
    
    if(status != VK_SUCCESS)
    {
        LOG(Debug) << "Swap chain window resize";
        
        mDevice->WaitIdle();
        
        DestroyFramebuffers();
        DestroyImageViews();
        DestroyImages();
        
        mDevice->DestroySwapchainKHR(mHandle, nullptr);
        
        mImageIndex = 0;
        
        Initialize(1400, 750);
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
    presentInfo.pSwapchains = &mHandle;
    presentInfo.pImageIndices = &mImageIndex;
    presentInfo.pResults = nullptr; // Optional
    
    mDevice->QueuePresentKHR(mPresentQueue, &presentInfo);
    
    renderer.NewFrame();
}

void SwapChainVK::DestroyImageViews()
{
    for (auto& view : mImageViews)
    {
        mDevice->DestroyImageView(view, nullptr);
    }
    
    mImageViews.clear();
}

void SwapChainVK::DestroyImages()
{
    for (auto& image : mImages)
    {
        //mDevice->DestroyImage(image, nullptr);
    }
    
    mImages.clear();
}

void SwapChainVK::DestroyFramebuffers()
{
    for (auto& fb : mFramebuffers)
    {
        mDevice->DestroyFramebuffer(fb, nullptr);
    }
    
    mFramebuffers.clear();
}

//void SwapChainVK::OnWindowResize(const Application::WindowResizeEvent& event)
//{
//    LOG(Debug) << "Swap chain window resize";
//
//    mDevice->WaitIdle();
//
//    DestroyFramebuffers();
//    DestroyImageViews();
//    DestroyImages();
//
//    mDevice->DestroySwapchainKHR(mHandle, nullptr);
//
//    mImageIndex = 0;
//
//    Initialize(event.width, event.height);
//}
