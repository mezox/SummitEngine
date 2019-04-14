#pragma once

#include <Renderer/RendererBase.h>

#include <string>

namespace PAL::RenderAPI
{
    class VulkanDevice;
}

namespace Renderer
{
    class DeviceObject;

    class ICommandImpl
    {
    public:
        virtual ~ICommandImpl() = default;
        
        virtual void Execute(const PAL::RenderAPI::VulkanDevice& device, const DeviceObject& cmdBuffer) const = 0;
        virtual ICommandImpl* Move(void* address) = 0;
        virtual std::string GetDescription() const = 0;
    };
    
    template<typename T>
    class CommandImpl final : public ICommandImpl
    {
    public:
        CommandImpl(T v) : data(std::move(v)) {}
        
        ICommandImpl* Move(void* addr) override
        {
            return new (addr) CommandImpl(std::move(*this));
        }

        void Execute(const PAL::RenderAPI::VulkanDevice& device, const DeviceObject& cmdBuffer) const override { data.Execute(device, cmdBuffer); }
        std::string GetDescription() const override { return data.GetDescription(); }
        
    public:
        T data;
    };
    
    class RENDERER_API Command
    {
        static constexpr std::size_t maxStorageSize = 96 - sizeof(ICommandImpl*);
        
    public:
        Command() = default;
        Command(Command&& other) noexcept;
        Command& operator=(Command&& other) noexcept;
        
        template<   typename T,
                    typename = std::enable_if_t<!std::is_same<std::decay_t<T>, DeviceObject>::value>>
        Command(T&& impl) noexcept
        {
            static_assert(sizeof(T) <= maxStorageSize, "Object too big");
            mImpl = new (&mStorage) CommandImpl<std::decay_t<T>>(std::forward<T>(impl));
        }
        
        Command(const Command& other) = delete;
        Command& operator=(const Command& other) = delete;

        template<typename T>
        static Command Create(T&& command);

        std::string GetDescription() const;
        void Execute(const PAL::RenderAPI::VulkanDevice& device, const DeviceObject& commandBuffer) const;
        
    private:
        std::aligned_storage<maxStorageSize>::type mStorage;
        ICommandImpl* mImpl{ nullptr };
    };
    
    template<typename T>
    Command Command::Create(T&& command)
    {
        Command instance;
        CommandImpl<T>* impl = new (&instance.mStorage) CommandImpl<T>(std::forward<T>(command));
        instance.mImpl = (ICommandImpl*)((char*)&instance.mStorage);
        return instance;
    }
}

