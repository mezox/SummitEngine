#include <Renderer/Resources/Framebuffer.h>

using namespace Renderer;

Attachment::Attachment(const Format format, const AttachmentType type)
    : mFormat(format)
    , mType(type)
{}

Attachment::Attachment(const Format format, const AttachmentType type, DeviceObject&& resource)
    : DeviceResource(std::move(resource))
    , mFormat(format)
    , mType(type)
{}

void Attachment::SetClearValue(const Graphics::Color value)
{
    mClearValue = value;
}

Format Attachment::GetFormat() const noexcept
{
    return mFormat;
}

AttachmentType Attachment::GetType() const noexcept
{
    return mType;
}

Graphics::Color Attachment::GetClearValue() const noexcept
{
    return mClearValue;
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

void Framebuffer::AddAttachment(Format format, AttachmentType type) noexcept
{
    mAttachments.push_back(std::make_shared<Attachment>(format, type));
}

void Framebuffer::Resize(uint32_t width, uint32_t height) noexcept
{
    mWidth = width;
    mHeight = height;
}

bool Framebuffer::HasAttachment(AttachmentType type) const noexcept
{
    const auto result = std::find_if(mAttachments.begin(), mAttachments.end(), [&type](const auto& attachmentPtr){
        return attachmentPtr->GetType() == type;
    });
    
    return result != mAttachments.end();
}

std::vector<Graphics::Color> Framebuffer::GetClearValues() const noexcept
{
    std::vector<Graphics::Color> clearValues;
    clearValues.reserve(mAttachments.size());
    
    for(const auto& attachment : mAttachments)
    {
       clearValues.push_back(attachment->GetClearValue());
    }
    
    return clearValues;
}

uint32_t Framebuffer::GetWidth() const noexcept
{
    return mWidth;
}

uint32_t Framebuffer::GetHeight() const noexcept
{
    return mHeight;
}
