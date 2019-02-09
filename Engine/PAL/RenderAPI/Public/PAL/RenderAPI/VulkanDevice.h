#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

#include <exception>
#include <vector>

namespace PAL::RenderAPI
{
	struct no_device_func : std::runtime_error { using std::runtime_error::runtime_error; };

	struct RENDERAPI_API DeviceData
	{
		VkPhysicalDevice device{ VK_NULL_HANDLE };
		VkDevice logicalDevice{ VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures deviceFeatures{ VK_NULL_HANDLE };
		VkPhysicalDeviceProperties deviceProperties{ VK_NULL_HANDLE };
		std::vector<VkExtensionProperties> deviceExtensions;
		PFN_vkGetDeviceProcAddr deviceProcAddrFunc{ nullptr };
	};

	class RENDERAPI_API VulkanDevice
	{
	public:
		explicit VulkanDevice(DeviceData& deviceData);
		~VulkanDevice();
        
        const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
        const VkDevice& GetDevice() const { return mLogicalDevice; }

		void CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) const;
		void DestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) const;
        VkResult AcquireNextImageKHR(VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex) const;
        void QueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo) const;
        std::vector<VkImage> GetSwapchainImagesKHR(VkSwapchainKHR swapchain) const;
        
        void CreateShaderModule(const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule) const;
        void DestroyShaderModule(VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator) const;
        void CreatePipelineLayout(const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout) const;
        void DestroyPipelineLayout(VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator) const;
        void CreateGraphicsPipeline(VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines) const;
        void DestroyPipeline(VkPipeline pipeline, const VkAllocationCallbacks* pAllocator) const;
        
        void CreateRenderPass(const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass) const;
        void DestroyRenderPass(VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator) const;
        void BeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents) const;
        void EndRenderPass(VkCommandBuffer commandBuffer) const;
        void BindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const;
        void Draw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;
        
        void CreateFramebuffer(const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer) const;
        void DestroyFramebuffer(VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator) const;
        
        void CreateCommandPool(const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool) const;
        void DestroyCommandPool(VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator) const;
        void ResetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags) const;
        void AllocateCommandBuffers(const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers) const;
        void FreeCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers) const;
        void BeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo) const;
        void EndCommandBuffer(VkCommandBuffer commandBuffer) const;
        void ResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) const;
        
        void WaitIdle() const;
        void QueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence) const;
        void GetDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) const;
        
        VkImageView CreateImageView(const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator) const;
        void DestroyImageView(VkImageView imageView, const VkAllocationCallbacks* pAllocator) const;
        
        void CreateSemaphore(const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore) const;
        void DestroySemaphore(VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator) const;
        
        VkResult CreateFence(const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence) const;
        VkResult WaitForFences(uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout) const;
        VkResult ResetFences(uint32_t fenceCount, const VkFence* pFences) const;
        void DestroyFence(VkFence fence, const VkAllocationCallbacks* pAllocator) const;
        
        VkResult CreateBuffer(const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer) const;
        void GetBufferMemoryRequirements(VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) const;
        VkResult BindBufferMemory(VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) const;
        void DestroyBuffer(VkBuffer buffer, const VkAllocationCallbacks* pAllocator) const;
        
        VkResult AllocateMemory(const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory) const;
        void FreeMemory(VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator) const;
        VkResult MapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) const;
        void UnmapMemory(VkDeviceMemory memory) const;
        
        VkResult QueueWaitIdle(VkQueue queue) const;
        
        // Command buffers
        void CmdBindVertexBuffer(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const;
        void CmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) const;
        void CmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
        void CmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const;
        
        // Descriptors        
        VkResult CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) const;
        void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) const;
        VkResult CreateDescriptorPool(const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) const;
        void DestroyDescriptorPool(VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) const;

	private:
		void LoadFunctions(PFN_vkGetDeviceProcAddr loadFunc);
		bool IsExtensionSupported(const char* extensionName) const;

	private:
		VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
		VkDevice mLogicalDevice{ VK_NULL_HANDLE };
		VkPhysicalDeviceProperties mDeviceProperties{ VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures mDeviceFeatures{ VK_NULL_HANDLE };
        
		std::vector<VkExtensionProperties> mDeviceExtensions;
        
        PFN_vkCreateShaderModule vkCreateShaderModule{ nullptr };
        PFN_vkDestroyShaderModule vkDestroyShaderModule{ nullptr };
        PFN_vkCreatePipelineLayout vkCreatePipelineLayout{ nullptr };
        PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout{ nullptr };
        PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines{ nullptr };
        PFN_vkDestroyPipeline vkDestroyPipeline{ nullptr };
        
        PFN_vkGetDeviceQueue vkGetDeviceQueue{ nullptr };
        PFN_vkCreateRenderPass vkCreateRenderPass{ nullptr };
        PFN_vkDestroyRenderPass vkDestroyRenderPass{ nullptr };
        PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass{ nullptr };
        PFN_vkCmdEndRenderPass vkCmdEndRenderPass{ nullptr };
        PFN_vkCmdBindPipeline vkCmdBindPipeline{ nullptr };
        PFN_vkCmdDraw vkCmdDraw{ nullptr };
        
        PFN_vkCreateFramebuffer vkCreateFramebuffer{ nullptr };
        PFN_vkDestroyFramebuffer vkDestroyFramebuffer{ nullptr };
        
        PFN_vkCreateCommandPool vkCreateCommandPool{ nullptr };
        PFN_vkDestroyCommandPool vkDestroyCommandPool{ nullptr };
        PFN_vkResetCommandPool vkResetCommandPool{ nullptr };
        PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers{ nullptr };
        PFN_vkFreeCommandBuffers vkFreeCommandBuffers{ nullptr };
        PFN_vkBeginCommandBuffer vkBeginCommandBuffer{ nullptr };
        PFN_vkEndCommandBuffer vkEndCommandBuffer{ nullptr };
        PFN_vkResetCommandBuffer vkResetCommandBuffer{ nullptr };
        PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers{ nullptr };
        PFN_vkCmdCopyBuffer vkCmdCopyBuffer{ nullptr };
        
        PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer{ nullptr };
        PFN_vkCmdDrawIndexed vkCmdDrawIndexed{ nullptr };
        
		PFN_vkDestroyDevice vkDestroyDevice{ nullptr };
        PFN_vkDeviceWaitIdle vkDeviceWaitIdle{ nullptr };
        PFN_vkQueueSubmit vkQueueSubmit{ nullptr };
        PFN_vkQueueWaitIdle vkQueueWaitIdle{ nullptr };
        
        PFN_vkCreateImageView vkCreateImageView{ nullptr };
        PFN_vkDestroyImageView vkDestroyImageView{ nullptr };
        
        // Descriptors
        PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout{ nullptr };
        PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout{ nullptr };
        PFN_vkCreateDescriptorPool vkCreateDescriptorPool{ nullptr };
        PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool{ nullptr };
        
        // Buffers
        PFN_vkCreateBuffer vkCreateBuffer{ nullptr };
        PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements{ nullptr };
        PFN_vkBindBufferMemory vkBindBufferMemory{ nullptr };
        PFN_vkDestroyBuffer vkDestroyBuffer{ nullptr };
        
        // Memory
        PFN_vkAllocateMemory vkAllocateMemory{ nullptr };
        PFN_vkFreeMemory vkFreeMemory{ nullptr };
        PFN_vkMapMemory vkMapMemory{ nullptr };
        PFN_vkUnmapMemory vkUnmapMemory{ nullptr };
        
        // Semaphores
        PFN_vkCreateSemaphore vkCreateSemaphore{ nullptr };
        PFN_vkDestroySemaphore vkDestroySemaphore{ nullptr };
        
        // Fences
        PFN_vkCreateFence vkCreateFence{ nullptr };
        PFN_vkDestroyFence vkDestroyFence{ nullptr };
        PFN_vkWaitForFences vkWaitForFences{ nullptr };
        PFN_vkResetFences vkResetFences{ nullptr };

		// VK_KHR_swapchain extension
		PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR{ nullptr };
		PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR{ nullptr };
        PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR{ nullptr };
        PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR{ nullptr };
        PFN_vkQueuePresentKHR vkQueuePresentKHR{ nullptr };
	};
}
