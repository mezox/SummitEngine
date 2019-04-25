#pragma once

#include <PAL/RenderAPI/Vulkan/VulkanAPI.h>
#include <PAL/RenderAPI/Vulkan/VulkanDevice.h>
#include <Core/Assert.h>

#include <array>

namespace Renderer
{
    namespace Vulkan
    {
        template<typename T>
        using PerFrameData = std::array<T, 3>;
        
        class DescriptorSetLayoutDeviceObject
        {
        public:
            DescriptorSetLayoutDeviceObject(const VkDescriptorSetLayout& dsl)
            : descriptorSetLayout(dsl)
            {}
            
        public:
            VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
        };
        
        class DescriptorSetDeviceObject
        {
        public:
            DescriptorSetDeviceObject(const VkDescriptorSet& ds)
            : descriptorSet(ds)
            {}
            
        public:
            VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
        };
        
        struct FenceDeviceObject
        {
            VkFence fence{ VK_NULL_HANDLE };
        };
        
        struct SemaphoreDeviceObject
        {
            VkSemaphore semaphore{ VK_NULL_HANDLE };
        };
        
        struct RenderPassDeviceObject
        {
            ManagedHandle<VkRenderPass> renderPass;
        };
        
        struct CommandBufferDeviceObject
        {
            VkCommandBuffer commandBuffer;
        };
        
        struct FrameSyncDeviceObject
        {
            ManagedHandle<VkSemaphore> imageAvailableSemaphore;
            ManagedHandle<VkSemaphore> renderFinishedSemaphore;
            ManagedHandle<VkFence> frameFence;
        };
        
        struct SwapChainDeviceObject
        {
            MovableHandle<VkSwapchainKHR> swapChain;
            ManagedHandle<VkSemaphore> imageAvailableSemaphore;
            ManagedHandle<VkSemaphore> renderFinishedSemaphore;
            ManagedHandle<VkFence> frameFence;
        };
        
        struct SurfaceDeviceObject
        {
            MovableHandle<VkSurfaceKHR> surface;
        };
    }
    
    class VulkanShaderDeviceObject
    {
    public:
        VulkanShaderDeviceObject(const VkShaderModule& m) : module(m) {}
        
    public:
        VkShaderModule module{ VK_NULL_HANDLE };
    };
    
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
        BufferDeviceObject() = default;
        BufferDeviceObject(const VkBuffer& b, const VkDeviceMemory& m)
            : buffer(b)
            , memory(m)
        {}
        
    public:
        VkBuffer buffer{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        void* mappedMemory{ nullptr };
    };
    
    class ImageDeviceObject
    {
    public:
        ImageDeviceObject() = default;
        ImageDeviceObject(const VkImage& img, const VkDeviceMemory& mem) : image(img), memory(mem) {}
        
    public:
        VkImage image{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
    };
    
    class VulkanAttachmentDeviceObject
    {
    public:
        VulkanAttachmentDeviceObject(const VkImage& img, const VkDeviceMemory& mem, const VkImageView& imgView)
            : image(img), memory(mem), view(imgView)
        {}
        
    public:
        VkImage image{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        VkImageView view{ VK_NULL_HANDLE };
    };
    
    class TextureDeviceObject
    {
    public:
        TextureDeviceObject() = default;
        TextureDeviceObject(const VkImage& img, const VkImageView& view, const VkDeviceMemory& mem, const VkSampler& s) : image(img), imageView(view), memory(mem), sampler(s)
        {
            
        }
        
    public:
        VkImage image{ VK_NULL_HANDLE };
        VkImageView imageView{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        VkSampler sampler{ VK_NULL_HANDLE };
    };
    
    class FramebufferDeviceObject
    {
    public:
        FramebufferDeviceObject() = default;
        
    public:
        VkFramebuffer framebuffer{ VK_NULL_HANDLE };
        VkImageView imageView{ VK_NULL_HANDLE };
        VkImage image{ VK_NULL_HANDLE };
    };
    
    class DeviceObjectVisitorBase : public IDeviceObjectVisitor
    {
    public:
        void Visit(const VulkanShaderDeviceObject& object) override {}
        void Visit(const VulkanRenderPassDeviceObject& object) override {}
        void Visit(const PipelineDeviceObject& object) override {}
        void Visit(const BufferDeviceObject& object) override {}
        void Visit(const FramebufferDeviceObject& object) override {}
        void Visit(const TextureDeviceObject& object) override {}
        void Visit(const VulkanAttachmentDeviceObject& object) override {}
        void Visit(const Vulkan::SurfaceDeviceObject& object) override {}
        void Visit(const Vulkan::SwapChainDeviceObject& object) override {}
        void Visit(const Vulkan::DescriptorSetLayoutDeviceObject& object) override{}
        void Visit(const Vulkan::DescriptorSetDeviceObject& object) override{}
        void Visit(const Vulkan::SemaphoreDeviceObject& object) override{}
        void Visit(const Vulkan::FenceDeviceObject& object) override{}
        void Visit(const Vulkan::EventDeviceObject& object) override{}
        void Visit(const Vulkan::RenderPassDeviceObject& object) override{}
        void Visit(const Vulkan::CommandBufferDeviceObject& object) override{}
        void Visit(const Vulkan::FrameSyncDeviceObject& object) override{}
    };
    
    class MutableDeviceObjectVisitorBase : public IMutableDeviceObjectVisitor
    {
    public:
        void Visit(VulkanShaderDeviceObject& object) override {}
        void Visit(VulkanRenderPassDeviceObject& object) override {}
        void Visit(PipelineDeviceObject& object) override {}
        void Visit(BufferDeviceObject& object) override {}
        void Visit(FramebufferDeviceObject& object) override {}
        void Visit(TextureDeviceObject& object) override {}
        void Visit(VulkanAttachmentDeviceObject& object) override {}
        void Visit(Vulkan::SurfaceDeviceObject& object) override {}
        void Visit(Vulkan::SwapChainDeviceObject& object) override {}
        void Visit(Vulkan::DescriptorSetLayoutDeviceObject& object) override{}
        void Visit(Vulkan::DescriptorSetDeviceObject& object) override{}
        void Visit(Vulkan::SemaphoreDeviceObject& object) override{}
        void Visit(Vulkan::FenceDeviceObject& object) override{}
        void Visit(Vulkan::EventDeviceObject& object) override{}
        void Visit(Vulkan::RenderPassDeviceObject& object) override{}
        void Visit(Vulkan::CommandBufferDeviceObject& object) override{}
        void Visit(Vulkan::FrameSyncDeviceObject& object) override{}
    };
    
    class DeviceObjectCounterVisitorBase : public IDeviceObjectVisitor
    {
    public:
        void Visit(const VulkanShaderDeviceObject& object) override {}
        void Visit(const VulkanRenderPassDeviceObject& object) override {}
        void Visit(const PipelineDeviceObject& object) override { ++buffers; }
        void Visit(const BufferDeviceObject& object) override { ++pipelines; }
        void Visit(const FramebufferDeviceObject& object) override { ++images; ++imageViews; ++framebuffers; }
        void Visit(const TextureDeviceObject& object) override {}
        void Visit(const VulkanAttachmentDeviceObject& object) override {}
        void Visit(const Vulkan::SurfaceDeviceObject& object) override {}
        void Visit(const Vulkan::SwapChainDeviceObject& object) override {}
        void Visit(const Vulkan::DescriptorSetLayoutDeviceObject& object) override{}
        void Visit(const Vulkan::DescriptorSetDeviceObject& object) override {}
        void Visit(const Vulkan::SemaphoreDeviceObject& object) override{}
        void Visit(const Vulkan::FenceDeviceObject& object) override{}
        void Visit(const Vulkan::EventDeviceObject& object) override{}
        void Visit(const Vulkan::RenderPassDeviceObject& object) override{}
        void Visit(const Vulkan::CommandBufferDeviceObject& object) override{}
        void Visit(const Vulkan::FrameSyncDeviceObject& object) override{}
        
    public:
        uint16_t buffers{ 0 };
        uint16_t pipelines{ 0 };
        uint16_t images{ 0 };
        uint16_t imageViews{ 0 };
        uint16_t framebuffers{ 0 };
    };
    
    class SurfaceVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::SurfaceDeviceObject& object) override
        {
            surface = object.surface.Get();
        }
        
    public:
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
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
            buffer = object.buffer;
            memory = object.memory;
        }
        
    public:
        VkBuffer buffer{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
    };
    
    class RenderPassVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::RenderPassDeviceObject& object) override
        {
            renderPass = object.renderPass.Get();
        }
        
    public:
        VkRenderPass renderPass{ VK_NULL_HANDLE };;
    };
    
    class FramebufferObjectVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const FramebufferDeviceObject& object) override
        {
            image = object.image;
            view = object.imageView;
            framebuffer = object.framebuffer;
        }
        
    public:
        VkImage image{ VK_NULL_HANDLE };
        VkImageView view{ VK_NULL_HANDLE };
        VkFramebuffer framebuffer{ VK_NULL_HANDLE };
    };
    
    class VulkanSwapChainVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::SwapChainDeviceObject& object) override
        {
            swapChain = object.swapChain.Get();
            imgAvailableSemaphore = object.imageAvailableSemaphore.Get();
            renderFinishedSemaphore = object.renderFinishedSemaphore.Get();
            frameFence = object.frameFence.Get();
        }
        
    public:
        VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
        VkSemaphore imgAvailableSemaphore{ VK_NULL_HANDLE };
        VkSemaphore renderFinishedSemaphore{ VK_NULL_HANDLE };
        VkFence frameFence{ VK_NULL_HANDLE };
    };
    
    class DescriptorSetVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::DescriptorSetDeviceObject& object) override
        {
            descriptorSet = object.descriptorSet;
        }
        
    public:
        VkDescriptorSet descriptorSet{ VK_NULL_HANDLE };
    };
    
    class DescriptorSetLayoutVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::DescriptorSetLayoutDeviceObject& object) override
        {
            descriptorSetLayout = object.descriptorSetLayout;
        }
        
    public:
        VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
    };
    
    class MemoryMapVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const BufferDeviceObject& object) override
        {
            memory = object.memory;
        }
        
        void Visit(const TextureDeviceObject& object) override
        {
            memory = object.memory;
        }
        
    public:
        VkDeviceMemory memory{ VK_NULL_HANDLE };
    };
    
    class TextureVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const TextureDeviceObject& object) override
        {
            image = object.image;
            memory = object.memory;
            imageView = object.imageView;
            sampler = object.sampler;
        }
        
    public:
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        VkImage image{ VK_NULL_HANDLE };
        VkImageView imageView{ VK_NULL_HANDLE };
        VkSampler sampler{ VK_NULL_HANDLE };
    };
    
    class CommandBufferVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::CommandBufferDeviceObject& object) override
        {
            commandBuffer = object.commandBuffer;
        }
        
    public:
        VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };
    };
    
    class FrameSyncVisitor : public DeviceObjectVisitorBase
    {
    public:
        void Visit(const Vulkan::FrameSyncDeviceObject& object) override
        {
            imgAvailableSemaphore = object.imageAvailableSemaphore.Get();
            renderFinishedSemaphore = object.renderFinishedSemaphore.Get();
            frameFence = object.frameFence.Get();
        }
        
    public:
        VkSemaphore imgAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence frameFence;
    };
    
    class DestroyVisitor : public MutableDeviceObjectVisitorBase
    {
    public:
        explicit DestroyVisitor(std::shared_ptr<PAL::RenderAPI::VulkanDevice> device)
            : mDevice(std::move(device))
        {}
        
        void Visit(BufferDeviceObject& object) override
        {
            _ASSERT(object.buffer != VK_NULL_HANDLE);
            _ASSERT(object.memory != VK_NULL_HANDLE);
            
            if(object.mappedMemory)
            {
                mDevice->UnmapMemory(object.memory);
            }
            
            mDevice->DestroyBuffer(object.buffer, nullptr);
            mDevice->FreeMemory(object.memory, nullptr);
            
            object.buffer = VK_NULL_HANDLE;
            object.memory = VK_NULL_HANDLE;
            object.mappedMemory = nullptr;
        }
        
        void Visit(VulkanAttachmentDeviceObject& object) override
        {
            _ASSERT(object.image != VK_NULL_HANDLE);
            _ASSERT(object.view != VK_NULL_HANDLE);
            _ASSERT(object.memory != VK_NULL_HANDLE);
            
            mDevice->DestroyImage(object.image, nullptr);
            mDevice->DestroyImageView(object.view, nullptr);
            mDevice->FreeMemory(object.memory, nullptr);
            
            object.image = VK_NULL_HANDLE;
            object.view = VK_NULL_HANDLE;
            object.memory = VK_NULL_HANDLE;
        }
        
        void Visit(Vulkan::SwapChainDeviceObject& object) override
        {
            auto& swapChainHandle = object.swapChain.Get();
            
            _ASSERT(swapChainHandle != VK_NULL_HANDLE);
            
            mDevice->DestroySwapchainKHR(swapChainHandle, nullptr);
            swapChainHandle = VK_NULL_HANDLE;
        }
        
    private:
        std::shared_ptr<PAL::RenderAPI::VulkanDevice> mDevice;
    };
}
