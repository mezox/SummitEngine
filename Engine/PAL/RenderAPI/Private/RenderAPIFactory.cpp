#include <PAL/RenderAPI/RenderAPIService.h>

#include <Vulkan/VulkanAPIImpl.h>
#include <Metal/MetalAPIImpl.h>

namespace PAL::RenderAPI
{
    std::shared_ptr<IRenderAPI> CreateRenderAPI(RenderBackend backend)
    {
        switch (backend)
        {
            case RenderBackend::Metal: return std::make_shared<MetalRenderAPI>();
            case RenderBackend::Vulkan: return std::make_shared<VulkanRenderAPI>();
            default:
                break;
        }
        
        return nullptr;
    }
}
