#include <Renderer/Resources/Framebuffer.h>

using namespace Renderer;

namespace
{
    bool IsAttachmentType(const Core::FlagMask<ImageUsage> usage)
    {
        if(usage.IsSet(ImageUsage::ColorAttachment) || usage.IsSet(ImageUsage::DepthStencilAttachment))
            return true;
        
        return false;
    }
    
    constexpr AttachmentType ToAttachmentType(const Core::FlagMask<ImageUsage> usage)
    {
        if(usage.IsSet(ImageUsage::ColorAttachment)) return AttachmentType::Color;
        if(usage.IsSet(ImageUsage::DepthStencilAttachment)) return AttachmentType::DepthStencil;
        
        return AttachmentType::Undefined;
    }
}

Attachable::Attachable(const AttachableDescriptor& desc)
    : mDescriptor(desc)
{}

Attachable::Attachable(const AttachableDescriptor& desc, DeviceObject&& deviceObject)
    : DeviceResource(std::move(deviceObject))
    , mDescriptor(desc)
{}

uint32_t Attachable::GetWidth() const noexcept
{
    return mDescriptor.width;
}

uint32_t Attachable::GetHeight() const noexcept
{
    return mDescriptor.height;
}

Format Attachable::GetFormat() const noexcept
{
    return mDescriptor.format;
}

Core::FlagMask<ImageUsage> Attachable::GetUsage() const noexcept
{
    return mDescriptor.usage;
}

Attachment::Attachment(const AttachableDescriptor& desc, const Graphics::Color clearValue)
    : Attachable(desc)
    , mClearValue(clearValue)
{}

Attachment::Attachment(const AttachableDescriptor& desc, const Graphics::Color clearValue, DeviceObject&& deviceObject)
    : Attachable(desc, std::move(deviceObject))
    , mClearValue(clearValue)
{}

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

void Framebuffer::AddAttachment(const Format format, const ImageUsage usage, const Graphics::Color clearValue) noexcept
{
    AttachableDescriptor desc;
    desc.width = mWidth;
    desc.height = mHeight;
    desc.format = format;
    desc.usage = usage;
    
    //static_assert(desc., <#message#>)
    
    mAttachments.push_back(std::make_shared<Attachment>(desc, clearValue));
}

const std::vector<Attachment*> Framebuffer::GetAttachment(AttachmentType type) const noexcept
{
    std::vector<Attachment*> attachmentPtrs;
    for(const auto& attachment : mAttachments)
    {
        const auto imageUsage = attachment->GetUsage();
        if(ToAttachmentType(imageUsage) == type)
        {
            attachmentPtrs.push_back(attachment.get());
        }
    }
    
    return attachmentPtrs;
}

void Framebuffer::Resize(uint32_t width, uint32_t height) noexcept
{
    mWidth = width;
    mHeight = height;
}

bool Framebuffer::HasAttachment(AttachmentType type) const noexcept
{
    const auto result = std::find_if(mAttachments.begin(), mAttachments.end(), [&type](const auto& attachmentPtr){
        return ToAttachmentType(attachmentPtr->GetUsage()) == type;
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
