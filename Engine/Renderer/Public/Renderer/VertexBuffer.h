#pragma once

#include "RendererBase.h"
#include "DeviceObject.h"
#include "SharedDeviceTypes.h"

#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <PAL/RenderAPI/Vulkan/VulkanAPI.h>
#include <Core/FlagMask.h>

#include <array>
#include <memory>

namespace Renderer
{
    enum class BufferUsage
    {
        Undefined,
        VertexBuffer,
        UniformBuffer,
        IndexBuffer
    };
    
    enum class VertexDataInputRate
    {
        Vertex,
        Instance
    };
    
    struct RENDERER_API BufferDesc
    {
        BufferUsage usage;
        Core::FlagMask<MemoryType> memoryUsage;
        uint32_t bufferSize{ 0 };
        void* data{ nullptr };
    };
    
    enum class CommitCommand
    {
        Commit,
        Map,
        CommitDiscard,
        MapAndDiscard,
        Discard
    };
    
    class RENDERER_API VertexBufferStreamBase
    {
        struct StreamData
        {
            void* data{ nullptr };
            uint32_t count{ 0 };
            uint32_t stride{ 0 };
        };
        
    public:
        explicit VertexBufferStreamBase(BufferUsage dataType);
        virtual ~VertexBufferStreamBase() = default;
        
        const bool IsDiscarded() const;
        const bool IsCommited() const;
        
        uint32_t GetStride() const;
        uint32_t GetCount() const;
        
        VertexDataInputRate GetVertexInputRate() const;
        BufferUsage GetDataType() const;
        
        DeviceObject& GetDeviceResource();
        const DeviceObject& GetDeviceResourcePtr() const;
        
        bool Commit(CommitCommand cmd);
        
    protected:
        void InvalidateStream();
        
    protected:
        DeviceObject mGpuBuffer;
        StreamData mStreamData;
        bool mIsCommited{ false };
        bool mIsDiscarded{ false };
        Core::FlagMask<MemoryType> mMemoryType;
        VertexDataInputRate mInputRate{ VertexDataInputRate::Vertex };
        BufferUsage mDataType{ BufferUsage::VertexBuffer };
    };
    
    template<typename T>
    class VertexBufferStream : public VertexBufferStreamBase
    {
    public:
        VertexBufferStream(BufferUsage dataType) : VertexBufferStreamBase(dataType) {}
        
        std::vector<T>& GetData()
        {
            return mData;
        }
        
        const std::vector<T>& GetData() const
        {
            return mData;
        }
        
        uint32_t GetSize() const
        {
            return static_cast<uint32_t>(mData.capacity() * sizeof(T));
        }
        
        uint32_t GetElementCount() const
        {
            return static_cast<uint32_t>(mData.size());
        }
        
        uint32_t GetStride() const
        {
            return static_cast<uint32_t>(sizeof(T));
        }
        
        void Discard()
        {
            std::vector<T> temp{};
            std::swap(mData, temp);
            
            mIsDiscarded = true;
        }
        
        void Lock(CommitCommand cmd)
        {
            mStreamData.count = mData.size();
            mStreamData.stride = sizeof(T);
            mStreamData.data = mData.data();
            
            Commit(cmd);
            
            if(cmd == CommitCommand::CommitDiscard || cmd == CommitCommand::Discard)
            {
                Discard();
            }
        }
        
        void Invalidate()
        {
            Discard();
            InvalidateStream();
        }
        
    private:
        std::vector<T> mData;
    };
    
    class RENDERER_API VertexBufferBase
    {
    public:
        virtual ~VertexBufferBase() = default;
        
        std::vector<std::unique_ptr<VertexBufferStreamBase>> mStreams;
    };
    
    template<size_t STREAM_COUNT>
    class VertexBuffer : public VertexBufferBase
    {
    public:
        VertexBuffer()
        {
            mStreams.resize(STREAM_COUNT);
        }
        
        uint32_t GetDataStreamCount() const
        {
            uint32_t count{ 0 };
            for(const auto& stream : mStreams)
            {
                if(stream->GetDataType() != BufferUsage::IndexBuffer)
                {
                    ++count;
                }
            }
            
            return count;
        }
    };
    
    template<typename DataStreamType, typename IndexType>
    class VertexBufferPI : public VertexBuffer<2>
    {
    public:
        VertexBufferStream<DataStreamType>* GetPositionDataStream()
        {
            return (VertexBufferStream<DataStreamType>*)(mStreams[0]);
        }
        
        VertexBufferStream<IndexType>* GetIndexDataStream()
        {
            return (VertexBufferStream<IndexType>*)(mStreams[1]);
        }
    };
    
    template<typename Data0StreamType, typename Data1StreamType, typename IndexType>
    class VertexBufferPCI : public VertexBuffer<3>
    {
    public:
        VertexBufferStream<Data0StreamType>& GetPositionDataStream()
        {
            if(!mStreams[0])
            {
                mStreams[0] = std::make_unique<VertexBufferStream<Data0StreamType>>(BufferUsage::VertexBuffer);
            }
            
            return (VertexBufferStream<Data0StreamType>&)(*mStreams[0].get());
        }
        
        VertexBufferStream<IndexType>& GetIndexDataStream()
        {
            if(!mStreams[1])
            {
                mStreams[1] = std::make_unique<VertexBufferStream<IndexType>>(BufferUsage::IndexBuffer);
            }
            
            return (VertexBufferStream<IndexType>&)(*mStreams[1].get());
        }
        
        VertexBufferStream<Data1StreamType>& GetColorDataStream()
        {
            if(!mStreams[2])
            {
                mStreams[2] = std::make_unique<VertexBufferStream<Data1StreamType>>(BufferUsage::VertexBuffer);
            }
            
            return (VertexBufferStream<Data1StreamType>&)(*mStreams[2].get());
        }
    };
    
    template<typename PositionType, typename TexCoordType, typename ColorType, typename IndexType>
    class VertexBufferPTCI : public VertexBuffer<4>
    {
    public:
        VertexBufferStream<PositionType>& GetPositionDataStream()
        {            
            if(!mStreams[0])
            {
                mStreams[0] = std::make_unique<VertexBufferStream<PositionType>>(BufferUsage::VertexBuffer);
            }
            
            return (VertexBufferStream<PositionType>&)(*mStreams[0].get());
        }
        
        VertexBufferStream<IndexType>& GetIndexDataStream()
        {
            if(!mStreams[1])
            {
                mStreams[1] = std::make_unique<VertexBufferStream<IndexType>>(BufferUsage::IndexBuffer);
            }
            
            return (VertexBufferStream<IndexType>&)(*mStreams[1].get());
        }
        
        VertexBufferStream<TexCoordType>& GetTexCoordDataStream()
        {
            if(!mStreams[3])
            {
                mStreams[3] = std::make_unique<VertexBufferStream<TexCoordType>>(BufferUsage::VertexBuffer);
            }
            
            return (VertexBufferStream<TexCoordType>&)(*mStreams[3].get());
        }
        
        VertexBufferStream<ColorType>& GetColorDataStream()
        {
            if(!mStreams[2])
            {
                mStreams[2] = std::make_unique<VertexBufferStream<ColorType>>(BufferUsage::VertexBuffer);
            }
            
            return (VertexBufferStream<ColorType>&)(*mStreams[2].get());
        }
    };
}
