#include "Command.h"
#include <PAL/RenderAPI/Vulkan/VulkanDevice.h>

using namespace Renderer;

Command::Command(Command&& other) noexcept
    : mImpl(other.mImpl->Move(&mStorage))
{}

//Command::~Command()
//{
//    if(mImpl)
//    {
//        mImpl->~ICommandImpl();
//    }
//}

Command& Command::operator=(Command&& other) noexcept
{
    mImpl->~ICommandImpl();
    mImpl = other.mImpl->Move(&mStorage);
    other.mImpl->~ICommandImpl();
    return *this;
}

std::string Command::GetDescription() const
{
    return mImpl->GetDescription();
}

void Command::Execute(const PAL::RenderAPI::VulkanDevice& device, const DeviceObject& commandBuffer) const
{
    mImpl->Execute(device, commandBuffer);
}
