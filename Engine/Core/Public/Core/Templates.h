#pragma once

template<typename T>
struct Bitmask : std::false_type{};

template<typename T>
struct BoolWrapper
{
    constexpr BoolWrapper(T t): t(t) { }
    constexpr operator T() const { return t; }
    constexpr explicit operator bool() const
    {
        using UT = typename std::underlying_type<T>::type;
        return static_cast<UT>(t);
    }
    T t;
};

template<typename T>
typename std::enable_if<Bitmask<T>::value, BoolWrapper<T>>::type
operator |(T lhs, T rhs)
{
    using UT = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
}

template<typename T>
typename std::enable_if<Bitmask<T>::value, BoolWrapper<T>>::type
operator &(T lhs, T rhs)
{
    using UT = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(lhs) & static_cast<UT>(rhs));
}

namespace Core
{
//    template<typename T>
//    class Range
//    {
//    public:
//        class Iterator
//        {
//            friend class Range<T>;
//        public:
//            T operator *() const { return mI; }
//            const Iterator &operator ++() { ++mI; return *this; }
//            Iterator operator ++(int) { Iterator copy(*this); ++mI; return copy; }
//            
//            bool operator ==(const Iterator &other) const { return mI == other.mI; }
//            bool operator !=(const Iterator &other) const { return mI != other.mI; }
//            
//        protected:
//            Iterator(T start) : mI(start) { }
//            
//        private:
//            T mI;
//        };
//        
//        Iterator begin() const { return mBegin; }
//        Iterator end() const { return mEnd; }
//        Range(T  begin, T end) : mBegin(begin), mEnd(end)
//        {
//            static_assert(std::is_integral<T>::value, "Integral required.");
//        }
//        
//    private:
//        Iterator mBegin;
//        Iterator mEnd;
//    };
    
    template<typename T>
    class ScopedIncrement
    {
    public:
        explicit ScopedIncrement(T& v)
            : mValue(v)
        {
        }
        
        ScopedIncrement()
        {
            ++mValue;
        }
        
        const T& GetValue() const
        {
            return mValue;
        }
        
    private:
        T& mValue;
    };
}
