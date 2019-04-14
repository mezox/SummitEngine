#include "VulkanCommandBuffer.h"

using namespace Renderer;
using namespace Renderer::Vulkan;

VulkanCommandBuffer::VulkanCommandBuffer(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device, const VkCommandPool& cp)
    : mDevice(std::move(device))
    , mCommandPool(cp)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mCommandPool;
    allocInfo.commandBufferCount = 1;
    
    mDevice->AllocateCommandBuffers(&allocInfo, &mCommandBuffer);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    mDevice->FreeCommandBuffers(mCommandPool, 1, &mCommandBuffer);
}

void VulkanCommandBuffer::Begin(VkCommandBufferUsageFlags flags) const
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    mDevice->BeginCommandBuffer(mCommandBuffer, &beginInfo);
}

void VulkanCommandBuffer::CopyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, const VkDeviceSize size) const
{
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    
    mDevice->CmdCopyBuffer(mCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
}

void VulkanCommandBuffer::CopyBufferToImage(const VkBuffer& buffer, const VkImage& image, const uint32_t width, const uint32_t height) const
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    
    mDevice->CmdCopyBufferToImage(mCommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void VulkanCommandBuffer::PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const std::vector<VkMemoryBarrier>& memoryBarriers, const std::vector<VkBufferMemoryBarrier>& bufferMemoryBarriers, const std::vector<VkImageMemoryBarrier>& imageMemoryBarriers) const
{
    mDevice->CmdPipelineBarrier(mCommandBuffer,
                                srcStageMask, dstStageMask,
                                dependencyFlags,
                                static_cast<uint32_t>(memoryBarriers.size()), memoryBarriers.data(),
                                static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(),
                                static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data());
}

void VulkanCommandBuffer::Submit(const VkQueue& queue) const
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffer;
    
    mDevice->QueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    mDevice->QueueWaitIdle(queue);
}

void VulkanCommandBuffer::End() const
{
    mDevice->EndCommandBuffer(mCommandBuffer);
}
