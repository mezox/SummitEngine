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
}
