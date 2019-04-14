#pragma once
#include <Renderer/RendererBase.h>
#include <Renderer/Resources/DeviceResource.h>

#include "SharedDeviceTypes.h"
#include "DeviceObject.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Renderer
{
    enum class ImageType
    {
        Image1D,
        Image2D,
        Image3D
    };
    
    enum class ImageUsage
    {
        Undefined,
        Sampled,
        ColorAttachment,
        DepthStencilAttachment
    };
    
    class RENDERER_API ImageDesc
    {
    public:
        ImageDesc() = default;
        
        explicit ImageDesc(uint32_t width) noexcept;
        ImageDesc(uint32_t width, uint32_t height) noexcept;
        ImageDesc(uint32_t width, uint32_t height, uint32_t depth) noexcept;
        
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        uint32_t depth{ 0 };
        uint32_t mipMapLevels{ 0 };
        ImageType type{ ImageType::Image2D };
        Format format{ Format::Undefined };
        ImageUsage usage{ ImageUsage::Undefined };
        MemoryType memoryUsage{ MemoryType::Undefined };
        void* data{ nullptr };
    };
    
    class RENDERER_API Image : public DeviceResource
    {
    public:
        Image(uint32_t w, uint32_t h, Format format, void* data);
        
        virtual ~Image() = default;
        
        Image(const Image& other) = delete;
        Image(Image&& other) = default;
        Image& operator=(const Image& other) = delete;
        Image& operator=(Image&& other) = delete;
        
        static Image CreateFromFile(const std::string& path);
        
    private:
        // Image meta data
        uint32_t mWidth{ 0 };
        uint32_t mHeight{ 0 };
        Format mFormat{ Format::Undefined };
        std::unique_ptr<std::vector<unsigned char>> mData{ nullptr };
    };
    
    enum class AddressMode
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        MirrorClampToEdge,
        ClampToBorder
    };
    
    enum class FilterMode
    {
        Linear,
        Nearest
    };
    
    struct SamplerDesc
    {
        FilterMode minFilter{ FilterMode::Linear };
        FilterMode magFilter{ FilterMode::Linear };
        AddressMode uAddressMode{ AddressMode::Repeat };
        AddressMode vAddressMode{ AddressMode::Repeat };
        AddressMode wAddressMode{ AddressMode::Repeat };
        uint8_t anisotropy{ 0 };
    };
}
