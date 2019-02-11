#pragma once

#include <PAL/RenderAPI/VulkanAPI.h>

namespace Renderer
{
    class PipelineDeviceObject
    {
    public:
        PipelineDeviceObject(const VkPipeline& pipeline, const VkPipelineLayout& layout)
            : mPipeline(pipeline)
            , mPipelineLayout(layout)
        {}
        
        const VkPipeline& GetPipeline() const { return mPipeline; }
        const VkPipelineLayout& GetPipelineLayout() const { return mPipelineLayout; }
        
    private:
        VkPipeline mPipeline{ VK_NULL_HANDLE };
        VkPipelineLayout mPipelineLayout{ VK_NULL_HANDLE };
    };
    
    class BufferDeviceObject
    {
    public:
        BufferDeviceObject(const VkBuffer& buffer, const VkDeviceMemory& memory)
            : mBuffer(buffer)
            , mMemory(memory)
        {}
        
        const VkBuffer& GetBuffer() const { return mBuffer; }
        const VkDeviceMemory& GetMemory() const { return mMemory; }
        
    private:
        VkBuffer mBuffer{ VK_NULL_HANDLE };
        VkDeviceMemory mMemory{ VK_NULL_HANDLE };
    };
    
    class DeviceObjectVisitorBase : public IDeviceObjectVisitor
    {
    public:
        void Visit(const PipelineDeviceObject& object) override {}
        void Visit(const BufferDeviceObject& object) override {}
    };
    
    class PipelineObjectVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const PipelineDeviceObject& object) override
        {
            pipeline = object.GetPipeline();
            layout = object.GetPipelineLayout();
        }
        
    public:
        VkPipeline pipeline{ VK_NULL_HANDLE };
        VkPipelineLayout layout{ VK_NULL_HANDLE };
    };
    
    class BufferObjectVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const BufferDeviceObject& object) override
        {
            buffer = object.GetBuffer();
            memory = object.GetMemory();
        }
        
    public:
        VkBuffer buffer{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
    };
}
