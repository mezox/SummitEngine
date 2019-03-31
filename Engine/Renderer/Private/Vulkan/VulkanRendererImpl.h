 #pragma once

#include <Renderer/Renderer.h>
#include <Renderer/Resources/Framebuffer.h>
#include <Renderer/RenderPass.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include <memory>

#include <Renderer/DeviceObject.h>
#include <Math/Matrix4.h>

#include "VulkanDeviceObjects.h"

namespace Renderer
{
    class CommandBufferFactory;
    
    struct VulkanImageDesc;
    
    class VulkanFrameData
    {
    public:
        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> frameFence;
    };
    
    struct VulkanFramebufferDesc
    {
        std::vector<VkImageView> attachments{ VK_NULL_HANDLE };
        VkRenderPass renderPass{ VK_NULL_HANDLE };
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

        void CreateSwapChain(std::unique_ptr<SwapChainBase>& swapChain, void* nativeHandle, uint32_t width, uint32_t height) override;
        void CreateShader(DeviceObject& shader, const std::vector<uint8_t>& code) const override;
        void CreatePipeline(Pipeline& pipeline) override;
        void CreateFramebuffer(const FramebufferDesc& desc, DeviceObject& framebuffer) override;
        void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) override;
        void CreateImage(const ImageDesc& desc, DeviceObject& image) override;
        void CreateCommandBuffers(const Pipeline& pipeline, Object3D& object) override;
        void CreateSampler(const SamplerDesc& desc, DeviceObject& sampler) override;
        void CreateTexture(const ImageDesc& desc, const SamplerDesc& samplerDesc, DeviceObject& texture) override;
        
        void MapMemory(uint32_t size, void* data) override;
        
        void CreateRenderPass();
        
        const std::vector<VkCommandBuffer>& GetCommandBuffers() const { return mCommandBuffers; }
        const VkQueue GetGraphicsQueue() const { return mGraphicsQueue; }
        
        void NewFrame(uint32_t index) { mCurrentFrameId = index; }
        const FrameSyncData GetFrameSyncData() const;
        
        VkImageView CreateImageView(const VkImage& image, const VkFormat& format, VkImageAspectFlags flags);
        
        VulkanAttachmentDeviceObject CreateAttachment(uint32_t width, uint32_t height, Format format, ImageUsage usage);

    private:
        void CreateDevice(DeviceType type);
        
        uint32_t FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
        void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
        
    private:
        [[nodiscard]] BufferDeviceObject    CreateBufferImpl(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode) const;
        [[nodiscard]] ImageDeviceObject     CreateImageImpl(const VulkanImageDesc& descriptor) const;
        [[nodiscard]] VkFramebuffer         CreateFramebufferImpl(uint32_t width, uint32_t height, const std::vector<VkImageView>& attachments, const VkRenderPass& renderPass) const;
        [[nodiscard]] VkSampler             CreateSamplerImpl(const SamplerDesc& descriptor) const;
        
	private:
		std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
        VkCommandPool mCommandPool{ VK_NULL_HANDLE };
        std::vector<VkCommandBuffer> mCommandBuffers;
        std::vector<VkFramebuffer> mFramebuffers;
        VkDescriptorPool mDescriptorPool;
        
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        
        VkFormat mImgFormat{ VK_FORMAT_B8G8R8A8_UNORM };   //TODO: Initialize
        size_t mCurrentFrameId{ 0 };
        VkQueue mGraphicsQueue{ VK_NULL_HANDLE };
        
        std::vector<DeviceObject*> mResourceManager;
        
        std::shared_ptr<CommandBufferFactory> mCommandBufferFactory;
        
        VulkanFrameData mFrameData;
        
        // Move to application
        std::unique_ptr<Image> mTexture;
        VkImage textureImage;
        VkImageView textureImageView;
        VkSampler textureSampler;
        
        // Depth
        RenderPass mDefaultRenderPass;
        VkRenderPass mRenderPass;
	};
}
