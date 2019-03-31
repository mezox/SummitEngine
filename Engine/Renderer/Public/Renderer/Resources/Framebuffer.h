#pragma once

#include "DeviceResource.h"

#include <Core/Templates.h>
#include <Renderer/RendererBase.h>
#include <Renderer/SharedDeviceTypes.h>
#include <Renderer/DeviceObject.h>

#include <vector>
#include <memory>

namespace Renderer
{
    /*!
     @brief An enumerator of framebuffer attachment types.
     */
    enum class AttachmentType : uint8_t
    {
        Undefined = 0x00000000,
        Color = 0x00000001,
        Stencil = 0x00000002,
        Depth = 0x00000004
    };
    
    /*!
     @brief Framebuffer's attachment.
     */
    class RENDERER_API Attachment final : public DeviceResource
    {
    public:
        /*!
         @brief Constructs framebuffer attachment.
         @param format Format of stored data.
         @param type Type of stored data.
         @param deviceObject Device object handler.
         */
        Attachment(Format format, AttachmentType type, DeviceObject&& deviceObject);
        
        /*!
         @brief Attachment destructor.
         */
        virtual ~Attachment() = default;
        
        Attachment(Attachment&& other) = default;
        Attachment& operator=(Attachment&& other) = default;
        
        /*!
         @brief Returns format of attachment.
         */
        [[nodiscard]] Format GetFormat() const noexcept;
        
        /*!
         @brief Returns type of attachment.
         */
        [[nodiscard]] AttachmentType GetType() const noexcept;
        
    private:
        /*!
         @brief Format of stored data. Undefined by default.
         */
        Format mFormat{ Format::Undefined };
        
        /*!
         @brief Type of attachment. Undefined by default.
         */
        AttachmentType mType{ AttachmentType::Undefined };
    };
    
    class RENDERER_API Framebuffer final : public DeviceResource
    {
    public:
        /*!
         @brief Constructs framebuffer attachment.
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
         @brief Returns width of the framebuffer.
         */
        [[nodiscard]] uint32_t GetWidth() const noexcept;
        
        /*!
         @brief Returns height of the framebuffer.
         */
        [[nodiscard]] uint32_t GetHeight() const noexcept;
        
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

template<>
struct BitMaskOperatorEnable<Renderer::AttachmentType>
{
    static const bool enable = true;
};
