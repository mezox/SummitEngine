#pragma once

#include "VulkanDeviceObjects.h"
#include <PAL/RenderAPI/VulkanDevice.h>
#include <Math/Vector2.h>
#include <Math/Vector4.h>

#include "Command.h"
#include <exception>

namespace Renderer::Vulkan
{
    template<typename Derived>
    class VulkanCommand
    {
    public:
        void Execute(const PAL::RenderAPI::VulkanDevice& device, const DeviceObject& commandBuffer) const
        {
            CommandBufferVisitor cmdBufVisitor;
            commandBuffer.Accept(cmdBufVisitor);
            
            if(cmdBufVisitor.commandBuffer != VK_NULL_HANDLE)
            {
                auto& derived = static_cast<const Derived&>(*this);
                derived.OnExecute(device, cmdBufVisitor.commandBuffer);
            }
            else
            {
                throw std::runtime_error("CMD");
            }
        }
    };
    
    class BeginCommand final : public VulkanCommand<BeginCommand>
    {
    public:
        BeginCommand(VkCommandBufferUsageFlags flags)
            : mFlags(flags)
        {}
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::Begin";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = mFlags;
            
            device.BeginCommandBuffer(cmdBuffer, &beginInfo);
        }
        
    private:
        VkCommandBufferUsageFlags mFlags;
    };
    
    class EndCommand final : public VulkanCommand<EndCommand>
    {
    public:
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::End";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.EndCommandBuffer(cmdBuffer);
        }
    };
    
    class BeginRenderPass final : public VulkanCommand<BeginRenderPass>
    {
    public:
        BeginRenderPass(const Vector2f& viewport, const DeviceObject& renderPass, const DeviceObject& framebuffer)
            : mViewPort(viewport)
        {
            RenderPassVisitor rpv;
            renderPass.Accept(rpv);
            
            mRenderPass = rpv.renderPass;
            
            FramebufferObjectVisitor fbv;
            framebuffer.Accept(fbv);
            
            mFrameBuffer = fbv.framebuffer;
        }
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::BeginRenderPass";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.1f, 0.5f, 0.85f, 1.0f };
            clearValues[1].depthStencil = {1.0f, 0};
            
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = mRenderPass;
            renderPassInfo.framebuffer = mFrameBuffer;
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = { (uint32_t)mViewPort.x, (uint32_t)mViewPort.y };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();
            
            device.BeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        }
        
    private:
        Vector2f mViewPort;
        VkRenderPass mRenderPass;
        VkFramebuffer mFrameBuffer;
    };
    
    class EndRenderPass final : public VulkanCommand<EndRenderPass>
    {
    public:
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::EndRenderPass";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.EndRenderPass(cmdBuffer);
        }
    };
    
    class BindVertexBuffer final : public VulkanCommand<BindVertexBuffer>
    {
    public:
        BindVertexBuffer(const VertexBufferBase& vb)
        {
            for(const auto& streamPtr : vb.mStreams)
            {
                BufferObjectVisitor bufferVisitor;
                if(streamPtr->GetDataType() == BufferUsage::VertexBuffer)
                {
                    const auto& bufferDeviceObject = streamPtr->GetDeviceResourcePtr();
                    bufferDeviceObject.Accept(bufferVisitor);
                    
                    mBuffers.push_back(bufferVisitor.buffer);
                    mOffsets.push_back(0);
                }
            }
        }
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::BindVertexBuffer";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.CmdBindVertexBuffer(cmdBuffer, 0, mBuffers.size(), mBuffers.data(), mOffsets.data());
        }
        
    private:
        std::vector<VkBuffer> mBuffers;
        std::vector<VkDeviceSize> mOffsets;
    };
    
    class BindIndexBuffer final : public VulkanCommand<BindIndexBuffer>
    {
    public:
        BindIndexBuffer(const VertexBufferBase& vb)
        {
            for(const auto& streamPtr : vb.mStreams)
            {
                BufferObjectVisitor bufferVisitor;
                if(streamPtr->GetDataType() == BufferUsage::IndexBuffer)
                {
                    const auto& bufferDeviceObject = streamPtr->GetDeviceResourcePtr();
                    bufferDeviceObject.Accept(bufferVisitor);
                    
                    mBuffer = bufferVisitor.buffer;
                    mIndexType = (streamPtr->GetStride() == sizeof(uint16_t)) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
                    break;
                }
            }
        }
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::BindIndexBuffer";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.CmdBindIndexBuffer(cmdBuffer, mBuffer, 0, mIndexType);
        }
        
    private:
        VkBuffer mBuffer;
        VkIndexType mIndexType;
    };
    
    class BindPipeline final : public VulkanCommand<BindPipeline>
    {
    public:
        BindPipeline(const DeviceObject& pipelineDeviceObject)
        {
            PipelineObjectVisitor pv;
            pipelineDeviceObject.Accept(pv);
            
            mPipeline = pv.pipeline;
        }
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::BindPipeline";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.BindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
        }
        
    private:
        VkPipeline mPipeline;
    };
    
    class DrawIndexed final : public VulkanCommand<DrawIndexed>
    {
    public:
        DrawIndexed(const uint32_t elemCnt, const uint32_t fIdx, const uint32_t vOffset)
            : mElementCount(elemCnt)
            , mFirstIndex(fIdx)
            , mVertexOffset(vOffset)
        {}
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::DrawIndexed";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.CmdDrawIndexed(cmdBuffer, mElementCount, mInstanceCount, mFirstIndex, mVertexOffset, mFirstInstance);
        }
        
    private:
        uint32_t mElementCount{ 0 };
        uint32_t mInstanceCount{ 1 };
        uint32_t mFirstIndex{ 0 };
        uint32_t mVertexOffset{ 0 };
        uint32_t mFirstInstance{ 0 };
    };
    
    class BindDescriptorSets final : public VulkanCommand<BindDescriptorSets>
    {
    public:
        BindDescriptorSets(const DeviceObject& pipeline, const DeviceObject& descriptorSet)
        {
            PipelineObjectVisitor pipelineVisitor;
            pipeline.Accept(pipelineVisitor);
            
            mLayout = pipelineVisitor.layout;
            
            DescriptorSetVisitor descriptorSetVisitor;
            descriptorSet.Accept(descriptorSetVisitor);
            
            mDescriptorSet = descriptorSetVisitor.descriptorSet;
        }
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::BindDescriptorSets";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.CmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout, 0, 1, &mDescriptorSet, 0, nullptr);
        }
        
    private:
        VkPipelineLayout mLayout;
        VkDescriptorSet mDescriptorSet;
    };
    
    class SetViewport final : public VulkanCommand<SetViewport>
    {
    public:
        SetViewport(const uint32_t w, const uint32_t h)
        {
            mViewport.x = 0.0f;
            mViewport.y = 0.0f;
            mViewport.width = w;
            mViewport.height = h;
            mViewport.minDepth = 0.0f;
            mViewport.maxDepth = 1.0f;
        }
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::SetViewport";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.CmdSetViewport(cmdBuffer, 0, 1, &mViewport);
        }
        
    private:
        VkViewport mViewport;
    };
    
    class SetScissor final : public VulkanCommand<SetScissor>
    {
    public:
        SetScissor(VkRect2D&& rect)
            : mScissor(std::move(rect))
        {}
        
        [[nodiscard]] std::string GetDescription() const noexcept
        {
            return "CommandBuffer::SetScissor";
        }
        
        void OnExecute(const PAL::RenderAPI::VulkanDevice& device, const VkCommandBuffer& cmdBuffer) const
        {
            device.CmdSetScissor(cmdBuffer, 0, 1, &mScissor);
        }
        
    private:
        VkRect2D mScissor;
    };
}
