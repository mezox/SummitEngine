#pragma once

#include "RendererBase.h"
#include "Resources/DeviceResource.h"

namespace Renderer
{
    class IRenderer;
    
    struct RenderPassDescriptor;
    
    class RENDERER_API RenderPass : public DeviceResource
    {
    public:
        RenderPass() = default;
        
        virtual ~RenderPass() = default;
        
        RenderPass(const RenderPass& other) = delete;
        RenderPass(RenderPass&& other) = delete;
        RenderPass& operator=(const RenderPass& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;
        
        bool Create(const RenderPassDescriptor& desc, IRenderer& renderer);
    };
}
