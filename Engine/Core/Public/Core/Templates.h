#pragma once

template<typename T>
struct BitMaskOperatorEnable
{
    static const bool enable = false;
};

template<typename T>
typename std::enable_if<BitMaskOperatorEnable<T>::enable, T>::type
operator |(T lhs, T rhs)
{
    using UT = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
}

template<typename T>
typename std::enable_if<BitMaskOperatorEnable<T>::enable, T>::type
operator &(T lhs, T rhs)
{
    using UT = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(lhs) & static_cast<UT>(rhs));
}