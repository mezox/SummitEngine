#pragma once

#include <Renderer/RendererBase.h>
#include "Transform.h"
#include "VertexBuffer.h"

namespace Renderer
{
    class RENDERER_API Object3d
    {
    public:
        Object3d() = default;
        
        const VertexBufferBase& GetVertexBuffer() const { return *mVertexBuffer.get(); }
        
    protected:
        Transform mTransform;
        std::unique_ptr<VertexBufferBase> mVertexBuffer;
    };
}
