#pragma once

#include <exception>

namespace PAL::FileSystem
{
    using FileHandle = void*;
    
    enum class EFileAccessMode
    {
        Read,
        Write,
        Append
    };
    
    class FileException : public std::exception
    {
    public:
        virtual const char* what() const throw()
        {
            return "FileError";
        }
    };
}
