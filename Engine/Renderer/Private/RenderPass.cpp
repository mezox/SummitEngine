#include <Renderer/RenderPass.h>
#include <Renderer/Renderer.h>
#include <Renderer/Resources/Synchronization.h>

using namespace Renderer;
using namespace Renderer::Vulkan;

bool RenderPass::Create(IRenderer& renderer)
{
    renderer.CreateRenderPass({}, mDeviceResource);
    
    return true;
}
