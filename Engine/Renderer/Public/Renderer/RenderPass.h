#pragma once

#include "RendererBase.h"

#include "Resources/DeviceResource.h"
#include "Resources/Framebuffer.h"

#include <Event/Signal.h>
#include <initializer_list>

namespace Renderer
{
    using AttachmentId = uint32_t;
    
    struct AttachmentDesc
    {
        Format format;
        ImageLayout initialLayout{ ImageLayout::Undefined };
        ImageLayout finalLayout{ ImageLayout::Undefined };
    };
    
    struct DependencyDesc
    {
        uint32_t srcIdx{ 0 };
        uint32_t dstIdx{ 0 };
        StageMask srcStageMask{ StageMask::Undefined };
        StageMask dstStageMask{ StageMask::Undefined };
        AccessMask srcAccessMask{ AccessMask::Undefined };
        AccessMask dstAccessMask{ AccessMask::Undefined };
    };
    
    struct AttachmentRef
    {
        AttachmentId attachmentId;
        AttachmentType attachmentType;
        ImageLayout attachmentLayout;
    };
    
    /*!
     @brief A subpass represents a phase of rendering that reads and writes subset
     of the attachments from in a render pass
     */
    class RENDERER_API Subpass
    {
    public:
        Subpass() = default;
        virtual ~Subpass() = default;
        
        Subpass(const Subpass& other) = delete;
        Subpass& operator=(const Subpass& other) = delete;
        Subpass(Subpass&& other) = default;
        Subpass& operator=(Subpass&& other) = delete;
        
        void AddAttachmentRef(AttachmentType type, AttachmentId attachmentId, ImageLayout layout) noexcept;
        std::vector<const AttachmentRef*> GetAttachments(AttachmentType type) const noexcept;
        
    private:
        std::string mName;
        std::vector<AttachmentRef> mAttachmentRefs;
    };
    
    /*!
     @brief A render pass represents a collection of attachments, subpasses
            and dependencies between the subpasses and describes how the attachments
            are used over the course of subpasses.
     */
    class RENDERER_API RenderPass : public DeviceResource
    {
        friend class VulkanRenderer;
        
    public:
        RenderPass() = default;
        virtual ~RenderPass() = default;
        
        RenderPass(const RenderPass& other) = delete;
        RenderPass(RenderPass&& other) = delete;
        RenderPass& operator=(const RenderPass& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;
        
        void SetActiveFramebuffer(const Renderer::Framebuffer& framebuffer) noexcept;
        
        AttachmentId AddAttachment(const AttachmentDesc& attachment) noexcept;
        
        Subpass& CreateSubpass() noexcept;
        void AddSubpass(Subpass&& subpass) noexcept;
        void SetDependency(const DependencyDesc& dependency) noexcept;
        
        const Renderer::Framebuffer* GetActiveFramebuffer() const noexcept;
        
    public:
        sigslot::signal<> EarlyBeginEmitter;
        sigslot::signal<> BeginEmitter;
        
    private:
        const Renderer::Framebuffer* mActiveFramebuffer{ nullptr };
        std::vector<AttachmentDesc> mAttachments;
        std::vector<Subpass> mSubPasses;
        std::vector<DependencyDesc> mDependencies;
    };
}
