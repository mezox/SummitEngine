#pragma once

#include "DeviceObject.h"

namespace Renderer
{
    class RenderPass
    {
        friend class VulkanRenderer;
        
    public:
        RenderPass() = default;
        
        virtual ~RenderPass() = default;
        
        RenderPass(const RenderPass& other) = delete;
        RenderPass(RenderPass&& other) = default;
        RenderPass& operator=(const RenderPass& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;
        
        const auto& GetDeviceObject() const { return mGpuRenderPass; }
        
    private:
        DeviceObject mGpuRenderPass;
    };
}
