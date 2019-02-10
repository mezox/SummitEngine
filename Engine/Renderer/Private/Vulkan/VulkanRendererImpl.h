 #pragma once

#include <Renderer/Renderer.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include <memory>

#include <Renderer/DeviceObject.h>

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
    
    
    class VulkanShaderModule : public RendererResource
    {
    public:
        VkShaderModule vulkanShader{ VK_NULL_HANDLE };
    };
    
    class VulkanFrameData
    {
    public:
        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> frameFence;
    };
    
    struct FrameSyncData
    {
        VkSemaphore imageAvailableSemaphore{ VK_NULL_HANDLE };
        VkSemaphore renderFinishedSemaphore{ VK_NULL_HANDLE };
        VkFence frameFence{ VK_NULL_HANDLE };
    };
    
	class VulkanRenderer : public IRenderer
	{
	public:
		void Initialize() override;
		void Deinitialize() override;

        void CreateSwapChain(std::unique_ptr<SwapChainResource>& swapChain, void* nativeHandle, uint32_t width, uint32_t height) override;
        void CreateShader(std::unique_ptr<RendererResource>& shader, const std::vector<uint8_t>& code) const override;
        void CreatePipeline(DeviceObject& object, const std::string& vs, const std::string& fs) const override;
        void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) const override;
        
        void CreateCommandBuffers(const Pipeline& pipeline, Object3D& object) override;
        
        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return mCommandBuffers; }
        const VkQueue GetGraphicsQueue() const { return mGraphicsQueue; }
        const VkRenderPass GetRenderPass() const { return mRenderPass; }
        
        void NewFrame() { mCurrentFrameId = (mCurrentFrameId + 1) % 2; }
        const FrameSyncData GetFrameSyncData() const;
        
    private:
        void CreateDevice(DeviceType type);
        
        uint32_t FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void CreateBufferInternal(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void CreateDescriptorSetLayout();
        void UpdateUniformBuffer(uint32_t currentImage);
        
	private:
		std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        VkRenderPass mRenderPass{ VK_NULL_HANDLE };
        VkCommandPool mCommandPool{ VK_NULL_HANDLE };
        std::vector<VkCommandBuffer> mCommandBuffers;
        std::vector<VkFramebuffer> mFramebuffers;
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        
        VkQueue mGraphicsQueue{ VK_NULL_HANDLE };
        
        VkFormat mImgFormat{ VK_FORMAT_B8G8R8A8_UNORM };   //TODO: Initialize
        size_t mCurrentFrameId{ 0 };
        
        VulkanFrameData mFrameData;
	};
}
