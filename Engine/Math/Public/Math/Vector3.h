#pragma once

#include <Math/MathBase.h>

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
    
public:
    T x{ 0 };
    T y{ 0 };
    T z{ 0 };
};

using Vector3f = Vector3<float>;
