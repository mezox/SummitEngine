#pragma once

#include <PAL/RenderAPI/VulkanAPI.h>

namespace Renderer
{
    namespace Vulkan
    {
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
    }
    
    class VulkanShaderDeviceObject
    {
    public:
        VulkanShaderDeviceObject(const VkShaderModule& m) : module(m) {}
        
    public:
        VkShaderModule module{ VK_NULL_HANDLE };
    };
    
    class VulkanRenderPassDeviceObject
    {
    public:
        VulkanRenderPassDeviceObject(const VkRenderPass& rp) : renderPass(rp) {}
        
    public:
        VkRenderPass renderPass{ VK_NULL_HANDLE };
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
        TextureDeviceObject(const VkImage& img, const VkImageView& view, const VkDeviceMemory& mem, const VkSampler& s) : image(img), imageView(view), memory(mem), sampler(s) {}
        
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
    
    class VulkanSwapChainDeviceObject
    {
    public:
        VulkanSwapChainDeviceObject(const VkSwapchainKHR& sc, const VkSurfaceKHR& surface)
            : swapChain(sc)
            , surface(surface)
        {}
        
    public:
        VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
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
        void Visit(const VulkanSwapChainDeviceObject& object) override {}
        void Visit(const Vulkan::DescriptorSetLayoutDeviceObject& object) override{}
        void Visit(const Vulkan::DescriptorSetDeviceObject& object) override{}
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
        void Visit(const VulkanSwapChainDeviceObject& object) override {}
        void Visit(const Vulkan::DescriptorSetLayoutDeviceObject& object) override{}
        void Visit(const Vulkan::DescriptorSetDeviceObject& object) override {}
        
    public:
        uint16_t buffers{ 0 };
        uint16_t pipelines{ 0 };
        uint16_t images{ 0 };
        uint16_t imageViews{ 0 };
        uint16_t framebuffers{ 0 };
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
        void Visit(const VulkanSwapChainDeviceObject& object) override
        {
            swapChain = object.swapChain;
            surface = object.surface;
        }
        
    public:
        VkSwapchainKHR swapChain{ VK_NULL_HANDLE };
        VkSurfaceKHR surface{ VK_NULL_HANDLE };
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
}
