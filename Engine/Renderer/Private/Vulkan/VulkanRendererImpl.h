 #pragma once

#include <Renderer/Renderer.h>
#include <Renderer/Resources/Framebuffer.h>
#include <Renderer/RenderPass.h>
#include <PAL/RenderAPI/Vulkan/VulkanDevice.h>
#include <memory>

#include <Renderer/DeviceObject.h>
#include <Math/Matrix4.h>

#include "VulkanDeviceObjects.h"
#include "Command.h"

namespace Renderer
{
    class CommandBufferFactory;
    
    struct VulkanImageDesc;
    
	class VulkanRenderer : public IRenderer
	{
	public:
		void Initialize() override;
		void Deinitialize() override;

        DeviceObject CreateSurface(void* nativeViewHandle) const override;
        void CreateSwapChain(std::unique_ptr<SwapChainBase>& swapChain, const DeviceObject& surface, uint32_t width, uint32_t height) override;
        void CreateShader(DeviceObject& shader, const std::vector<uint8_t>& code) const override;
        void CreatePipeline(Pipeline& pipeline) override;
        void CreateFramebuffer(const FramebufferDesc& desc, DeviceObject& framebuffer) override;
        void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) override;
        void CreateImage(const ImageDesc& desc, DeviceObject& image) override;
        void CreateSampler(const SamplerDesc& desc, DeviceObject& sampler) override;
        void CreateTexture(const ImageDesc& desc, const SamplerDesc& samplerDesc, DeviceObject& texture) override;
        DeviceObject CreateSemaphore(const SemaphoreDescriptor& desc) const override;
        DeviceObject CreateFence(const FenceDescriptor& desc) const override;
        DeviceObject CreateEvent(const EventDescriptor& desc) const override;
        void CreateRenderPass(const RenderPassDescriptor& desc, DeviceObject& deviceObject) const override;
        
        void MapMemory(const DeviceObject& deviceObject, uint32_t size, void* data) override;
        void UnmapMemory(const DeviceObject& deviceObject) const override;
        
        void Render(const Object3d& vb, const Pipeline& pipeline) override;
        void RenderGui(const VertexBufferBase& vb, const Pipeline& pipeline) override;
        
        void DestroyDeviceObject(DeviceObject& buffer) const override;
        
        
        void BeginCommandRecording(SwapChainBase* swapChain) override;
        void EndCommandRecording(SwapChainBase* swapChain) override;
        
        
        const std::vector<DeviceObject>& GetCommandBuffers() const { return mCommandBuffers; }
        const VkQueue GetGraphicsQueue() const { return mGraphicsQueue; }
        
        VkImageView CreateImageView(const VkImage& image, const VkFormat& format, VkImageAspectFlags flags);
        
        VulkanAttachmentDeviceObject CreateAttachment(uint32_t width, uint32_t height, Format format, ImageUsage usage);

    private:
        void CreateDevice(DeviceType type);
        
        uint32_t FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
        
        // Pipeline
        std::vector<VkPipelineShaderStageCreateInfo> PrepareModules(Effect& effect) const;  // Non-const because it stores module device objects back to effect. This might not be needed & could be stored in some pipeline manager?
        
        
    private:
        [[nodiscard]] BufferDeviceObject    CreateBufferImpl(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode) const;
        [[nodiscard]] ImageDeviceObject     CreateImageImpl(const VulkanImageDesc& descriptor) const;
        [[nodiscard]] VkFramebuffer         CreateFramebufferImpl(uint32_t width, uint32_t height, const std::vector<VkImageView>& attachments, const VkRenderPass& renderPass) const;
        [[nodiscard]] VkSampler             CreateSamplerImpl(const SamplerDesc& descriptor) const;
        
	private:
		std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        VkCommandPool mCommandPool{ VK_NULL_HANDLE };
        std::vector<DeviceObject> mCommandBuffers;
        VkDescriptorPool mDescriptorPool;
        
        VkFormat mImgFormat{ VK_FORMAT_B8G8R8A8_UNORM };   //TODO: Initialize
        VkQueue mGraphicsQueue{ VK_NULL_HANDLE };
        
        std::vector<DeviceObject*> mResourceManager;
        
        std::shared_ptr<CommandBufferFactory> mCommandBufferFactory;
        
        RenderPass mDefaultRenderPass;
        
        //VulkanAttachmentDeviceObject mShadowMap;

        VkCommandBuffer mCmdBuff;
        std::vector<Command> mCmdList;
	};
}
