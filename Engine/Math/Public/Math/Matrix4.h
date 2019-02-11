#pragma once

#include <Math/MathBase.h>
#include <Math/Vector4.h>
#include <Math/Vector3.h>

#include <array>
#include <ostream>

class MATH_API Matrix4
{
public:
    Matrix4();
    void MakeIdentity();
    
    const float operator()(uint16_t x, uint16_t y) const;
    float& operator()(uint16_t x, uint16_t y);
    
    static Matrix4 MakeOrtho(float left, float right, float top, float bottom, float near, float far);
    static Matrix4 MakePerspective(float fovY, float aspect, float near, float far);
    
    static Matrix4 MakeTranslation(const Vector3f& translation);
    static Matrix4 MakeScale(const Vector3f& scale);
    static Matrix4 MakeRotation(const Vector3f& rotation);
    
    const Vector3f& GetTranslation() const;
    
    void Translate(float x, float y, float z);
    void Translate(const Vector3f& translation);
    void TranslateX(float x);
    void TranslateY(float y);
    void TranslateZ(float z);
    void ResetTranslation();
    
    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);
    
    void SetTranslation(float x, float y, float z);
    void SetTranslation(const Vector3f& translation);
    void SetScale(float x, float y, float z);
    void SetScale(const Vector3f& scale);
    
    void Transpose();
    
    Matrix4& operator*=(const Matrix4& other);
    
    MATH_API friend std::ostream& operator<<(std::ostream& s, const Matrix4& m);
    
private:
    struct Data
    {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
    };
    
    union
    {
        Data m;
        std::array<Vector4f, 4> mVectorData;
        std::array<float, 16> mData;
    };
};

