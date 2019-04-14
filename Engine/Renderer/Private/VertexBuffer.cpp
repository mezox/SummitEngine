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

uint32_t VertexBufferStreamBase::GetStride() const
{
    return mStreamData.stride;
}

uint32_t VertexBufferStreamBase::GetCount() const
{
    return mStreamData.count;
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

bool VertexBufferStreamBase::Commit(CommitCommand cmd)
{
    auto& renderer = RendererLocator::GetRenderer();
    
    BufferDesc descriptorPosition;
    descriptorPosition.usage = mDataType;
    descriptorPosition.memoryUsage = (cmd == CommitCommand::Commit) ? MemoryType::DeviceLocal : MemoryType::HostVisible;
    descriptorPosition.bufferSize = mStreamData.count * mStreamData.stride;
    descriptorPosition.data = mStreamData.data;
    
    renderer.CreateBuffer(descriptorPosition, mGpuBuffer);
    mIsCommited = true;
    mMemoryType = descriptorPosition.memoryUsage;
    
    return mIsCommited;
}

void VertexBufferStreamBase::InvalidateStream()
{
    if(mIsCommited)
    {
        mStreamData = {};
        RendererLocator::GetRenderer().DestroyDeviceObject(mGpuBuffer);
    }
}
