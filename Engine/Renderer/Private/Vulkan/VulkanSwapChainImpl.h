#pragma once

#include <Renderer/Renderer.h>
#include <Renderer/SwapChain.h>
#include <PAL/RenderAPI/VulkanAPI.h>

#include <Event/EventHandler.h>
//#include <Engine/WindowEvent.h>

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace PAL::RenderAPI
{
    class VulkanDevice;
}

namespace Renderer
{    
    class SwapChainVK : public SwapChainResource
    {
    public:
        SwapChainVK(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device, const VkSurfaceKHR& surface);
        ~SwapChainVK();

        // SwapChainBase interface
        void Initialize(const uint32_t width, const uint32_t height) override;
        void Destroy() override;
        void SwapBuffers() override;

        void SetSemaphore(VkQueue present);

        const VkSwapchainKHR& GetSwapChain() const { return mHandle; }
        const std::vector<VkImageView>& GetSwapImageViews() const { return mImageViews; }
        const std::vector<VkFramebuffer>& GetFramebuffers() const { return mFramebuffers; }
        
    private:
        void DestroyImageViews();
        void DestroyImages();
        void DestroyFramebuffers();
        
    private:
        //void OnWindowResize(const Application::WindowResizeEvent& event);

    private:
        std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        
        VkFormat mFormat{ VK_FORMAT_UNDEFINED };
        
        VkSwapchainKHR mHandle{ VK_NULL_HANDLE};
        VkSurfaceKHR mSurface{ VK_NULL_HANDLE };
        
        VkQueue mPresentQueue{ VK_NULL_HANDLE };
        
        std::vector<VkImage> mImages;
        std::vector<VkImageView> mImageViews;
        std::vector<VkFramebuffer> mFramebuffers;

        // Swap chain properties
        VkSurfaceCapabilitiesKHR mCapabilities;
        std::vector<VkSurfaceFormatKHR> mFormats;
        std::vector<VkPresentModeKHR> mPresentationModes;

        uint32_t mImageIndex{ 0 };
        
        //Event::EventHandlerFunc<SwapChainVK, Application::WindowResizeEvent> mWindowResizeHandler;
    };
}
