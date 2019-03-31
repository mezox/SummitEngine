#pragma once

#include <type_traits>
#include <cstdint>
#include <stdexcept>

namespace Renderer
{    
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
        R8G8B8A8,
        R32G32F,
        R32G32B32F,
        R32G32B32A32F,
        
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
            case Format::R8G8B8A8: return 4;
            case Format::R32G32F: return 8;
            case Format::R32G32B32F: return 12;
            case Format::R32G32B32A32F: return 16;
                
            case Format::D32F: return 4;
            case Format::D32FS8F: return 5;
            case Format::D24S8: return 4;
        }
    }
}
