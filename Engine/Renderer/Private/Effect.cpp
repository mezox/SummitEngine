#include <Renderer/Effect.h>
#include <Renderer/Resources/Buffer.h>
#include <Logging/LoggingService.h>

#include <exception>

using namespace Renderer;

void Effect::AddModule(const ModuleStage type, const std::string& filePath)
{
    mModuleDescriptors.push_back({ type, filePath });
}

void Effect::AddAttributeExplicit(const Format f, const uint32_t stride, const uint32_t location, const uint32_t binding, const uint32_t offset)
{
    throw std::runtime_error("Not implemented");
}

void Effect::AddAttribute(const Format f, const uint32_t binding)
{    
    mAttribBindings[binding].push_back(f);
}

void Effect::AddUniform(const UniformType type, const ModuleStage stage, const uint32_t binding, const uint32_t count)
{
    if(binding >= mUniformBindings.capacity())
        mUniformBindings.resize(binding + 1);
    
    mUniformBindings[binding].push_back({ type, stage, count });
}

void Effect::AddConstantRange(ModuleStage stage, const uint32_t offset, const uint32_t size)
{
    mConstantRanges.push_back({ stage, offset, size });
}

void Effect::AddUniformBuffer(ModuleStage stage, uint32_t binding, const Buffer& buffer)
{
    AddUniform(UniformType::Buffer, stage, binding, 1);
    
    mUniformBuffers.push_back(&buffer);
}

void Effect::AddTexture(ModuleStage stage, uint32_t binding, const Image& image)
{
    AddUniform(UniformType::Sampler, stage, binding, 1);
    
    mTextures.push_back(&image);
}

uint8_t Effect::GetBindingCount() const
{
    return mAttribBindings.size();
}

const std::vector<Format>& Effect::GetBindingDescriptor(const uint8_t binding) const
{
    const auto resultIt = mAttribBindings.find(binding);
    if(resultIt == mAttribBindings.end())
    {
        throw std::runtime_error("Effect::GetBindingDescriptor: Invalid binding!");
    }
    
    return resultIt->second;
}

const std::vector<Effect::ModuleDescriptor>& Effect::GetModuleDescriptors() const
{
    return mModuleDescriptors;
}

const std::vector<Effect::UniformBindingDesc>& Effect::GetUniformBindings() const
{
    return mUniformBindings;
}
