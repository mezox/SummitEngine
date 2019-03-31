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
    
    bool VulkanDevice::IsFeatureSupported(DeviceFeature f) const
    {
        if(f == DeviceFeature::AnisotropicFiltering)
            return mDeviceFeatures.samplerAnisotropy;
        
        return false;
    }

	void VulkanDevice::CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) const
	{
		VK_CHECK_RESULT(vkCreateSwapchainKHR(mLogicalDevice, pCreateInfo, pAllocator, pSwapchain));
	}

	void VulkanDevice::DestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) const
	{
		vkDestroySwapchainKHR(mLogicalDevice, swapchain, pAllocator);
	}
    
    VkResult VulkanDevice::AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) const
    {
        const auto result = vkAcquireNextImageKHR(mLogicalDevice, swapchain, timeout, semaphore, fence, pImageIndex);
        VK_CHECK_RESULT(result);
        return result;
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
    
    VkResult VulkanDevice::CreateImage(const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) const
    {
        const auto result = vkCreateImage(mLogicalDevice, pCreateInfo, pAllocator, pImage);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::DestroyImage(VkImage image, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyImage(mLogicalDevice, image, pAllocator);
    }
    
    VkResult VulkanDevice::BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) const
    {
        const auto result = vkBindImageMemory(mLogicalDevice, image, memory, memoryOffset);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::GetImageMemoryRequirements(VkImage image, VkMemoryRequirements* pMemoryRequirements) const
    {
        vkGetImageMemoryRequirements(mLogicalDevice, image, pMemoryRequirements);
    }
    
    VkResult VulkanDevice::CreateImageView(const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pImageView) const
    {
        VK_CHECK_RESULT(vkCreateImageView(mLogicalDevice, pCreateInfo, nullptr, pImageView));
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
    
    VkResult VulkanDevice::CreateFence(const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) const
    {
        const auto result = vkCreateFence(mLogicalDevice, pCreateInfo, pAllocator, pFence);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    VkResult VulkanDevice::WaitForFences(uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) const
    {
        const auto result = vkWaitForFences(mLogicalDevice, fenceCount, pFences, waitAll, timeout);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    VkResult VulkanDevice::ResetFences(uint32_t fenceCount, const VkFence* pFences) const
    {
        const auto result = vkResetFences(mLogicalDevice, fenceCount, pFences);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::DestroyFence(VkFence fence, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyFence(mLogicalDevice, fence, pAllocator);
    }
    
    VkResult VulkanDevice::CreateBuffer(const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) const
    {
        const auto result = vkCreateBuffer(mLogicalDevice, pCreateInfo, pAllocator, pBuffer);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::GetBufferMemoryRequirements(VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) const
    {
        vkGetBufferMemoryRequirements(mLogicalDevice, buffer, pMemoryRequirements);
    }
    
    VkResult VulkanDevice::BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) const
    {
        const auto result = vkBindBufferMemory(mLogicalDevice, buffer, memory, memoryOffset);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::DestroyBuffer(VkBuffer buffer, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyBuffer(mLogicalDevice, buffer, pAllocator);
    }
    
    VkResult VulkanDevice::AllocateMemory(const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) const
    {
        const auto result = vkAllocateMemory(mLogicalDevice, pAllocateInfo, pAllocator, pMemory);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::FreeMemory(VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) const
    {
        vkFreeMemory(mLogicalDevice, memory, pAllocator);
    }
    
    VkResult VulkanDevice::MapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) const
    {
        const auto result = vkMapMemory(mLogicalDevice, memory, offset, size, flags, ppData);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::UnmapMemory(VkDeviceMemory memory) const
    {
        vkUnmapMemory(mLogicalDevice, memory);
    }
    
    VkResult VulkanDevice::QueueWaitIdle(VkQueue queue) const
    {
        const auto result = vkQueueWaitIdle(queue);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    VkResult VulkanDevice::CreateSampler(const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) const
    {
        const auto result = vkCreateSampler(mLogicalDevice, pCreateInfo, pAllocator, pSampler);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::DestroySampler(VkSampler sampler, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroySampler(mLogicalDevice, sampler, pAllocator);
    }
    
    void VulkanDevice::CmdBindVertexBuffer(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const
    {
        vkCmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    }
    
    void VulkanDevice::CmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) const
    {
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    }
    
    void VulkanDevice::CmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) const
    {
        vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    }
    
    void VulkanDevice::CmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const
    {
        vkCmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
    }
    
    void VulkanDevice::CmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const
    {
        vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
    
    void VulkanDevice::CmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) const
    {
        vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    }
    
    void VulkanDevice::CmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) const
    {
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
    
    VkResult VulkanDevice::CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) const
    {
        const auto result = vkCreateDescriptorSetLayout(mLogicalDevice, pCreateInfo, pAllocator, pSetLayout);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyDescriptorSetLayout(mLogicalDevice, descriptorSetLayout, pAllocator);
    }
    
    VkResult VulkanDevice::CreateDescriptorPool(const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) const
    {
        const auto result = vkCreateDescriptorPool(mLogicalDevice, pCreateInfo, pAllocator, pDescriptorPool);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::DestroyDescriptorPool(VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) const
    {
        vkDestroyDescriptorPool(mLogicalDevice, descriptorPool, pAllocator);
    }
    
    VkResult VulkanDevice::AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) const
    {
        const auto result = vkAllocateDescriptorSets(mLogicalDevice, pAllocateInfo, pDescriptorSets);
        VK_CHECK_RESULT(result);
        return result;
    }
    
    void VulkanDevice::UpdateDescriptorSets(uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) const
    {
        vkUpdateDescriptorSets(mLogicalDevice, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    }
    
    VkResult VulkanDevice::FreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) const
    {
        const auto result = vkFreeDescriptorSets(mLogicalDevice, descriptorPool, descriptorSetCount, pDescriptorSets);
        VK_CHECK_RESULT(result);
        return result;
    }

	void VulkanDevice::LoadFunctions(PFN_vkGetDeviceProcAddr loadFunc)
	{
		LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyDevice);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDeviceWaitIdle);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkQueueSubmit);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkGetDeviceQueue);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateImage);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyImage);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkBindImageMemory);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateImageView);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyImageView);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkGetImageMemoryRequirements);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateShaderModule);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyShaderModule);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreatePipelineLayout);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyPipelineLayout);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateGraphicsPipelines);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyPipeline);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyRenderPass);
        
        // Samplers
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateSampler);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroySampler);
        
        // Command buffers
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBindVertexBuffers);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdCopyBuffer);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdCopyBufferToImage);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBeginRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdEndRenderPass);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBindPipeline);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdDraw);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBindIndexBuffer);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdDrawIndexed);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdBindDescriptorSets);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCmdPipelineBarrier);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateDescriptorSetLayout);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyDescriptorSetLayout);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateDescriptorPool);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyDescriptorPool);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkAllocateDescriptorSets);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkUpdateDescriptorSets);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkFreeDescriptorSets);
        
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
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateBuffer);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkGetBufferMemoryRequirements);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkBindBufferMemory);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyBuffer);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkAllocateMemory);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkMapMemory);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkFreeMemory);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkUnmapMemory);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkCreateFence);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkWaitForFences);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkResetFences);
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyFence);
        
        LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkQueueWaitIdle);

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
			LOG(Warning) << "Device Extension: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME << " not available.";
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
