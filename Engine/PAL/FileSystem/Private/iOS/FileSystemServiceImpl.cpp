#include "FileSystemServiceImpl.h"

#import <Foundation/Foundation.h>

#define LOGGER_ID "Engine"

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('I','F','S',' ')

using namespace PAL::FileSystem;

namespace PAL::FileSystem
{
    std::unique_ptr<IFileSystemService> FileSystemServiceLocator::mService = nullptr;
}
    
class FileSystemServiceImpl::FileSystemNative
{
public:
    ~FileSystemNative()
    {
        mFileManager = nil;
        mWritableFolder = nil;
        mReadableFolder = nil;
    }
    
public:
    NSFileManager* mFileManager{ nil };
    NSString* mWritableFolder{ nil };
    NSString* mReadableFolder{ nil };
};

FileSystemServiceImpl::FileSystemServiceImpl()
    : mNativeFileSystem(std::make_unique<FileSystemNative>())
{
}

FileSystemServiceImpl::~FileSystemServiceImpl()
{
}

void FileSystemServiceImpl::Initialize()
{
    mNativeFileSystem->mFileManager = [NSFileManager defaultManager];
    mNativeFileSystem->mWritableFolder = [[[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:@"summitEngine"] copy];
    mNativeFileSystem->mReadableFolder = [[[NSBundle mainBundle] bundlePath] copy];
}
        
FileHandle FileSystemServiceImpl::FileOpen(const std::string& filePath, EFileAccessMode mode) const
{
    NSString* nsFilePath = [NSString stringWithUTF8String:filePath.c_str()];
    NSString* bundlePath = [[NSBundle bundleWithIdentifier:@"com.summit.engine"] bundlePath];
    NSString* bundleFilePath = [bundlePath stringByAppendingPathComponent:nsFilePath];
    
    auto strWritableDir = [[[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:@"summit"] copy];

    if([mNativeFileSystem->mFileManager fileExistsAtPath:bundleFilePath])
    {
        @autoreleasepool
        {
            NSFileHandle *handle{ nil };

            if (mode == EFileAccessMode::Read)
            {
                handle = [NSFileHandle fileHandleForReadingAtPath:bundleFilePath];
            }
            else if(mode == EFileAccessMode::Write)
            {
                handle = [NSFileHandle fileHandleForWritingAtPath:nsFilePath];
            }

            if (handle)
            {
                return (FileHandle)CFBridgingRetain(handle);
            }
        }
    }
    else
    {
        @autoreleasepool
        {
            NSFileHandle *handle{ nil };
            
            auto strWritableDir = [[[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:@"summit"] copy];
            NSString* bundleFilePath = [strWritableDir stringByAppendingPathComponent:nsFilePath];
            
            bool status = [mNativeFileSystem->mFileManager createFileAtPath:bundleFilePath contents:nil attributes:nil];
            
            if(mode == EFileAccessMode::Write)
            {
                handle = [NSFileHandle fileHandleForWritingAtPath:bundleFilePath];
            }
            
            if (handle)
            {
                return (FileHandle)CFBridgingRetain(handle);
            }
        }
    }
    
    return nullptr;
}
        
void FileSystemServiceImpl::FileClose(FileHandle handle) const
{
    CFBridgingRelease(handle);
}

void FileSystemServiceImpl::FileFlush(FileHandle handle) const
{
    NSFileHandle* file = (__bridge NSFileHandle*)handle;
    [file synchronizeFile];
}

uint32_t FileSystemServiceImpl::FileRead(FileHandle handle, void* buffer, uint32_t bytesToRead) const
{
    NSFileHandle* file = (__bridge NSFileHandle*)handle;
    
    uint32_t readBytes{ 0 };
    
    @autoreleasepool
    {
        NSData* data = [file readDataOfLength:bytesToRead];
        
        if( data )
        {
            [data getBytes:buffer length:bytesToRead];
            readBytes = (uint32_t)data.length;
        }
    }
    
    return readBytes;
}

uint32_t FileSystemServiceImpl::FileWrite(FileHandle handle, void* buffer, uint32_t bytesToWrite) const
{
    uint32_t writeBytes{ 0 };
    
    @autoreleasepool
    {
        NSFileHandle* file = (__bridge NSFileHandle*)handle;
        NSData *data = [[NSData alloc] initWithBytesNoCopy:(void*)buffer length:bytesToWrite freeWhenDone:NO];
        
        [file writeData:data];
        
        writeBytes = (uint32_t)data.length;
        
        data = nil;
    }
    
    return writeBytes;
}

uint32_t FileSystemServiceImpl::FileGetSize(FileHandle handle) const
{
    NSFileHandle* file = (__bridge NSFileHandle*)handle;
    
    uint32_t nOriginalPos = [file offsetInFile];
    uint32_t dwRet = (uint32_t)[file seekToEndOfFile];
    [file seekToFileOffset:nOriginalPos];
    
    return dwRet;
}
        
//std::string FileSystemServiceImpl::GetFilePath(const std::string& file) const
//{
//            NSString* strName = [NSString stringWithUTF8String:file.c_str()];
//
//            if([mFileManager fileExistsAtPath:strName])
//                return [[NSString stringWithFormat:@"%@/%@", [mFileManager currentDirectoryPath], strName] UTF8String];
//
//            return file;
//}
        
void FileSystemServiceImpl::DeInitialize()
{
    //LOG_INFORMATION("Deinitialized filesystem")
}
        
//    private:
//        std::string GetRealPath(const std::string& filePath) const
//        {
//            const std::string absolutePath("file://");
//
//            if(filePath.find(absolutePath) == 0)
//            {
//                return [[NSString stringWithUTF8String:filePath.substr(absolutePath.size(), std::string::npos).c_str()] UTF8String];
//            }
//
//            NSString* strFile = [NSString stringWithUTF8String:filePath.c_str()];
//            strFile = [strFile stringByReplacingOccurrencesOfString:mWritableFolder withString:@""];
//            strFile = [strFile stringByReplacingOccurrencesOfString:mReadableFolder withString:@""];
//
//            return [[mWritableFolder stringByAppendingPathComponent:strFile] UTF8String];
//        }
//
//    private:

//    };

std::unique_ptr<IFileSystemService> PAL::FileSystem::CreateFileSystemService()
{
    return std::make_unique<FileSystemServiceImpl>();
}
