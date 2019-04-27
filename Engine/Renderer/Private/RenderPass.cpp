#include <Renderer/RenderPass.h>
#include <Renderer/Renderer.h>
#include <Renderer/Resources/Synchronization.h>

using namespace Renderer;
using namespace Renderer::Vulkan;

//bool RenderPass::Create(const RenderPassDescriptor& desc, IRenderer& renderer)
//{
//    renderer.CreateRenderPass(desc, mDeviceResource);
//
//    return true;
//}

void RenderPass::AddAttachment(const AttachmentType type, const Format format, const ImageLayout layout) noexcept
{
    mAttachmentDescs.push_back({ type, format, layout });
}

void RenderPass::SetActiveFramebuffer(const Renderer::Framebuffer& framebuffer) noexcept
{
    mActiveFramebuffer = &framebuffer;
}

const Renderer::Framebuffer* RenderPass::GetActiveFramebuffer() const noexcept
{
    return mActiveFramebuffer;
}
