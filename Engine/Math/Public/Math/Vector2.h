#pragma once

#include <Math/MathBase.h>
#include <Core/TupleHash.h>

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

namespace std
{
    template<>
    struct hash<Vector2f>
    {
        size_t operator()(const Vector2f& key)
        {
            auto t = std::tie(key.x, key.y);
            return std::hash<decltype(t)>()(t);
        }
    };
}
