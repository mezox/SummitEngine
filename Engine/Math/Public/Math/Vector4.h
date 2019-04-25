#pragma once

#include <Math/MathBase.h>
#include <Math/Vector3.h>
#include <cmath>

template<typename T>
class Vector4
{
public:
    constexpr Vector4() = default;
    
    constexpr explicit Vector4(T value)
    : x(value), y(value), z(value), w(value)
    {}
    
    constexpr Vector4(T x_, T y_, T z_, T w_)
    : x(x_), y(y_), z(z_), w(w_)
    {}
    
    void Normalize()
    {
        const float lengthSquared = x * x + y * y + z * z;
        
        if (lengthSquared == 0.0f)
            return;
        
        float inv = 1.0f / std::sqrt(lengthSquared);
        x *= inv;
        y *= inv;
        z *= inv;
    }
    
    const Vector4<T> GetNormalized() const
    {
        Vector4<T> result(*this);
        result.Normalize();
        return result;
    }
    
    Vector3<T> GetXYZ() const
    {
        return Vector3<T>(x, y, z);
    }
    
public:
    T x{ 0 };
    T y{ 0 };
    T z{ 0 };
    T w{ 0 };
};

using Vector4f = Vector4<float>;
