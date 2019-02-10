#pragma once

#include <Renderer/RendererBase.h>
#include <type_traits>
#include <stdexcept>
#include <memory>

namespace Renderer
{
    class PipelineDeviceObject;
    class BufferDeviceObject;
    
    class IDeviceObjectVisitor
    {
    public:
        virtual void Visit(const PipelineDeviceObject& object) = 0;
        virtual void Visit(const BufferDeviceObject& object) = 0;
    };
    
    class RENDERER_API DeviceObject
    {
        class IDeviceObjectImpl
        {
        public:
            virtual ~IDeviceObjectImpl() = default;
            virtual IDeviceObjectImpl* Move(void* address) = 0;
            virtual void Accept(IDeviceObjectVisitor& visitor) const = 0;
        };
        
        class EmptyDeviceObject final : public IDeviceObjectImpl
        {
            IDeviceObjectImpl* Move(void* addr) override
            {
                return new (addr) EmptyDeviceObject(std::move(*this));
            }
            
            void Accept(IDeviceObjectVisitor& visitor) const override
            {
                throw std::invalid_argument("Bad function call");
            }
        };
        
        template<typename T>
        class DeviceObjectImpl final : public IDeviceObjectImpl
        {
        public:
            DeviceObjectImpl() = default;
            DeviceObjectImpl(const T& v) : data(v) {}
            DeviceObjectImpl(T&& v) : data(std::move(v)) {}
            
            IDeviceObjectImpl* Move(void* addr) override
            {
                return new (addr) DeviceObjectImpl(std::move(*this));
            }
            
            void Accept(IDeviceObjectVisitor& visitor) const override
            {
                visitor.Visit(data);
            }
            
        public:
            T data;
        };
    
    public:
        DeviceObject() : mImpl(new (&mStorage) EmptyDeviceObject) {}
        DeviceObject(DeviceObject&& other) noexcept : mImpl(other.mImpl->Move(&mStorage)) {}
        DeviceObject(const DeviceObject& other) = delete;
        ~DeviceObject() { mImpl->~IDeviceObjectImpl(); }
        
        DeviceObject& operator=(const DeviceObject& other) = delete;
        DeviceObject& operator=(DeviceObject&& other) noexcept
        {
            mImpl->~IDeviceObjectImpl();
            mImpl = other.mImpl->Move(&mStorage);
            return *this;
        }
        
        template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, DeviceObject>::value>>
        DeviceObject& operator=(T&& impl)
        {
            static_assert(sizeof(T) <= c_max_id_sizeof, "Object too big");
            mImpl->~IDeviceObjectImpl();
            mImpl = new (&mStorage) DeviceObjectImpl<std::decay_t<T>>(std::forward<T>(impl));
            return *this;
        }
        
        void Accept(IDeviceObjectVisitor& visitor) const
        {
            mImpl->Accept(visitor);
        }
        
    private:
        enum
        {
            c_max_id_sizeof = 32,
            c_alignment = 8
        };
        
    private:
        std::aligned_storage<c_max_id_sizeof>::type mStorage;
        IDeviceObjectImpl* mImpl{ nullptr };
    };
}
