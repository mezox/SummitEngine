#pragma once

#include "FileSystemBase.h"
#include "FileTypes.h"

#include <memory>
#include <string>
#include <cstdint>

namespace PAL::FileSystem
{        
    class FILESYSTEM_API IFileSystemService
    {
    public:
        virtual ~IFileSystemService() = default;
        
        /**
         * @brief   Initializes platform's file system service.
         */
        virtual void Initialize() = 0;
        
        /**
         * @brief   Deinitializes platform's file system service.
         */
        virtual void DeInitialize() = 0;
        
        /**
         * @brief   Attempts to open file.
         * @param   filePath Path to file.
         * @param   mode File open mode (read/write/append).
         * @return  File handle of opened file or invalid handle if open failed.
         */
        virtual FileHandle FileOpen(const std::string& filePath, EFileAccessMode mode) const = 0;
        
        /**
         * @brief   Reads data from file.
         * @param   handle File handle.
         * @param   buffer Pointer to output buffer.
         * @param   bytesToRead Count of bytes to read.
         * @return  Count of read bytes.
         */
        virtual uint32_t FileRead(FileHandle handle, void* buffer, uint32_t bytesToRead) const = 0;
        
        /**
         * @brief   Writes data to file.
         * @param   handle File handle.
         * @param   buffer Pointer to data buffer.
         * @param   bytesToWrite Count of bytes to write.
         * @return  Count of read bytes.
         */
        virtual uint32_t FileWrite(FileHandle handle, void* buffer, uint32_t bytesToWrite) const = 0;
        
        /**
         * @brief   Queries data size.
         * @param   handle File handle.
         * @return  Size of file in bytes.
         */
        virtual uint32_t FileGetSize(FileHandle handle) const = 0;
        
        /**
         * @brief   Attempts to close file.
         * @param   handle File handle.
         */
        virtual void FileClose(FileHandle handle) const = 0;
        
        /**
         * @brief   Flushes file contents.
         * @param   handle File handle.
         */
        virtual void FileFlush(FileHandle handle) const = 0;
    };
    
    FILESYSTEM_API std::shared_ptr<IFileSystemService> CreateFileSystemService();
    
    class FILESYSTEM_API FileSystemServiceLocator
    {
    public:
        static void Provide(std::shared_ptr<IFileSystemService> service)
        {
            mService = std::move(service);
        }
        
        static std::shared_ptr<IFileSystemService> Service()
        {
            return mService;
        }
        
        static bool Available()
        {
            return mService != nullptr;
        }
        
    private:
        static std::shared_ptr<IFileSystemService> mService;
    };
}
