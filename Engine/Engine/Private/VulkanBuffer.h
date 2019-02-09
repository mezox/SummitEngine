#pragma once

#include <Engine/Renderer.h>
#include <PAL/RenderAPI/VulkanAPI.h>

namespace Renderer
{
    class VulkanBuffer : public RendererResource
    {
    public:
        
    public:
        VkBuffer handle{ VK_NULL_HANDLE };
        VkDeviceMemory memory{ VK_NULL_HANDLE };
        uint32_t verticesCnt{ 0 };
    };
}
