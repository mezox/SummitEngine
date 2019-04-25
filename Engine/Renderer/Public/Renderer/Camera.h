#pragma once

#include <Renderer/RendererBase.h>
#include <Core/Platform.h>

#include <Math/Matrix4.h>
#include "Transform.h"

namespace Renderer
{
    enum class ProjectionType
    {
        Perspective,
        Orthographic
    };
    
    class RENDERER_API Frustum
    {
    public:
        float fieldOfViewX;
        float fieldOfViewY;
        float nearPlane{ 0.0f };
        float farPlane{ 0.0f };
    };
    
    class RENDERER_API Camera
    {
    public:
        DECLARE_NOCOPY_NOMOVE(Camera)
        
    public:
        Camera();
        
        void Update(uint32_t w, uint32_t h);
        
        NO_DISCARD const Vector3f& GetForward() const noexcept;
        NO_DISCARD const Vector3f& GetRight() const noexcept;
        NO_DISCARD const Vector3f& GetUp() const noexcept;
        
        NO_DISCARD const Matrix4& GetOrientation() const noexcept;
        NO_DISCARD const Matrix4& GetViewMatrix() const noexcept;
        NO_DISCARD const Matrix4& GetProjectionMatrix() const noexcept;
        
        Transform mTransform;
        
    private:
        void OnViewportChange(uint32_t w, uint32_t h);
        
    private:
        ProjectionType mProjectionType{ ProjectionType::Perspective};
        Matrix4 mViewMatrix;
        Matrix4 mProjectionMatrix;
        
        Vector3f mForwardUnit;
        Vector3f mRightUnit;
        Vector3f mUpUnit;
        Vector3f mBackwardUnit;
        Vector3f mLeftUnit;
        Vector3f mDownUnit;
    };
}
