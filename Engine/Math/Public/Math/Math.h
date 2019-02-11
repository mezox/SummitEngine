#pragma once

#include <cmath>

namespace Math
{
	constexpr float MATH_PI = 3.14159265;
	constexpr float MATH_2PI = 2 * MATH_PI;

	template<typename T>
	constexpr T DegreesToRadians(const T angle) { return static_cast<T>(angle * MATH_PI / 180); }

	template<typename T>
	constexpr T RadiansToDegrees(const T angle) { return static_cast<T>(angle * 180 / MATH_PI); }

	template<typename T>
	T Tan(const T angle) { return std::tan(angle); }
    
    template<typename T>
    T Cos(const T angle) { return std::cos(angle); }
    
    template<typename T>
    T Sin(const T angle) { return std::sin(angle); }
}
