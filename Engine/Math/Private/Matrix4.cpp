#include <Math/Math.h>
#include <Math/Matrix4.h>

#include <algorithm>
#include <iomanip>

//using namespace Math;

Matrix4::Matrix4()
{
    std::fill_n(mData.begin(), 16, 0.0f);
}

void Matrix4::MakeIdentity()
{
    std::fill_n(mData.begin(), 16, 0.0f);
    mData[0] = 1.0f;
    mData[5] = 1.0f;
    mData[10] = 1.0f;
    mData[15] = 1.0f;
}

const float Matrix4::operator()(const uint16_t x, const uint16_t y) const
{
    //TODO: add ASSERT();
    return mData[(x - 1) * 4 + (y - 1)];
}

float& Matrix4::operator()(const uint16_t x, const uint16_t y)
{
    return mData[(x - 1) * 4 + (y - 1)];
}

Matrix4 Matrix4::MakeOrtho(float left, float right, float top, float bottom, float near, float far)
{
    Matrix4 m;
    m.MakeIdentity();
    
    m(1,1) = 2.0f / (right - left);
    m(2,2) = 2.0f / (bottom - top);
    m(3,3) = -1.0f / (far - near);
    m(4,1) = -(right + left) / (right - left);
    m(4,2) = -(bottom + top) / (bottom - top);
    m(4,3) = -near / (far - near);
    
    return m;
}

Matrix4 Matrix4::MakePerspective(float fovY, float aspect, float near, float far)
{
    /* Compute x',y',z' as x' = x, y' = y, z' = -z
     * and also w' = -z so we can do perspective division & convert
     * homogenous coords back to cartesian
     *
     * 1  0  0  0
     * 0  1  0  0
     * 0  0 -1 -1
     * 0  0  0  0
     *
     * 2.   Normalize z' between 0 & 1,
     *      z′=x∗m20+y∗m21+z∗m22+1∗m23,
     *
     *      therefore:  m22 = −f/(f−n), if near = z, then it's 0
     *                  m32 = −f∗n/(f−n), if far = z, then it's 1
     *
     */
    Matrix4 m;
    m.MakeIdentity();
    
    const float tanHalfFovY = Math::Tan(fovY * 0.5f);
    
    m(1,1) = 1.0f / (tanHalfFovY * aspect);
    m(2,2) = 1.0f / tanHalfFovY;
    m(3,3) = far / (near - far);
    m(3,4) = -1.0f;
    m(4,3) = -(far * near) / (far - near);
    
    return m;
}

Matrix4 Matrix4::MakeTranslation(const Vector3f& translation)
{
    Matrix4 m;
    m.MakeIdentity();
    m.SetTranslation(translation);
    return m;
}

Matrix4 Matrix4::MakeScale(const Vector3f& scale)
{
    Matrix4 m;
    m.MakeIdentity();
    m.SetScale(scale);
    return m;
}

Matrix4 Matrix4::MakeRotation(const Vector3f& rotation)
{
    Matrix4 rotationX;
    rotationX.MakeIdentity();
    
    return rotationX;
}

const Vector3f& Matrix4::GetTranslation() const
{
    return *(Vector3f*)(&m._41);
}

void Matrix4::Translate(float x, float y, float z)
{
    m._41 += x;
    m._42 += y;
    m._43 += z;
}

void Matrix4::Translate(const Vector3f& translation)
{
    m._41 += translation.x;
    m._42 += translation.y;
    m._43 += translation.z;
}

void Matrix4::TranslateX(float x)
{
    m._41 += x;
}

void Matrix4::TranslateY(float y)
{
    m._42 += y;
}

void Matrix4::TranslateZ(float z)
{
    m._43 += z;
}

void Matrix4::ResetTranslation()
{
    SetTranslation(0.0f, 0.0f, 0.0f);
}

void Matrix4::RotateX(float angle)
{
    Matrix4 m;
    m.MakeIdentity();
    
    const float cosAngle = Math::Cos(angle);
    const float sinAngle = Math::Sin(angle);
    
    m(2,2) = cosAngle;
    m(2,3) = sinAngle;
    m(3,2) = -sinAngle;
    m(3,3) = cosAngle;
    
    *this *= m;
}

void Matrix4::RotateY(float angle)
{
    Matrix4 m;
    m.MakeIdentity();
    
    const float cosAngle = Math::Cos(angle);
    const float sinAngle = Math::Sin(angle);
    
    m(1,1) = cosAngle;
    m(1,3) = sinAngle;
    m(3,1) = -sinAngle;
    m(3,3) = cosAngle;
    
    *this *= m;
}

void Matrix4::RotateZ(float angle)
{
    Matrix4 m;
    m.MakeIdentity();
    
    const float cosAngle = Math::Cos(angle);
    const float sinAngle = Math::Sin(angle);
    
    m(1,1) = cosAngle;
    m(1,2) = sinAngle;
    m(2,1) = -sinAngle;
    m(2,2) = cosAngle;
    
    *this *= m;
}

void Matrix4::SetTranslation(float x, float y, float z)
{
    m._41 = x;
    m._42 = y;
    m._43 = z;
}

void Matrix4::SetTranslation(const Vector3f& translation)
{
    m._41 = translation.x;
    m._42 = translation.y;
    m._43 = translation.z;
}

void Matrix4::SetScale(float x, float y, float z)
{
    m._11 = x;
    m._22 = y;
    m._33 = z;
}

void Matrix4::SetScale(const Vector3f& translation)
{
    m._11 = translation.x;
    m._22 = translation.y;
    m._33 = translation.z;
}

Vector4f Matrix4::GetColumn(uint8_t idx) const noexcept
{
    return *(Vector4f*)&mData[idx * 4];
}

void Matrix4::Transpose()
{
    std::swap(mData[1], mData[4]);
    std::swap(mData[2], mData[8]);
    std::swap(mData[3], mData[12]);
    std::swap(mData[6], mData[9]);
    std::swap(mData[7], mData[13]);
    std::swap(mData[11], mData[14]);
}

Matrix4& Matrix4::operator*=(const Matrix4& other)
{
    Matrix4 result;
    
    result.m._11 = m._11 * other.m._11 + m._12 * other.m._21 + m._13 * other.m._31 + m._14 * other.m._41;
    result.m._12 = m._11 * other.m._12 + m._12 * other.m._22 + m._13 * other.m._32 + m._14 * other.m._42;
    result.m._13 = m._11 * other.m._13 + m._12 * other.m._23 + m._13 * other.m._33 + m._14 * other.m._43;
    result.m._14 = m._11 * other.m._14 + m._12 * other.m._24 + m._13 * other.m._34 + m._14 * other.m._44;
    
    result.m._21 = m._21 * other.m._11 + m._22 * other.m._21 + m._23 * other.m._31 + m._24 * other.m._41;
    result.m._22 = m._21 * other.m._12 + m._22 * other.m._22 + m._23 * other.m._32 + m._24 * other.m._42;
    result.m._23 = m._21 * other.m._13 + m._22 * other.m._23 + m._23 * other.m._33 + m._24 * other.m._43;
    result.m._24 = m._21 * other.m._14 + m._22 * other.m._24 + m._23 * other.m._34 + m._24 * other.m._44;
    
    result.m._31 = m._31 * other.m._11 + m._32 * other.m._21 + m._33 * other.m._31 + m._34 * other.m._41;
    result.m._32 = m._31 * other.m._12 + m._32 * other.m._22 + m._33 * other.m._32 + m._34 * other.m._42;
    result.m._33 = m._31 * other.m._13 + m._32 * other.m._23 + m._33 * other.m._33 + m._34 * other.m._43;
    result.m._34 = m._31 * other.m._14 + m._32 * other.m._24 + m._33 * other.m._34 + m._34 * other.m._44;
    
    result.m._41 = m._41 * other.m._11 + m._42 * other.m._21 + m._43 * other.m._31 + m._44 * other.m._41;
    result.m._42 = m._41 * other.m._12 + m._42 * other.m._22 + m._43 * other.m._32 + m._44 * other.m._42;
    result.m._43 = m._41 * other.m._13 + m._42 * other.m._23 + m._43 * other.m._33 + m._44 * other.m._43;
    result.m._44 = m._41 * other.m._14 + m._42 * other.m._24 + m._43 * other.m._34 + m._44 * other.m._44;
    
    *this = result;
    return *this;
}

std::ostream& operator<<(std::ostream& s, const Matrix4& m)
{
    for(uint8_t i{ 0 } ; i < 4; ++i)
    {
        for(uint8_t j{ 0 } ; j < 4; ++j)
        {
            s << std::right << std::fixed << std::setw(7) << std::setprecision(3) << m.mData[i * 4 + j];
        }
        
        s << "\n";
    }
    
    return s;
}
