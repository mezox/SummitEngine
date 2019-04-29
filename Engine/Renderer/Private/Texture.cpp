#include <Renderer/Resources/Texture.h>
#include <Renderer/Image.h>
#include <Renderer/Renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Renderer;

Texture::Texture(uint32_t width, uint32_t height, Format format, void* data)
    : Attachable(AttachableDescriptor{ width, height, format, ImageUsage::Sampled })
{
    mData = std::make_unique<std::vector<unsigned char>>((char*)data, (char*)data + (width * height * GetSizeFromFormat(format)));
    
    auto& renderer = RendererLocator::GetRenderer();
    
    ImageDesc imgDescriptor;
    imgDescriptor.width = GetWidth();
    imgDescriptor.height = GetHeight();
    imgDescriptor.depth = 1;
    imgDescriptor.format = GetFormat();
    imgDescriptor.memoryUsage = MemoryType::DeviceLocal;
    imgDescriptor.mipMapLevels = 1;
    imgDescriptor.usage = ImageUsage::Sampled;
    imgDescriptor.type = ImageType::Image2D;
    imgDescriptor.data = mData->data();
    
    SamplerDesc samplerDesc;
    samplerDesc.anisotropy = 0;
    samplerDesc.minFilter = FilterMode::Linear;
    samplerDesc.magFilter = FilterMode::Linear;
    samplerDesc.uAddressMode = AddressMode::Repeat;
    samplerDesc.vAddressMode = AddressMode::Repeat;
    samplerDesc.wAddressMode = AddressMode::Repeat;
    
    renderer.CreateTexture(imgDescriptor, samplerDesc, mDeviceResource);
}

Texture Texture::CreateFromFile(const std::string& path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    
    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }
    
    Texture image(texWidth, texHeight, Format::R8G8B8A8, pixels);
    
    stbi_image_free(pixels);
    
    return image;
}

