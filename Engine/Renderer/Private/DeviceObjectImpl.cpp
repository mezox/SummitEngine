#include <Renderer/DeviceObject.h>

using namespace Renderer;

namespace Renderer
{
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
}

DeviceObject::DeviceObject()
    : mImpl(new (&mStorage) EmptyDeviceObject)
{}

DeviceObject::DeviceObject(DeviceObject&& other) noexcept
    : mImpl(other.mImpl->Move(&mStorage))
{}

DeviceObject::~DeviceObject()
{
    mImpl->~IDeviceObjectImpl();
}

DeviceObject& DeviceObject::operator=(DeviceObject&& other) noexcept
{
    mImpl->~IDeviceObjectImpl();
    mImpl = other.mImpl->Move(&mStorage);
    return *this;
}

void DeviceObject::Accept(IDeviceObjectVisitor& visitor) const
{
    mImpl->Accept(visitor);
}
