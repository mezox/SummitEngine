#include <PAL/FileSystem/File.h>
#include <PAL/FileSystem/FileSystemService.h>

using namespace PAL::FileSystem;

File::File(const std::string& path)
: mPath(path)
{
    
}

File::~File()
{
    Close();
}

void File::Open(EFileAccessMode mode)
{
    if(!FileSystemServiceLocator::Available())
        return;
    
    mHandle = FileSystemServiceLocator::Service().FileOpen(mPath, mode);
    if(!mHandle)
    {
        throw FileException();
    }
}

void File::ReadBytes(uint32_t bytesToRead)
{
//    const uint32_t bytesRead = FileSystemServiceLocator::Service()->FileRead(mHandle, mBuffer.get(), bytesToRead);
//    if(bytesToRead != bytesRead)
//    {
//        throw FileException();
//    }
}

void File::Close()
{
    if(!FileSystemServiceLocator::Available())
        return;
    
    if (mHandle)
    {
        FileSystemServiceLocator::Service().FileClose(mHandle);
        mHandle = nullptr;
    }
}

bool File::IsOpened() const
{
    return mHandle != nullptr;
}

const std::vector<uint8_t>& File::Read()
{
    if(!FileSystemServiceLocator::Available())
        return{};
    
    if(!IsOpened())
        Open(EFileAccessMode::Read);
    
    const uint32_t fileSize = FileSystemServiceLocator::Service().FileGetSize(mHandle);
    
    std::vector<uint8_t> data;
    data.resize(fileSize);
    
    mBuffer = std::make_unique<std::vector<uint8_t>>(data);
    
    const uint32_t bytesRead = FileSystemServiceLocator::Service().FileRead(mHandle, mBuffer->data(), fileSize);
    if(fileSize != bytesRead)
    {
        mBuffer.release();
        Close();
        throw FileException();
    }
    
    Close();
    
    return *mBuffer.get();
}

uint32_t File::Write(void* data, uint32_t bytesToWrite) const
{
    if(!FileSystemServiceLocator::Available())
        return 0;
    
    return FileSystemServiceLocator::Service().FileWrite(mHandle, data, bytesToWrite);
}

void File::Flush() const
{
    if(!FileSystemServiceLocator::Available())
        return;
    
    FileSystemServiceLocator::Service().FileFlush(mHandle);
}
