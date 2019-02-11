#pragma once

#include <Math/MathBase.h>

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
    
public:
    T x{ 0 };
    T y{ 0 };
    T z{ 0 };
    T w{ 0 };
};

using Vector4f = Vector4<float>;
