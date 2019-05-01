#include <Renderer/RenderPass.h>

using namespace Renderer;
using namespace Renderer::Vulkan;

void Subpass::AddAttachmentRef(const AttachmentType type, const AttachmentId attachmentId, const ImageLayout layout) noexcept
{
    mAttachmentRefs.push_back({ attachmentId, type, layout });
}

std::vector<const AttachmentRef*> Subpass::GetAttachments(AttachmentType type) const noexcept
{
    std::vector<const AttachmentRef*> refs;
    for(auto& ref : mAttachmentRefs)
    {
        if(ref.attachmentType == type)
        {
            refs.push_back(&ref);
        }
    }
    return refs;
}

AttachmentId RenderPass::AddAttachment(const AttachmentDesc& desc) noexcept
{
    mAttachments.push_back(desc);
    return mAttachments.size() - 1;
}

void RenderPass::SetActiveFramebuffer(const Renderer::Framebuffer& framebuffer) noexcept
{
    mActiveFramebuffer = &framebuffer;
}

void RenderPass::AddSubpass(Subpass&& subpass) noexcept
{
    mSubPasses.push_back(std::move(subpass));
}

Subpass& RenderPass::CreateSubpass() noexcept
{
    return mSubPasses.emplace_back();
}

void RenderPass::SetDependency(const DependencyDesc& desc) noexcept
{
    mDependencies.push_back(desc);
}

const Renderer::Framebuffer* RenderPass::GetActiveFramebuffer() const noexcept
{
    return mActiveFramebuffer;
}
