#include <Renderer/Resources/Framebuffer.h>

using namespace Renderer;

Attachment::Attachment(const Format format, const AttachmentType type, DeviceObject&& resource)
    : DeviceResource(std::move(resource))
    , mFormat(format)
    , mType(type)
{}

Format Attachment::GetFormat() const noexcept
{
    return mFormat;
}

AttachmentType Attachment::GetType() const noexcept
{
    return mType;
}

Framebuffer::Framebuffer(const uint32_t width, const uint32_t height, DeviceObject&& resource)
    : DeviceResource(std::move(resource))
    , mWidth(width)
    , mHeight(height)
{}

void Framebuffer::AddAttachment(std::shared_ptr<Attachment> attachment) noexcept
{
    mAttachments.push_back(std::move(attachment));
}

uint32_t Framebuffer::GetWidth() const noexcept
{
    return mWidth;
}

uint32_t Framebuffer::GetHeight() const noexcept
{
    return mHeight;
}
