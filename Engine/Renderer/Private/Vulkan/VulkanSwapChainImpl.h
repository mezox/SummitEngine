#pragma once

#include <Renderer/SwapChain.h>
#include "VulkanDeviceObjects.h"

#include <vulkan/vulkan.h>
#include <memory>

namespace PAL::RenderAPI
{
    class VulkanDevice;
}

namespace Renderer
{
    class VulkanSwapChain : public SwapChainBase
    {
    public:
        VulkanSwapChain(std::shared_ptr<PAL::RenderAPI::VulkanDevice>device, DeviceObject&& swapChain, const VkQueue& presentationQueue);
        virtual ~VulkanSwapChain();

        // SwapChainBase interface
        void Destroy() override;
        void SwapBuffers() override;

    private:
        std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        VkSemaphore mImageAvailableSemaphore{ VK_NULL_HANDLE };
        VkSemaphore mRenderFinishedSemaphore{ VK_NULL_HANDLE };
        VkFence mFrameFence{ VK_NULL_HANDLE };
        VkQueue mPresentQueue{ VK_NULL_HANDLE };
    };
}
