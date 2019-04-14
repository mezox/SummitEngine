#pragma once

#include <Renderer/RendererBase.h>
#include <type_traits>
#include <stdexcept>
#include <memory>

namespace Renderer
{
    class PipelineDeviceObject;
    class BufferDeviceObject;
    class FramebufferDeviceObject;
    class VulkanShaderDeviceObject;
    class TextureDeviceObject;
    class VulkanRenderPassDeviceObject;
    class VulkanAttachmentDeviceObject;
    
    namespace Vulkan
    {
        struct SurfaceDeviceObject;
        class DescriptorSetLayoutDeviceObject;
        class DescriptorSetDeviceObject;
        struct SemaphoreDeviceObject;
        struct FenceDeviceObject;
        struct EventDeviceObject;
        struct RenderPassDeviceObject;
        struct CommandBufferDeviceObject;
        struct FrameSyncDeviceObject;
        struct SwapChainDeviceObject;
    }
    
    class IDeviceObjectVisitor
    {
    public:
        virtual void Visit(const VulkanShaderDeviceObject& object) = 0;
        virtual void Visit(const VulkanRenderPassDeviceObject& object) = 0;
        virtual void Visit(const PipelineDeviceObject& object) = 0;
        virtual void Visit(const BufferDeviceObject& object) = 0;
        virtual void Visit(const FramebufferDeviceObject& object) = 0;
        virtual void Visit(const TextureDeviceObject& object) = 0;
        virtual void Visit(const VulkanAttachmentDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::SurfaceDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::SwapChainDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::DescriptorSetLayoutDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::DescriptorSetDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::SemaphoreDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::FenceDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::EventDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::RenderPassDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::CommandBufferDeviceObject& object) = 0;
        virtual void Visit(const Vulkan::FrameSyncDeviceObject& object) = 0;
    };
    
    class IMutableDeviceObjectVisitor
    {
    public:
        virtual void Visit(VulkanShaderDeviceObject& object) = 0;
        virtual void Visit(VulkanRenderPassDeviceObject& object) = 0;
        virtual void Visit(PipelineDeviceObject& object) = 0;
        virtual void Visit(BufferDeviceObject& object) = 0;
        virtual void Visit(FramebufferDeviceObject& object) = 0;
        virtual void Visit(TextureDeviceObject& object) = 0;
        virtual void Visit(VulkanAttachmentDeviceObject& object) = 0;
        virtual void Visit(Vulkan::SurfaceDeviceObject& object) = 0;
        virtual void Visit(Vulkan::SwapChainDeviceObject& object) = 0;
        virtual void Visit(Vulkan::DescriptorSetLayoutDeviceObject& object) = 0;
        virtual void Visit(Vulkan::DescriptorSetDeviceObject& object) = 0;
        virtual void Visit(Vulkan::SemaphoreDeviceObject& object) = 0;
        virtual void Visit(Vulkan::FenceDeviceObject& object) = 0;
        virtual void Visit(Vulkan::EventDeviceObject& object) = 0;
        virtual void Visit(Vulkan::RenderPassDeviceObject& object) = 0;
        virtual void Visit(Vulkan::CommandBufferDeviceObject& object) = 0;
        virtual void Visit(Vulkan::FrameSyncDeviceObject& object) = 0;
    };
    
    class IDeviceObjectImpl
    {
    public:
        virtual ~IDeviceObjectImpl() = default;
        virtual IDeviceObjectImpl* Move(void* address) = 0;
        virtual void Accept(IDeviceObjectVisitor& visitor) const = 0;
        virtual void Accept(IMutableDeviceObjectVisitor& visitor) = 0;
    };
    
    class RENDERER_API DeviceObject
    {
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
            
            void Accept(IMutableDeviceObjectVisitor& visitor) override
            {
                visitor.Visit(data);
            }
            
        public:
            T data;
        };
    
    public:
        DeviceObject();
        DeviceObject(DeviceObject&& other) noexcept;
        ~DeviceObject();
        
        DeviceObject(const DeviceObject& other) = delete;
        DeviceObject& operator=(const DeviceObject& other) = delete;
        DeviceObject& operator=(DeviceObject&& other) noexcept;
        
        template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, DeviceObject>::value>>
        DeviceObject& operator=(T&& impl) noexcept;
        
        void Accept(IDeviceObjectVisitor& visitor) const;
        void Accept(IMutableDeviceObjectVisitor& visitor);
        
    private:
        enum
        {
            c_max_id_sizeof_vtable = 288,
            c_max_id_sizeof = c_max_id_sizeof_vtable - sizeof(IDeviceObjectImpl*),
            c_alignment = 8
        };
        
    private:
        std::aligned_storage<c_max_id_sizeof_vtable>::type mStorage;
        IDeviceObjectImpl* mImpl{ nullptr };
    };
    
    template <typename T, typename F>
    DeviceObject& DeviceObject::operator=(T&& impl) noexcept
    {        
        static_assert(sizeof(T) <= c_max_id_sizeof, "Object too big");
        mImpl->~IDeviceObjectImpl();
        mImpl = new (&mStorage) DeviceObjectImpl<std::decay_t<T>>(std::forward<T>(impl));
        return *this;
    }
    
    template<typename T>
    DeviceObject Basify(T&& impl)
    {
        DeviceObject obj;
        obj = std::move(impl);
        return obj;
    }
}
