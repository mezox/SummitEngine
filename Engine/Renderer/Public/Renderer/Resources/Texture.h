#pragma once

#include <Renderer/RendererBase.h>
#include <Renderer/Resources/Framebuffer.h>

#include <string>
#include <memory>

namespace Renderer
{
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
    
    class RENDERER_API Texture : public Attachable
    {
    public:
        Texture(uint32_t w, uint32_t h, Format format, void* data);
        virtual ~Texture() = default;
        
        Texture(const Texture& other) = delete;
        Texture(Texture&& other) = default;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) = delete;
        
        static Texture CreateFromFile(const std::string& path);
        
    private:
        std::unique_ptr<std::vector<unsigned char>> mData{ nullptr };
    };
}
