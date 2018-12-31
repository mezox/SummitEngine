#pragma once

#include <PAL/FileSystem/FileSystemService.h>

namespace PAL::FileSystem
{        
    class FileSystemServiceImpl : public IFileSystemService
    {
        class FileSystemNative;
        
    public:
        FileSystemServiceImpl();
        ~FileSystemServiceImpl();
        
        void Initialize() override;
        FileHandle FileOpen(const std::filesystem::path& filePath, EFileAccessMode mode) const override;
        void FileClose(FileHandle handle) const override;
        void FileFlush(FileHandle handle) const override;
        
        uint32_t FileRead(FileHandle handle, void* buffer, uint32_t bytesToRead) const override;
        uint32_t FileWrite(FileHandle handle, void* buffer, uint32_t bytesToWrite) const override;
        uint32_t FileGetSize(FileHandle handle) const override;
        void DeInitialize() override;
        
    private:
        std::unique_ptr<FileSystemNative> mNativeFileSystem;
    };
}
