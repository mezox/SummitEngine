#pragma once

#include <Math/MathBase.h>

template<typename T>
class Vector2
{
public:
    constexpr Vector2() = default;
    
    constexpr explicit Vector2(T value)
        : x(value), y(value)
    {}
    
    constexpr Vector2(T x_, T y_)
        : x(x_), y(y_)
    {}
    
public:
    T x{ 0 };
    T y{ 0 };
};

using Vector2f = Vector2<float>;
