#pragma once

#include "FileSystemBase.h"
#include "FileTypes.h"

#include <string>
#include <memory>
#include <cstdint>
#include <vector>

namespace PAL::FileSystem
{
    class FILESYSTEM_API File
    {
    public:
        File() = default;
        explicit File(const std::string& path);
        virtual ~File();
        
        void Open(EFileAccessMode mode);
        void ReadBytes(uint32_t bytesToRead);
        const std::vector<uint8_t>& Read();
        uint32_t Write(void* data, uint32_t bytesToWrite) const;
        void Close();
        bool IsOpened() const;
        void Flush() const;
        
    private:
        FileHandle mHandle{ nullptr };
		std::string mPath;
        std::unique_ptr<std::vector<uint8_t>> mBuffer;
    };
}
