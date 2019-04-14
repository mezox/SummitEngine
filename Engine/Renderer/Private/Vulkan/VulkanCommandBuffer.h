#pragma once

#include <PAL/RenderAPI/VulkanDevice.h>
#include <Renderer/Resources/DeviceResource.h>
#include <Renderer/CommandBuffer.h>

namespace Renderer
{
    class VulkanCommandBuffer
    {
    public:
        VulkanCommandBuffer(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device, const VkCommandPool& cp);
        virtual ~VulkanCommandBuffer();
        
        VulkanCommandBuffer(const VulkanCommandBuffer& cb) = delete;
        VulkanCommandBuffer& operator=(const VulkanCommandBuffer& cb) = delete;
        VulkanCommandBuffer(VulkanCommandBuffer&& cb) = default;
        VulkanCommandBuffer& operator=(VulkanCommandBuffer&& cb) = default;
        
        void Begin(VkCommandBufferUsageFlags flags) const;
        void End() const;
        
        void CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size) const;
        void CopyBufferToImage(const VkBuffer& srcBuffer, const VkImage& dstImage, uint32_t width, uint32_t height) const;
        void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,  const std::vector<VkMemoryBarrier>& memoryBarriers, const std::vector<VkBufferMemoryBarrier>& bufferMemoryBarriers, const std::vector<VkImageMemoryBarrier>& imageMemoryBarriers) const;
        void Submit(const VkQueue& queue) const;
        
    private:
        std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };
        VkCommandPool mCommandPool{ VK_NULL_HANDLE };
    };

    class ScopeCommandBuffer : public VulkanCommandBuffer
    {
    public:
        ScopeCommandBuffer(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device, const VkCommandPool& cp, const VkQueue& queue)
            : VulkanCommandBuffer(std::move(device), cp)
            , mQueue(queue)
        {
            Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        }
        
        ~ScopeCommandBuffer()
        {
            End();
            Submit(mQueue);
        }
        
    private:
        VkQueue mQueue{ VK_NULL_HANDLE };
    };
    
    class CommandBufferFactory
    {
    public:
        CommandBufferFactory(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device, const VkCommandPool& cp, const VkQueue& queue)
        : mDevice(std::move(device))
        , mCommandPool(cp)
        , mQueue(queue)
        {}
        
        VulkanCommandBuffer CreateCommandBuffer()
        {
            return VulkanCommandBuffer(mDevice, mCommandPool);
        }
        
        ScopeCommandBuffer CreateScopeCommandBuffer()
        {
            return ScopeCommandBuffer(mDevice, mCommandPool, mQueue);
        }
        
    private:
        std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };
        VkCommandPool mCommandPool{ VK_NULL_HANDLE };
        VkQueue mQueue{ VK_NULL_HANDLE };
    };
}
