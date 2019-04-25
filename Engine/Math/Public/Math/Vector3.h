#pragma once

#include <Math/MathBase.h>
#include <Core/TupleHash.h>

template<typename T>
class Vector3
{
public:
    constexpr Vector3() = default;
    
    constexpr explicit Vector3(T value)
        : x(value), y(value), z(value)
    {}
    
    constexpr Vector3(T x_, T y_, T z_)
        : x(x_), y(y_), z(z_)
    {}
    
    Vector3<T> operator*(const float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    
    Vector3<T> operator-()
    {
        x *= -1.0f;
        y *= -1.0f;
        z *= -1.0f;
        return *this;
    }
    
public:
    T x{ 0 };
    T y{ 0 };
    T z{ 0 };
};

using Vector3f = Vector3<float>;

namespace std
{
    template<>
    struct hash<Vector3f>
    {
        size_t operator()(const Vector3f& key)
        {
            auto t = std::tie(key.x, key.y, key.z);
            return std::hash<decltype(t)>()(t);
        }
    };
}
