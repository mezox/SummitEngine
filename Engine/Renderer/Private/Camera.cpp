#include <Renderer/Camera.h>

#include <Logging/LoggingService.h>

#include <Math/Math.h>

using namespace Renderer;

Camera::Camera()
{
    mTransform.position = Vector3f(0.0f, 0.0f, -2.0f);
    mTransform.rotation = Vector3f(Math::DegreesToRadians(-180.0f), 0.0f, 0.0f);
    
    mProjectionMatrix = Matrix4::MakePerspective(Math::DegreesToRadians(60.0f), 1280/(float)720, 0.1f, 1000.0f);
    //Matrix4::MakeOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    Update(1280, 720);
}

void Camera::Update(uint32_t w, uint32_t h)
{
    mProjectionMatrix = Matrix4::MakePerspective(Math::DegreesToRadians(60.0f), w/(float)h, 0.1f, 1000.0f);
    
    mViewMatrix.MakeIdentity();
    mViewMatrix.RotateX(mTransform.rotation.x);
    mViewMatrix.RotateY(mTransform.rotation.y);
    mViewMatrix.SetTranslation(mTransform.position);
    
    mRightUnit = mViewMatrix.GetColumn(0).GetNormalized().GetXYZ();
    mUpUnit = mViewMatrix.GetColumn(1).GetNormalized().GetXYZ();
    mForwardUnit = -mViewMatrix.GetColumn(2).GetNormalized().GetXYZ();
    
    mLeftUnit = -mRightUnit;
    mDownUnit = -mUpUnit;
    mBackwardUnit = mBackwardUnit;
}

const Vector3f& Camera::GetForward() const noexcept
{
    return mForwardUnit;
}

const Vector3f& Camera::GetRight() const noexcept
{
    return mRightUnit;
}

const Vector3f& Camera::GetUp() const noexcept
{
    return mUpUnit;
}

const Matrix4& Camera::GetViewMatrix() const noexcept
{
    return mViewMatrix;
}

const Matrix4& Camera::GetProjectionMatrix() const noexcept
{
    return mProjectionMatrix;
}

void Camera::OnViewportChange(uint32_t w, uint32_t h)
{
    mProjectionMatrix = Matrix4::MakePerspective(Math::DegreesToRadians(60.0f), w/(float)h, 0.1f, 1000.0f);
}
