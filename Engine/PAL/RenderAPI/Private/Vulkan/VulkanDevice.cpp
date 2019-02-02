#include <PAL/RenderAPI/VulkanAPI.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include "VulkanLoaderHelper.h"
#include <Logging/LoggingService.h>

#include <algorithm>

namespace PAL::RenderAPI
{
	VulkanDevice::VulkanDevice(DeviceData& deviceData)
		: mPhysicalDevice(deviceData.device)
		, mLogicalDevice(deviceData.logicalDevice)
		, mDeviceProperties(deviceData.deviceProperties)
		, mDeviceFeatures(deviceData.deviceFeatures)
		, mDeviceExtensions(std::move(deviceData.deviceExtensions))
	{
		LoadFunctions(deviceData.deviceProcAddrFunc);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(mLogicalDevice, nullptr);
	}

	void VulkanDevice::CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) const
	{
		VK_CHECK_RESULT(vkCreateSwapchainKHR(mLogicalDevice, pCreateInfo, pAllocator, pSwapchain));
	}

	void VulkanDevice::DestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) const
	{
		vkDestroySwapchainKHR(mLogicalDevice, swapchain, pAllocator);
	}
    
    void VulkanDevice::AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) const
    {
       VK_CHECK_RESULT(vkAcquireNextImageKHR(mLogicalDevice, swapchain, timeout, semaphore, fence, pImageIndex));
    }
    
    void VulkanDevice::QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) const
    {
        VK_CHECK_RESULT(vkQueuePresentKHR(queue, pPresentInfo));
    }
    
    std::vector<VkImage> VulkanDevice::GetSwapchainImagesKHR(VkSwapchainKHR swapchain) const
    {
        uint32_t swapchainImageCount{ 0 };
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mLogicalDevice, swapchain, &swapchainImageCount, nullptr));
        
        std::vector<VkImage> swapchainImages(swapchainImageCount);
        VK_CHECK_RESULT(vkGetSwapchainImagesKHR(mLogicalDevice, swapchain, &swapchainImageCount, swapchainImages.data()));
        
        return swapchainImages;
    }
    
    void VulkanDevice::CreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) const
    {
        VK_CHECK_RESULT(vkCreateShaderModule(mLogicalDevice, pCreateInfo, pAllocator, pShaderModule));
    }
    
    void VulkanDevice::DestroyShaderModule(VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyShaderModule(mLogicalDevice, shaderModule, pAllocator);
    }
    
    void VulkanDevice::CreatePipelineLayout(const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) const
    {
        VK_CHECK_RESULT(vkCreatePipelineLayout(mLogicalDevice, pCreateInfo, pAllocator, pPipelineLayout));
    }
    
    void VulkanDevice::DestroyPipelineLayout(VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyPipelineLayout(mLogicalDevice, pipelineLayout, pAllocator);
    }
    
    void VulkanDevice::CreateGraphicsPipeline(VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) const
    {
        VK_CHECK_RESULT(vkCreateGraphicsPipelines(mLogicalDevice, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines));
    }
    
    void VulkanDevice::DestroyPipeline(VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyPipeline(mLogicalDevice, pipeline, pAllocator);
    }
    
    void VulkanDevice::CreateRenderPass(const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) const
    {
        VK_CHECK_RESULT(vkCreateRenderPass(mLogicalDevice, pCreateInfo, pAllocator, pRenderPass));
    }
    
    void VulkanDevice::DestroyRenderPass(VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) const
    {
         vkDestroyRenderPass(mLogicalDevice, renderPass, pAllocator);
    }
    
    void VulkanDevice::BeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) const
    {
        vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    }
    
    void VulkanDevice::EndRenderPass(VkCommandBuffer commandBuffer) const
    {
        vkCmdEndRenderPass(commandBuffer);
    }
    
    void VulkanDevice::BindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const
    {
        vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    }
    
    void VulkanDevice::Draw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
    {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
    
    void VulkanDevice::CreateFramebuffer(const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) const
    {
        VK_CHECK_RESULT(vkCreateFramebuffer(mLogicalDevice, pCreateInfo, pAllocator, pFramebuffer));
    }
    
    void VulkanDevice::DestroyFramebuffer(VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyFramebuffer(mLogicalDevice, framebuffer, pAllocator);
    }
    
    void VulkanDevice::CreateCommandPool(const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) const
    {
        VK_CHECK_RESULT(vkCreateCommandPool(mLogicalDevice, pCreateInfo, pAllocator, pCommandPool));
    }
    
    void VulkanDevice::DestroyCommandPool(VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyCommandPool(mLogicalDevice, commandPool, pAllocator);
    }
    
    void VulkanDevice::ResetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags) const
    {
        VK_CHECK_RESULT(vkResetCommandPool(mLogicalDevice, commandPool, flags));
    }
    
    void VulkanDevice::AllocateCommandBuffers(const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) const
    {
        VK_CHECK_RESULT(vkAllocateCommandBuffers(mLogicalDevice, pAllocateInfo, pCommandBuffers));
    }
    
    void VulkanDevice::FreeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) const
    {
        vkFreeCommandBuffers(mLogicalDevice, commandPool, commandBufferCount, pCommandBuffers);
    }
    
    void VulkanDevice::BeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) const
    {
        VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, pBeginInfo));
    }
    
    void VulkanDevice::EndCommandBuffer(VkCommandBuffer commandBuffer) const
    {
        VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));
    }
    
    void VulkanDevice::ResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) const
    {
        VK_CHECK_RESULT(vkResetCommandBuffer(commandBuffer, flags));
    }
    
    VkImageView VulkanDevice::CreateImageView(const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator) const
    {
        VkImageView imageView{ VK_NULL_HANDLE };
        VK_CHECK_RESULT(vkCreateImageView(mLogicalDevice, pCreateInfo, nullptr, &imageView));
        return imageView;
    }
    
    void VulkanDevice::DestroyImageView(VkImageView imageView, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyImageView(mLogicalDevice, imageView, pAllocator);
    }
    
    void VulkanDevice::CreateSemaphore(const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) const
    {
       VK_CHECK_RESULT(vkCreateSemaphore(mLogicalDevice, pCreateInfo, pAllocator, pSemaphore));
    }
    
    void VulkanDevice::DestroySemaphore(VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) const
    {
       vkDestroySemaphore(mLogicalDevice, semaphore, pAllocator);
    }

	void VulkanDevice::LoadFunctions(PFN_vkGetDeviceProcAddr loadFunc)
	{
		LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyDevice);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDeviceWaitIdle);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkQueueSubmit);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkGetDeviceQueue);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateImageView);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyImageView);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateShaderModule);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyShaderModule);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreatePipelineLayout);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyPipelineLayout);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateGraphicsPipelines);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyPipeline);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBeginRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdEndRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBindPipeline);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdDraw);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateFramebuffer);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyFramebuffer);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateCommandPool);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyCommandPool);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkResetCommandPool);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkAllocateCommandBuffers);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkFreeCommandBuffers);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkBeginCommandBuffer);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkEndCommandBuffer);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkResetCommandBuffer);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateSemaphore);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroySemaphore);

		if (IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		{
			LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkCreateSwapchainKHR);
			LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkDestroySwapchainKHR);
            LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkGetSwapchainImagesKHR);
            LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkAcquireNextImageKHR);
            LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkQueuePresentKHR);
		}
		else
		{
			LOG(Warn) << "Device Extension: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME << " not available.";
		}
	}
    
    void VulkanDevice::WaitIdle() const
    {
        VK_CHECK_RESULT(vkDeviceWaitIdle(mLogicalDevice));
    }
    
    void VulkanDevice::QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) const
    {
        VK_CHECK_RESULT(vkQueueSubmit(queue, submitCount, pSubmits, fence));
    }
    
    void VulkanDevice::GetDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) const
    {
        vkGetDeviceQueue(mLogicalDevice, queueFamilyIndex, queueIndex, pQueue);
    }

	bool VulkanDevice::IsExtensionSupported(const char * extensionName) const
	{
		return std::find_if(mDeviceExtensions.begin(), mDeviceExtensions.end(), [extensionName](const VkExtensionProperties& props) {
			return strcmp(extensionName, props.extensionName) == 0;
		}) != mDeviceExtensions.end();
	}
}
