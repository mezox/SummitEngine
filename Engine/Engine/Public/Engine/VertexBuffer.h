#pragma once

#include <Math/Vector2.h>
#include <Math/Vector3.h>

#include <PAL/RenderAPI/VulkanAPI.h>

#include <array>
#include <memory>

template<typename T>
struct BitMaskOperatorEnable
{
    static const bool enable = false;
};

template<typename T>
typename std::enable_if<BitMaskOperatorEnable<T>::enable, T>::type
operator |(T lhs, T rhs)
{
    using UT = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
}

template<typename T>
typename std::enable_if<BitMaskOperatorEnable<T>::enable, T>::type
operator &(T lhs, T rhs)
{
    using UT = typename std::underlying_type<T>::type;
    return static_cast<T>(static_cast<UT>(lhs) & static_cast<UT>(rhs));
}

namespace Renderer
{
    class RendererResource;
    
    enum class BufferUsage
    {
        VertexBuffer = 0,
        IndexBuffer,
    };
    
    enum class SharingMode
    {
        Exclusive = 0,
    };
    
    enum MemoryType
    {
        Undefined = 0x00000000,
        DeviceLocal = 0x00000001,
        HostVisible = 0x00000002,
        HostCoherent = 0x00000004
    };
    
    enum class VertexDataInputRate
    {
        Vertex,
        Instance
    };
    
    enum class VertexStreamDataType
    {
        Data,
        Index
    };
    
    struct BufferDesc
    {
        BufferUsage usage;
        SharingMode sharingMode;
        MemoryType memoryUsage;
        uint32_t bufferSize{ 0 };
        void* data{ nullptr };
    };
    
    class VertexBufferStreamBase
    {
    public:
        explicit VertexBufferStreamBase(VertexStreamDataType dataType) : mDataType(dataType) {}
        virtual ~VertexBufferStreamBase() = default;
        
        const bool IsDiscarded() const { return mIsDiscarded; }
        const bool IsCommited() const { return mIsCommited; }
        
        VertexDataInputRate GetVertexInputRate() const { return mInputRate; }
        VertexStreamDataType GetDataType() const { return mDataType; }
        
        std::unique_ptr<RendererResource>& GetDeviceResource() { return mGpuBuffer; }
        const RendererResource* GetDeviceResourcePtr() { return mGpuBuffer.get(); }
        
    protected:
        bool mIsCommited{ false };
        bool mIsDiscarded{ false };
        std::unique_ptr<RendererResource> mGpuBuffer;
        VertexDataInputRate mInputRate{ VertexDataInputRate::Vertex };
        VertexStreamDataType mDataType{ VertexStreamDataType::Data };
    };
    
    template<typename T>
    class VertexBufferStream : public VertexBufferStreamBase
    {
    public:
        VertexBufferStream(VertexStreamDataType dataType) : VertexBufferStreamBase(dataType) {}
        
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
        
    private:
        std::vector<T> mData;
    };
    
    class VertexBufferBase
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
                if(stream->GetDataType() == VertexStreamDataType::Data)
                {
                    ++count;
                }
            }
            
            return count;
        }
    };
    
    template<typename PositionType, typename IndexType>
    class VertexBufferPI : public VertexBuffer<2>
    {
    public:
        VertexBufferStream<PositionType>* GetPositionDataStream()
        {
            return (VertexBufferStream<PositionType>*)(mStreams[0]);
        }
        
        VertexBufferStream<IndexType>* GetIndexDataStream()
        {
            return (VertexBufferStream<IndexType>*)(mStreams[1]);
        }
    };
    
    template<typename PositionType, typename ColorType, typename IndexType>
    class VertexBufferPCI : public VertexBuffer<3>
    {
    public:
        VertexBufferStream<PositionType>& GetPositionDataStream()
        {
            if(!mStreams[0])
            {
                mStreams[0] = std::make_unique<VertexBufferStream<PositionType>>(VertexStreamDataType::Data);
            }
            
            return (VertexBufferStream<PositionType>&)(*mStreams[0].get());
        }
        
        VertexBufferStream<IndexType>& GetIndexDataStream()
        {
            if(!mStreams[1])
            {
                mStreams[1] = std::make_unique<VertexBufferStream<IndexType>>(VertexStreamDataType::Index);
            }
            
            return (VertexBufferStream<IndexType>&)(*mStreams[1].get());
        }
        
        VertexBufferStream<ColorType>& GetColorDataStream()
        {
            if(!mStreams[2])
            {
                mStreams[2] = std::make_unique<VertexBufferStream<ColorType>>(VertexStreamDataType::Data);
            }
            
            return (VertexBufferStream<ColorType>&)(*mStreams[2].get());
        }
    };
}
