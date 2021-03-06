#pragma once

#include <Math/Vector2.h>
#include <Core/Templates.h>

#include <type_traits>
#include <cstdint>
#include <stdexcept>

namespace Graphics
{
    class Color
    {
    public:
        constexpr Color() = default;
        constexpr explicit Color(const uint32_t color) : mColor(color) {}
        constexpr explicit Color(const float value) : mColor(static_cast<uint32_t>(value)) {}
        
        constexpr Color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
        {
            mColor = ((r | static_cast<uint16_t>(g) << 8)) | ((static_cast<uint32_t>(b)) << 16) | ((static_cast<uint32_t>(a) << 24));
        }
        
        template<typename T = uint8_t>
        constexpr T R() const { return static_cast<T>(static_cast<uint8_t>(mColor)); }
        
        template<typename T = uint8_t>
        constexpr T G() const { return static_cast<T>(static_cast<uint16_t>(mColor) >> 8); }
        
        template<typename T = uint8_t>
        constexpr T B() const { return static_cast<T>(static_cast<uint8_t>(static_cast<uint16_t>(mColor >> 16))); }
        
        template<typename T = uint8_t>
        constexpr T A() const { return static_cast<T>(static_cast<uint8_t>(mColor >> 24)); }
        
        constexpr Color RGB() const { return Color(mColor & 0x00FFFFFF); }
        constexpr Color RG() const { return Color(mColor & 0x0000FFFF); }
        
        template<typename T = uint32_t>
        constexpr uint32_t Get() const { return static_cast<T>(mColor); }
        
    private:
        uint32_t mColor{ 0 };
    };
    
    static constexpr Color ColorBlack = Color(0, 0, 0, 0);
    static constexpr Color ClearValueDepthStencil = Color(255, 0, 0, 0);
}

namespace Renderer
{
    using SummitBufferId = struct { uint32_t id{ 0 }; };
    using SummitImageId = struct { uint32_t id{ 0 }; };
    
    template<typename T>
    struct Rectangle
    {
        constexpr Rectangle() = default;
        constexpr Rectangle(const T w, const T h) : width(w), height(h) {}
        constexpr Rectangle(const Vector2<T>& vec) : width(vec.x), height(vec.y) {}
        constexpr Rectangle(const T w, const T h, const T x, const T y) : width(w), height(h), offset(x, y) {}
        constexpr Rectangle(const Vector2<T>& size, const Vector2<T>& offset) : width(size.x), height(size.y), offset(offset) {}
        
        T width{ 0 };
        T height{ 0 };
        Vector2<T> offset{ 0, 0 };
    };
    
    enum class ImageUsage : uint32_t
    {
        Undefined = 0x00000000,
        Sampled = 0x00000001,
        ColorAttachment = 0x00000010,
        DepthStencilAttachment = 0x00000100
    };
    
    enum class AccessMask : uint32_t
    {
        Undefined           = 0x00000000,
        DepthStencilRead    = 0x00000001,
        DepthStencilWrite   = 0x00000002,
        ColorRead           = 0x00000004,
        ColorWrite          = 0x00000008,
        InputRead           = 0x00000010,
        MemoryRead          = 0x00000020,
        ShaderRead          = 0x00000040
    };
    
    enum class StageMask : uint32_t
    {
        Undefined           = 0x00000000,
        EarlyFragmentTest   = 0x00000001,
        LateFragmentTest    = 0x00000002,
        ColorAttachment     = 0x00000004,
        FragmentShader      = 0x00000008,
        BottomOfPipe        = 0x00000010,
    };
    
    enum class ImageLayout
    {
        Undefined,
        Present,
        ColorAttachment,
        DepthAttachment,
        ShaderReadOnly,
        DepthStencilReadOnly,
    };
    
    enum MemoryType : uint32_t
    {
        Undefined = 0x00000000,
        DeviceLocal = 0x00000001,
        HostVisible = 0x00000002,
        HostCoherent = 0x00000004
    };
    
    enum class Format
    {
        Undefined,
        R8,
        R8G8B8A8,
        R32G32F,
        R32G32B32F,
        R32G32B32A32F,
        
        B8G8R8A8,
        
        // Depth buffer formats
        D32F,
        D32FS8F,
        D24S8
    };
    
    constexpr uint32_t GetSizeFromFormat(const Format f)
    {
        switch (f)
        {
            case Format::Undefined: std::runtime_error("Requested size of undefined format!");
            case Format::R8: return 1;
            case Format::R8G8B8A8: return 4;
            case Format::R32G32F: return 8;
            case Format::R32G32B32F: return 12;
            case Format::R32G32B32A32F: return 16;
                
            case Format::B8G8R8A8: return 4;
                
            case Format::D32F: return 4;
            case Format::D32FS8F: return 5;
            case Format::D24S8: return 4;
        }
        
        return 0;
    }
    
    namespace Detail
    {
        constexpr bool IsDepthFormat(const Format f)
        {
            switch (f)
            {
                case Format::D32F:
                case Format::D32FS8F:
                case Format::D24S8: return true;
                    
                default: return false;
            }
        }
    }
}

template<> struct Bitmask<Renderer::ImageUsage> : std::true_type{};
template<> struct Bitmask<Renderer::AccessMask> : std::true_type{};
template<> struct Bitmask<Renderer::StageMask> : std::true_type{};
