#pragma once

#include <cstdint>
#include <type_traits>

namespace Core
{
    namespace Traits
    {
        template<typename EnumType, typename = void>
        struct IsFlagMask : std::false_type
        {};
        
        template<typename EnumType>
        struct IsFlagMask<EnumType, decltype(static_cast<void>(EnumType::FlagMaskSize))> : std::is_enum<EnumType>
        {};
        
        template<typename EnumType>
        constexpr typename std::enable_if_t<IsFlagMask<EnumType>::value, size_t>
            GetSize() noexcept
        {
            return static_cast<size_t>(EnumType::FlagMaskSize);
        }
        
        template<typename EnumType>
        constexpr typename std::enable_if_t<!IsFlagMask<EnumType>::value, size_t>
            GetSize() noexcept
        {
            return static_cast<size_t>(0);
        }
    }

    template<typename EnumType>
    struct FlagMaskTraits : Traits::IsFlagMask<EnumType>
    {
        static constexpr size_t Size() noexcept
        {
            return Traits::GetSize<EnumType>();
        }
    };

    namespace Detail
    {
        template<size_t Size, typename = void>
        struct SmallestIntType
        {
            static_assert(Size != 0u, "Invalid number of bits!");
        };
        
        // Specialization for flag masks with 0 - 8 bits
        template<size_t Size>
        struct SmallestIntType<Size, typename std::enable_if_t<(0 < Size && Size <= 8)>>
        {
            using type = std::uint_least8_t;
        };
        
        // Specialization for flag masks with 9 - 16 bits
        template<size_t Size>
        struct SmallestIntType<Size, typename std::enable_if_t<(8 < Size && Size <= 16)>>
        {
            using type = std::uint_least16_t;
        };
        
        // Specialization for flag masks with 17 - 32 bits
        template<size_t Size>
        struct SmallestIntType<Size, typename std::enable_if_t<(16 < Size && Size <= 32)>>
        {
            using type = std::uint_least32_t;
        };
        
        template<typename EnumType>
        class FlagMaskImpl
        {
        public:
            using FlagMaskTraits = FlagMaskTraits<EnumType>;
            using IntType = typename Detail::SmallestIntType<FlagMaskTraits::Size()>::type;
            
        public:
            explicit constexpr FlagMaskImpl(const EnumType& e)
                : mBits(Mask(e))
            {}
            
            static constexpr FlagMaskImpl MaskAll() noexcept
            {
                return FlagMaskImpl(static_cast<IntType>(((static_cast<IntType>(1) << FlagMaskTraits::Size()) - static_cast<IntType>(1))));
            }
            
            static constexpr FlagMaskImpl MaskNone() noexcept
            {
                return FlagMaskImpl(static_cast<IntType>(0));
            }
            
            constexpr FlagMaskImpl Set(const EnumType& flag) const noexcept
            {
                return FlagMaskImpl(mBits | Mask(flag));
            }
            
            constexpr FlagMaskImpl Reset(const EnumType& flag) const noexcept
            {
                return FlagMaskImpl(mBits | ~Mask(flag));
            }
            
            constexpr FlagMaskImpl BitwiseOr(const FlagMaskImpl& other) const noexcept
            {
                return FlagMaskImpl(mBits | other.mBits);
            }
            
            constexpr FlagMaskImpl BitwiseAnd(const FlagMaskImpl& other) const noexcept
            {
                return FlagMaskImpl(mBits & other.mBits);
            }
            
            constexpr bool IsSet(const EnumType& flag) const noexcept
            {
                return (mBits & Mask(flag)) != 0;
            }
            
            constexpr IntType GetInt() const noexcept
            {
                return mBits;
            }
            
        private:
            explicit constexpr FlagMaskImpl(IntType bits)
                : mBits(bits)
            {}
            
            static constexpr IntType Mask(const EnumType& e)
            {
                return IntType(IntType(1u) << static_cast<size_t>(e));
            }
            
        private:
            IntType mBits;
        };
        
        template <typename T>
        using EnableMask = typename std::enable_if<FlagMaskTraits<T>::value>::type;
        
        template <typename EnumType>
        using FlagMask = FlagMaskImpl<EnumType>;
        
        template <typename EnumType>
        constexpr bool operator==(const FlagMask<EnumType>& a, const FlagMask<EnumType>& b)
        {
            return a.GetInt() == b.GetInt();
        }
        
        template <typename EnumType>
        constexpr bool operator==(const FlagMask<EnumType>& a, const EnumType& b)
        {
            return a == FlagMask<EnumType>(b);
        }
        
        template <typename EnumType>
        constexpr bool operator==(const EnumType& a, const FlagMask<EnumType>& b)
        {
            return FlagMask<EnumType>(a) == b;
        }
        
        template <typename EnumType>
        constexpr FlagMask<EnumType> operator|(const FlagMask<EnumType>& a, const FlagMask<EnumType>& b)
        {
            return a.BitwiseOr(b);
        }
        
        template <typename EnumType>
        constexpr FlagMask<EnumType> operator|(const FlagMask<EnumType>& a, const EnumType& b)
        {
            return a | FlagMask<EnumType>(b);
        }
        
        template <typename EnumType>
        constexpr FlagMask<EnumType> operator|(const EnumType& a, const FlagMask<EnumType>& b)
        {
            return FlagMask<EnumType>(a) | b;
        }
    }

    template<typename EnumType>
    class FlagMask
    {
        static_assert(std::is_enum<EnumType>::value, "Invalid type, requires enum class!");
        static_assert(FlagMaskTraits<EnumType>::value, "Invalid enum for FlagMask");
        
    public:
        constexpr FlagMask() noexcept
            : mFlags(Detail::FlagMaskImpl<EnumType>::MaskNone())
        {}
        
        constexpr FlagMask(const EnumType& flag) noexcept
            : mFlags(Detail::FlagMaskImpl<EnumType>(flag))
        {}
        
        constexpr FlagMask(const Detail::FlagMask<EnumType>& impl) noexcept
            : mFlags(impl)
        {}
        
        FlagMask& operator=(const EnumType& flag) noexcept
        {
            return *this = Detail::FlagMask<EnumType>(flag);
        }
        
        void Reset(const EnumType& flag) noexcept
        {
            mFlags = mFlags.Reset(flag);
        }
        
        void ResetAll() noexcept
        {
            mFlags = mFlags.MaskNone();
        }
        
        /// \returns Whether or not the specified flag is set.
        constexpr bool IsSet(const EnumType& flag) const noexcept
        {
            return mFlags.IsSet(flag);
        }
        
        constexpr bool IsAnySet() const noexcept
        {
            return mFlags.GetInt() != mFlags.MaskNone().GetInt();
        }
        
        constexpr bool AreAllSet() const noexcept
        {
            return mFlags.GetInt() == mFlags.MaskAll().GetInt();
        }
        
        constexpr bool IsNoneSet() const noexcept
        {
            return !IsAnySet();
        }
        
        FlagMask& operator|=(const EnumType& flag) noexcept
        {
            mFlags = mFlags.Set(flag);
            return *this;
        }
        
    private:
        Detail::FlagMaskImpl<EnumType> mFlags;
    };
}

template<typename EnumType, typename = Core::Detail::EnableMask<EnumType>>
constexpr Core::FlagMask<EnumType> operator|(const EnumType& a, const EnumType& b) noexcept
{
    return Core::Detail::FlagMask<EnumType>(a) | b;
}
