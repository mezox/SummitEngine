#pragma once

#include "DeviceResource.h"

#include <Renderer/RendererBase.h>
#include <Renderer/SharedDeviceTypes.h>
#include <Core/FlagMask.h>

#include <vector>
#include <memory>

namespace Renderer
{
    /*!
     @brief Enumerator of attachment types.
     */
    enum class AttachmentType
    {
        Undefined,
        Input,
        Color,
        DepthStencil,
        Resolve,
    };
    
    /*!
     @brief Descriptor of attachable image.
     */
    struct AttachableDescriptor
    {
        /*!
         @brief Width of the attachment. 0 by default.
         */
        uint32_t width{ 0 };
        
        /*!
         @brief Height of the attachment. 0 by default.
         */
        uint32_t height{ 0 };
        
        /*!
         @brief Format of stored data. Undefined by default.
         */
        Format format{ Format::Undefined };
        
        /*!
         @brief Usage of underlying image. Undefined by default.
         */
        Core::FlagMask<ImageUsage> usage;
    };
    
    /*!
     @brief Class representing buffer/image attachable to framebuffer.
     */
    class RENDERER_API Attachable : public DeviceResource
    {
    public:
        /*!
         @brief Constructs attachable buffer.
         @param desc Attachable descriptor.
         */
        Attachable(const AttachableDescriptor& desc);
        
        /*!
         @brief Constructs attachable buffer.
         @param desc Attachable descriptor.
         @param deviceObject Device object handle.
         */
        Attachable(const AttachableDescriptor& desc, DeviceObject&& deviceObject);
        
        /*!
         @brief Attachment destructor.
         */
        virtual ~Attachable() = default;
        
        Attachable(Attachable&& other) = default;
        Attachable& operator=(Attachable&& other) = default;
        
        /*!
         @brief Returns width of the attachment.
         */
        [[nodiscard]] uint32_t GetWidth() const noexcept;
        
        /*!
         @brief Returns height of the attachment.
         */
        [[nodiscard]] uint32_t GetHeight() const noexcept;
        
        /*!
         @brief Returns format of attachment.
         */
        [[nodiscard]] Format GetFormat() const noexcept;
        
        /*!
         @brief Returns usage of underlying image.
         */
        [[nodiscard]] Core::FlagMask<ImageUsage> GetUsage() const noexcept;
        
    private:
        /*!
         @brief Descriptor of attachable image.
         */
        AttachableDescriptor mDescriptor;
    };
    
    /*!
     @brief Framebuffer's attachment.
     */
    class RENDERER_API Attachment : public Attachable
    {
    public:
        /*!
         @brief Constructs framebuffer attachment.
         @param desc Attachable descriptor.
         @param clearValue Clear value.
         */
        Attachment(const AttachableDescriptor& desc, Graphics::Color clearValue);
        
        /*!
         @brief Constructs framebuffer attachment.
         @param desc Attachable descriptor.
         @param clearValue Clear value.
         @param deviceObject Device object handle.
         */
        Attachment(const AttachableDescriptor& desc, Graphics::Color clearValue, DeviceObject&& deviceObject);
        
        /*!
         @brief Attachment destructor.
         */
        virtual ~Attachment() = default;
        
        /*!
         @brief Returns clear value.
         */
        [[nodiscard]] Graphics::Color GetClearValue() const noexcept;
        
    private:
        /*!
         @brief Clear value whole image is cleared to on render pass begin.
         */
        Graphics::Color mClearValue;
        
        // Load operation
        // Store operation
        
        ImageLayout mInitialLayout{ ImageLayout::Undefined };
        ImageLayout mFinalLayout{ ImageLayout::Undefined };
    };
    
    /*!
     @brief Class Framebuffer represents a collection of attachable render buffers.
     */
    class RENDERER_API Framebuffer final : public DeviceResource
    {
        friend class SwapChainBase;
        friend class VulkanRenderer;
        
    public:
        Framebuffer() = default;

        /*!
         @brief Constructs framebuffer.
         @param width Width of framebuffer.
         @param height Height of framebuffer.
         @param deviceObject Device object handler.
         */
        Framebuffer(uint32_t width, uint32_t height, DeviceObject&& deviceObject);
        
        /*!
         @brief Framebuffer destructor.
         */
        virtual ~Framebuffer() = default;
        
        Framebuffer(Framebuffer&& other) = default;
        Framebuffer& operator=(Framebuffer&& other) = default;
        
        /*!
         @brief Attaches buffer to framebuffer.
         */
        void AddAttachment(std::shared_ptr<Attachment> attachment) noexcept;
        
        /*!
         @brief Adds attachment description to the framebuffer.
         @param format Format of stored data.
         @param usage Usage of underlying image.
         */
        void AddAttachment(Format format, ImageUsage usage, Graphics::Color clearValue) noexcept;
        
        /*!
         @brief Retrieves attachments by given type.
         @param AttachmentType Type of the attachment.
         @return Array of pointers to attachments.
         */
        const std::vector<Attachment*> GetAttachment(AttachmentType type) const noexcept;
        
        /*!
         @brief Sets new width & height of the framebuffer.
         */
        void Resize(uint32_t width, uint32_t height) noexcept;
        
        /*!
         @brief Returns width of the attachment.
         */
        [[nodiscard]] uint32_t GetWidth() const noexcept;
        
        /*!
         @brief Returns height of the attachment.
         */
        [[nodiscard]] uint32_t GetHeight() const noexcept;
        
        /*!
         @brief Checks if framebuffer contains attachment given by type.
         @param type Attachment type.
         @return True if it does, false otherwise.
         */
        [[nodiscard]] bool HasAttachment(AttachmentType type) const noexcept;
        
        /*!
         @brief Returns clear values for all attachments in order they are registered.
         @return Array of clear values.
         */
        [[nodiscard]] std::vector<Graphics::Color> GetClearValues() const noexcept;
        
    private:
        /*!
         @brief Width of the framebuffer. 0 by default.
         */
        uint32_t mWidth{ 0 };
        
        /*!
         @brief Height of the framebuffer. 0 by default.
         */
        uint32_t mHeight{ 0 };
        
        /*!
         @brief Array of pointers to attachments.
         */
        std::vector<std::shared_ptr<Attachment>> mAttachments;
    };
}
