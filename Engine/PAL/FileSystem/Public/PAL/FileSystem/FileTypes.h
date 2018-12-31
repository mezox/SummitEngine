#pragma once

#include <exception>
#include <cstdint>

namespace PAL::FileSystem
{
    using FileHandle = void*;
    
    enum class EFileAccessMode : uint8_t
    {
        Read,
        Write,
        Append
    };

	inline bool operator|(EFileAccessMode a, EFileAccessMode b)
	{
		using T = std::underlying_type_t<EFileAccessMode>;
		return static_cast<bool>(static_cast<T>(a) | static_cast<T>(b));
	}

	inline bool operator&(EFileAccessMode a, EFileAccessMode b)
	{
		using T = std::underlying_type_t<EFileAccessMode>;
		return static_cast<bool>(static_cast<T>(a) & static_cast<T>(b));
	}
    
    class FileException : public std::exception
    {
    public:
        virtual const char* what() const throw()
        {
            return "FileError";
        }
    };
}
