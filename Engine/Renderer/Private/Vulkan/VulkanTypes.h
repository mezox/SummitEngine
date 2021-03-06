#pragma once

#include <Core/TypeConvert.h>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>
#include <vector>

#include <Renderer/SharedDeviceTypes.h>

namespace Renderer
{
    enum class BufferUsage;
    enum class ImageType;
    enum class ImageUsage : uint32_t;
    enum class ImageLayout;
    enum class Format;
    enum class AddressMode;
    enum class FilterMode;
    enum class UniformType;
    enum class ModuleStage;
    enum MemoryType : uint32_t;
    enum class StageMask : uint32_t;
    enum class AccessMask : uint32_t;
}

// Convertors from Renderer types to Vulkan types
template<> struct TypeLinkerAuto<Renderer::BufferUsage> : public TypeLinkerTempl<Renderer::BufferUsage, VkBufferUsageFlagBits> {};
template<> struct TypeLinkerAuto<Renderer::ImageType> : public TypeLinkerTempl<Renderer::ImageType, VkImageType> {};
template<> struct TypeLinkerAuto<Renderer::ImageUsage> : public TypeLinkerTempl<Renderer::ImageUsage, VkImageUsageFlags> {};
template<> struct TypeLinkerAuto<Renderer::ImageLayout> : public TypeLinkerTempl<Renderer::ImageLayout, VkImageLayout> {};
template<> struct TypeLinkerAuto<Renderer::Format> : public TypeLinkerTempl<Renderer::Format, VkFormat> {};
template<> struct TypeLinkerAuto<Renderer::MemoryType> : public TypeLinkerTempl<Renderer::MemoryType, VkMemoryPropertyFlags> {};
template<> struct TypeLinkerAuto<Renderer::AddressMode> : public TypeLinkerTempl<Renderer::AddressMode, VkSamplerAddressMode> {};
template<> struct TypeLinkerAuto<Renderer::FilterMode> : public TypeLinkerTempl<Renderer::FilterMode, VkFilter> {};
template<> struct TypeLinkerAuto<Renderer::UniformType> : public TypeLinkerTempl<Renderer::UniformType, VkDescriptorType> {};
template<> struct TypeLinkerAuto<Renderer::ModuleStage> : public TypeLinkerTempl<Renderer::ModuleStage, VkShaderStageFlagBits> {};

template<> struct TypeLinkerAuto<Renderer::StageMask> : public TypeLinkerTempl<Renderer::StageMask, VkPipelineStageFlags> {};
template<> struct TypeLinkerAuto<Renderer::AccessMask> : public TypeLinkerTempl<Renderer::AccessMask, VkAccessFlags> {};

template<> struct TypeLinkerAuto<Renderer::Rectangle<float>> : public TypeLinkerTempl<Renderer::Rectangle<float>, VkViewport> {};
template<> struct TypeLinkerAuto<Renderer::Rectangle<uint32_t>> : public TypeLinkerTempl<Renderer::Rectangle<uint32_t>, VkRect2D> {};

namespace Renderer
{
    struct VulkanImageDesc
    {
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        uint32_t depth{ 0 };
        uint32_t mipMapLevels{ 0 };
        VkImageType type{ VK_IMAGE_TYPE_2D };
        VkFormat format{ VK_FORMAT_UNDEFINED };
        VkSharingMode sharingMode{ VK_SHARING_MODE_EXCLUSIVE };
        VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
        VkImageUsageFlags usage{ VK_IMAGE_USAGE_SAMPLED_BIT};
        VkMemoryPropertyFlags memoryProps;
        void* data{ nullptr };
    };
    
    struct VulkanFramebufferDesc
    {
        std::vector<VkImageView> attachments;
        VkRenderPass renderPass{ VK_NULL_HANDLE };
    };
}
