#pragma once

#include "RendererBase.h"

#include "Resources/DeviceResource.h"
#include "Resources/Framebuffer.h"
#include "Image.h"

namespace Renderer
{
    class RENDERER_API RenderPass : public DeviceResource
    {
        friend class VulkanRenderer;
        
        struct AttachmentDesc
        {
            AttachmentType type;
            Format format;
            ImageLayout layout;
        };
        
    public:
        RenderPass() = default;
        
        virtual ~RenderPass() = default;
        
        RenderPass(const RenderPass& other) = delete;
        RenderPass(RenderPass&& other) = delete;
        RenderPass& operator=(const RenderPass& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;
        
        void AddAttachment(AttachmentType type, Format format, ImageLayout layout) noexcept;
        void SetActiveFramebuffer(const Renderer::Framebuffer& framebuffer) noexcept;
        
        const Renderer::Framebuffer* GetActiveFramebuffer() const noexcept;
        
    private:
        std::vector<AttachmentDesc> mAttachmentDescs;
        const Renderer::Framebuffer* mActiveFramebuffer{ nullptr };
    };
}
