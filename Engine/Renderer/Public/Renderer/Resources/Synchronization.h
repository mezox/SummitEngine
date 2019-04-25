#pragma once

#include "Framebuffer.h"
#include <Renderer/Image.h>

namespace Renderer
{
    struct SemaphoreDescriptor
    {
        
    };
    
    struct FenceDescriptor
    {
        bool signaled{ false };
    };
    
    struct AttachmentDesc
    {
        AttachmentType type;
        Format format;
        ImageLayout layout;
    };
    
    // TODO: Move elesewhere
    struct RenderPassDescriptor
    {
        std::vector<AttachmentDesc> attachments;
    };
}
