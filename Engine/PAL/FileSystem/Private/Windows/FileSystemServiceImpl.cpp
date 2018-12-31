#include "FileSystemServiceImpl.h"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#	define NOMINMAX
#endif
#include <windows.h>

using namespace PAL::FileSystem;

class FileSystemServiceImpl::FileSystemNative
{
public:
    ~FileSystemNative()
    {

    }
    
public:

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
    
}
        
FileHandle FileSystemServiceImpl::FileOpen(const std::filesystem::path& filePath, EFileAccessMode mode) const
{   
	uint32_t fileAccess = 0;
	uint32_t shareMode = FILE_SHARE_READ;

	if (mode == EFileAccessMode::Read)
		fileAccess |= GENERIC_READ;
	if (mode == EFileAccessMode::Write || mode & EFileAccessMode::Append)
		fileAccess |= GENERIC_WRITE;

	return ::CreateFileW(filePath.c_str(), fileAccess, shareMode, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}
        
void FileSystemServiceImpl::FileClose(FileHandle handle) const
{
	::CloseHandle(handle);
}

void FileSystemServiceImpl::FileFlush(FileHandle handle) const
{
	::FlushFileBuffers(handle);
}

uint32_t FileSystemServiceImpl::FileRead(FileHandle handle, void* buffer, uint32_t bytesToRead) const
{
	DWORD bytesRead;
	::ReadFile(handle, buffer, bytesToRead, &bytesRead, NULL);
	return static_cast<uint32_t>(bytesRead);
}

uint32_t FileSystemServiceImpl::FileWrite(FileHandle handle, void* buffer, uint32_t bytesToWrite) const
{
	DWORD writeBytes{ 0 };

	::WriteFile(handle, buffer, bytesToWrite, &writeBytes, nullptr);
	return static_cast<uint32_t>(writeBytes);
}

uint32_t FileSystemServiceImpl::FileGetSize(FileHandle handle) const
{
	return ::GetFileSize(handle, nullptr);
}
        
void FileSystemServiceImpl::DeInitialize()
{
}

std::shared_ptr<IFileSystemService> PAL::FileSystem::CreateFileSystemService()
{
    return std::make_shared<FileSystemServiceImpl>();
}

#include "FileSystemTests.inl"
