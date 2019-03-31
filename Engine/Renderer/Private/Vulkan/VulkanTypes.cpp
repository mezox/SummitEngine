#include "VulkanTypes.h"

#include <Renderer/VertexBuffer.h>
#include <Renderer/Image.h>
#include <Renderer/Effect.h>
#include <Renderer/SharedDeviceTypes.h>

template<>
auto TypeLinkerTempl<Renderer::BufferUsage, VkBufferUsageFlagBits>::operator()(const from_t& bufferUsage) -> to_t
{
    switch(bufferUsage)
    {
        case Renderer::BufferUsage::Undefined: throw std::runtime_error("Undefined buffer usage");
        case Renderer::BufferUsage::VertexBuffer: return to_t{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT };
        case Renderer::BufferUsage::UniformBuffer: return to_t{ VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT };
        case Renderer::BufferUsage::IndexBuffer: return to_t{ VK_BUFFER_USAGE_INDEX_BUFFER_BIT };
    }
}

template<>
auto TypeLinkerTempl<Renderer::ImageType, VkImageType>::operator()(const from_t& imageType) -> to_t
{
    switch(imageType)
    {
        case Renderer::ImageType::Image1D: return to_t{ VK_IMAGE_TYPE_1D };
        case Renderer::ImageType::Image2D: return to_t{ VK_IMAGE_TYPE_2D };
        case Renderer::ImageType::Image3D: return to_t{ VK_IMAGE_TYPE_3D };
    }
}

template<>
auto TypeLinkerTempl<Renderer::ImageUsage, VkImageUsageFlagBits>::operator()(const from_t& imageUsage) -> to_t
{
    switch(imageUsage)
    {
        case Renderer::ImageUsage::Undefined: throw std::runtime_error("Undefined image usage");
        case Renderer::ImageUsage::ColorAttachment: return to_t{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT };
        case Renderer::ImageUsage::DepthStencilAttachment: return to_t{ VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT };
        case Renderer::ImageUsage::Sampled: return to_t{ VK_IMAGE_USAGE_SAMPLED_BIT };
    }
}

template<>
auto TypeLinkerTempl<Renderer::Format, VkFormat>::operator()(const from_t& imageType) -> to_t
{
    switch(imageType)
    {
        case Renderer::Format::Undefined: return to_t{ VK_FORMAT_UNDEFINED };
        case Renderer::Format::R8G8B8A8: return to_t{ VK_FORMAT_R8G8B8A8_UNORM };
        case Renderer::Format::R32G32F: return to_t{ VK_FORMAT_R32G32_SFLOAT };
        case Renderer::Format::R32G32B32F: return to_t{ VK_FORMAT_R32G32B32_SFLOAT };
        case Renderer::Format::R32G32B32A32F: return to_t{ VK_FORMAT_R32G32B32A32_SFLOAT };
        case Renderer::Format::D32F: return to_t{ VK_FORMAT_D32_SFLOAT };
        case Renderer::Format::D32FS8F: return to_t{ VK_FORMAT_D32_SFLOAT_S8_UINT };
        case Renderer::Format::D24S8: return to_t{ VK_FORMAT_D24_UNORM_S8_UINT };
    }
}

template<>
auto TypeLinkerTempl<Renderer::AddressMode, VkSamplerAddressMode>::operator()(const from_t& mode) -> to_t
{
    switch(mode)
    {
        case Renderer::AddressMode::Repeat: return to_t{ VK_SAMPLER_ADDRESS_MODE_REPEAT };
        case Renderer::AddressMode::MirroredRepeat: return to_t{ VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT };
        case Renderer::AddressMode::ClampToEdge: return to_t{ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
        case Renderer::AddressMode::MirrorClampToEdge: return to_t{ VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE };
        case Renderer::AddressMode::ClampToBorder: return to_t{ VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER };
    }
}

template<>
auto TypeLinkerTempl<Renderer::FilterMode, VkFilter>::operator()(const from_t& mode) -> to_t
{
    switch(mode)
    {
        case Renderer::FilterMode::Linear: return to_t{ VK_FILTER_LINEAR };
        case Renderer::FilterMode::Nearest: return to_t{ VK_FILTER_NEAREST };
    }
}

template<>
auto TypeLinkerTempl<Renderer::MemoryType, VkMemoryPropertyFlags>::operator()(const from_t& memType) -> to_t
{
    VkMemoryPropertyFlags flags{ 0 };
    
    if(Renderer::MemoryType::Undefined & memType)
        throw std::runtime_error("Undefined memory property");
    if(Renderer::MemoryType::DeviceLocal & memType)
        flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if(Renderer::MemoryType::HostVisible & memType)
        flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    if(Renderer::MemoryType::HostCoherent & memType)
        flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            
    return flags;
}

template<>
auto TypeLinkerTempl<Renderer::ModuleStage, VkShaderStageFlagBits>::operator()(const from_t& stage) -> to_t
{
    switch(stage)
    {
        case Renderer::ModuleStage::Undefined: throw std::runtime_error("Undefined shader stage");
        case Renderer::ModuleStage::Vertex: return to_t{ VK_SHADER_STAGE_VERTEX_BIT };
        case Renderer::ModuleStage::Fragment: return to_t{ VK_SHADER_STAGE_FRAGMENT_BIT };
    }
}

// Descriptor types
template<>
auto TypeLinkerTempl<Renderer::UniformType, VkDescriptorType>::operator()(const from_t& type) -> to_t
{
    switch(type)
    {
        case Renderer::UniformType::Undefined: throw std::runtime_error("Undefined uniform type");
        case Renderer::UniformType::Buffer: return to_t{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
        case Renderer::UniformType::Sampler: return to_t{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
    }
}
