#pragma once

#include <Renderer/RendererBase.h>
#include <Renderer/DeviceObject.h>

namespace Renderer
{
    class RENDERER_API Buffer
    {
    public:
        Buffer() = default;
        
    public:
        uint32_t offset{ 0 };
        uint32_t dataSize{ 0 };
        DeviceObject deviceObject;
    };
}
