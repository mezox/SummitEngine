#include <Renderer/Renderer.h>
#include <Renderer/VertexBuffer.h>

using namespace Renderer;

VertexBufferStreamBase::VertexBufferStreamBase(BufferUsage dataType)
    : mDataType(dataType)
{}

const bool VertexBufferStreamBase::IsDiscarded() const
{
    return mIsDiscarded;
}

const bool VertexBufferStreamBase::IsCommited() const
{
    return mIsCommited;
}

VertexDataInputRate VertexBufferStreamBase::GetVertexInputRate() const
{
    return mInputRate;
}

BufferUsage VertexBufferStreamBase::GetDataType() const
{
    return mDataType;
}

DeviceObject& VertexBufferStreamBase::GetDeviceResource()
{
    return mGpuBuffer;
}

const DeviceObject& VertexBufferStreamBase::GetDeviceResourcePtr() const
{
    return mGpuBuffer;
}

bool VertexBufferStreamBase::Commit(CommitCommand)
{
    auto& renderer = RendererLocator::GetRenderer();
    
    BufferDesc descriptorPosition;
    descriptorPosition.usage = mDataType;
    descriptorPosition.sharingMode = SharingMode::Exclusive;
    descriptorPosition.memoryUsage = MemoryType::DeviceLocal;
    descriptorPosition.bufferSize = mStreamData.count * mStreamData.stride;
    descriptorPosition.data = mStreamData.data;
    
    renderer.CreateBuffer(descriptorPosition, mGpuBuffer);
    mIsCommited = true;
    
    return mIsCommited;
}
