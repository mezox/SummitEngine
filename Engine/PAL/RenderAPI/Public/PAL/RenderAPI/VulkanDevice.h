#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

#include <exception>
#include <vector>
#include <mutex>

namespace PAL::RenderAPI
{
    class VulkanDevice;
}

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
    
    enum class DeviceFeature
    {
        None = 0x00000000,
        AnisotropicFiltering = 0x00000001
    };
    
    template<typename T>
    class MovableHandle
    {
    public:
        MovableHandle() = default;
        MovableHandle(const MovableHandle& other) = delete;
        MovableHandle& operator=(const MovableHandle& other) = delete;
        
        MovableHandle(MovableHandle&& other)
        {
            mHandle = other.mHandle;
            other.mHandle = VK_NULL_HANDLE;
        }
        
        MovableHandle& operator=(MovableHandle&& other)
        {
            mHandle = other.mHandle;
            other.mHandle = VK_NULL_HANDLE;
            return *this;
        }
        
        MovableHandle(T& directHandle)
            : mHandle(directHandle)
        {}
        
        MovableHandle(T&& directHandle)
            : mHandle(std::move(directHandle))
        {}
        
        const T& Get() const
        {
            return mHandle;
        }
        
        T& Get()
        {
            return mHandle;
        }
        
    private:
        T mHandle{ VK_NULL_HANDLE };
    };
    
    template<typename T>
    class ManagedHandle
    {
    public:
        using DeleterType = std::function<void(const T&)>;
        
    public:
        ManagedHandle() = delete;
        ManagedHandle(const ManagedHandle& other) = delete;
        ManagedHandle operator=(const ManagedHandle& other) = delete;
        
        ManagedHandle(T& handle, DeleterType&& deleter)
            : mHandle(handle)
            , mDeleter(std::move(deleter))
        {}
        
        ManagedHandle(ManagedHandle&& other)
        {
            mHandle = std::move(other.mHandle);
            mDeleter = std::move(other.mDeleter);
            other.mDeleter = nullptr;
        }
        
        ManagedHandle& operator=(ManagedHandle&& other)
        {
            mHandle = std::move(other.mHandle);
            mDeleter = std::move(other.mDeleter);
            other.mDeleter = nullptr;
            return *this;
        }
        
        const T& Get() const
        {
            return mHandle.Get();
        }
        
        T& Get()
        {
            return mHandle.Get();
        }
        
        ~ManagedHandle()
        {
            if(mDeleter)
            {
                mDeleter(mHandle.Get());
            }
        }
        
    private:
        MovableHandle<T> mHandle;
        std::function<void(const T&)> mDeleter;
    };

    class RENDERAPI_API VulkanDevice : public std::enable_shared_from_this<VulkanDevice>
	{
	public:
		explicit VulkanDevice(DeviceData& deviceData);
		~VulkanDevice();
        
        const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
        const VkDevice& GetDevice() const { return mLogicalDevice; }
        
        bool IsFeatureSupported(DeviceFeature f) const;

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
        
        VkResult CreateImage(const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage) const;
        void DestroyImage(VkImage image, const VkAllocationCallbacks* pAllocator) const;
        VkResult CreateImageView(const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pImageView) const;
        void DestroyImageView(VkImageView imageView, const VkAllocationCallbacks* pAllocator) const;
        VkResult BindImageMemory(VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) const;
        void GetImageMemoryRequirements(VkImage image, VkMemoryRequirements* pMemoryRequirements) const;
        
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
        VkResult FlushMappedMemoryRanges(uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges) const;
        
        VkResult QueueWaitIdle(VkQueue queue) const;
        
        // Samplers
        VkResult    CreateSampler(const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler) const;
        void        DestroySampler(VkSampler sampler, const VkAllocationCallbacks* pAllocator) const;
        
        // Command buffers
        void CmdBindVertexBuffer(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets) const;
        void CmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions) const;
        void CmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions) const;
        void CmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) const;
        void CmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const;
        void CmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets) const;
        void CmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers) const;
        void CmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports) const;
        void CmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors) const;
        
        // Descriptors        
        VkResult CreateDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout) const;
        void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator) const;
        VkResult CreateDescriptorPool(const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool) const;
        void DestroyDescriptorPool(VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator) const;
        VkResult AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets) const;
        void UpdateDescriptorSets(uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies) const;
        VkResult FreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets) const;
        
        [[nodiscard]] ManagedHandle<VkSwapchainKHR> CreateManagedSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator) const;
        [[nodiscard]] ManagedHandle<VkRenderPass> CreateManagedRenderPass(const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator) const;
        [[nodiscard]] ManagedHandle<VkSemaphore> CreateManagedSemaphore(const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator) const;
        [[nodiscard]] ManagedHandle<VkFence> CreateManagedFence(const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator) const;

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
        PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage{ nullptr };
        PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier{ nullptr };
        PFN_vkCmdSetViewport vkCmdSetViewport{ nullptr };
        PFN_vkCmdSetScissor vkCmdSetScissor{ nullptr };
        
        PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets{ nullptr };
        PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer{ nullptr };
        PFN_vkCmdDrawIndexed vkCmdDrawIndexed{ nullptr };
        
		PFN_vkDestroyDevice vkDestroyDevice{ nullptr };
        PFN_vkDeviceWaitIdle vkDeviceWaitIdle{ nullptr };
        PFN_vkQueueSubmit vkQueueSubmit{ nullptr };
        PFN_vkQueueWaitIdle vkQueueWaitIdle{ nullptr };
        
        // Images
        PFN_vkCreateImage vkCreateImage{ nullptr };
        PFN_vkDestroyImage vkDestroyImage{ nullptr };
        PFN_vkCreateImageView vkCreateImageView{ nullptr };
        PFN_vkDestroyImageView vkDestroyImageView{ nullptr };
        PFN_vkBindImageMemory vkBindImageMemory{ nullptr };
        PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements{ nullptr };
        
        // Descriptors
        PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout{ nullptr };
        PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout{ nullptr };
        PFN_vkCreateDescriptorPool vkCreateDescriptorPool{ nullptr };
        PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool{ nullptr };
        PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets{ nullptr };
        PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets{ nullptr };
        PFN_vkFreeDescriptorSets vkFreeDescriptorSets{ nullptr };
        
        // Buffers
        PFN_vkCreateBuffer vkCreateBuffer{ nullptr };
        PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements{ nullptr };
        PFN_vkBindBufferMemory vkBindBufferMemory{ nullptr };
        PFN_vkDestroyBuffer vkDestroyBuffer{ nullptr };
        
        // Samplers
        PFN_vkCreateSampler vkCreateSampler{ nullptr };
        PFN_vkDestroySampler vkDestroySampler{ nullptr };
        
        // Memory
        PFN_vkAllocateMemory vkAllocateMemory{ nullptr };
        PFN_vkFreeMemory vkFreeMemory{ nullptr };
        PFN_vkMapMemory vkMapMemory{ nullptr };
        PFN_vkUnmapMemory vkUnmapMemory{ nullptr };
        PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges{ nullptr };
        
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
