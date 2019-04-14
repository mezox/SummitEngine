#include <Renderer/SwapChain.h>
#include <Renderer/Renderer.h>

using namespace Renderer;

SwapChainBase::SwapChainBase(DeviceObject&& deviceObject)
    :  DeviceResource(std::move(deviceObject))
{}

SwapChainBase::~SwapChainBase()
{
    auto& renderer = RendererLocator::GetRenderer();
    
    // Destroy shared depth attachment
    if(mDepthAttachment)
    {
        auto& deviceDepthAttachment = mDepthAttachment->GetDeviceObject();
        renderer.DestroyDeviceObject(deviceDepthAttachment);
    }
    
    for(auto& framebuffer : mFramebuffers)
    {
        // Destroy framebuffer attachments
//        for(auto& attachment : framebuffer.mAttachments)
//        {
//            if(attachment)
//            {
//                auto& deviceAttachment = attachment->GetDeviceObject();
//                renderer.DestroyDeviceObject(deviceAttachment);
//            }
//        }
        
        // Destroy framebuffer
        auto& deviceFramebuffer = framebuffer.GetDeviceObject();
        renderer.DestroyDeviceObject(deviceFramebuffer);
    }
}

void SwapChainBase::AddFramebuffer(Framebuffer&& fb)
{
    mFramebuffers.push_back(std::move(fb));
}

void SwapChainBase::SetDepthAttachment(std::shared_ptr<Attachment> depth)
{
    mDepthAttachment = std::move(depth);
}

const Framebuffer& SwapChainBase::GetActiveFramebuffer() const
{
    return mFramebuffers[mAcquiredImageIndex];
}
