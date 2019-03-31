#include <Renderer/Image.h>
#include <Renderer/Renderer.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Renderer;

Image::Image(const uint32_t w, const uint32_t h, const Format format, void* data)
    : mWidth(w)
    , mHeight(h)
    , mFormat(format)
{
    mData = std::make_unique<std::vector<unsigned char>>((char*)data, (char*)data + (w * h * GetSizeFromFormat(format)));
    
    auto& renderer = RendererLocator::GetRenderer();
    
    ImageDesc imgDescriptor;
    imgDescriptor.width = mWidth;
    imgDescriptor.height = mHeight;
    imgDescriptor.depth = 1;
    imgDescriptor.format = mFormat;
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
    
    renderer.CreateTexture(imgDescriptor, samplerDesc, mGpuImage);
}

Image Image::CreateFromFile(const std::string& path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    
    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }
    
    Image image(texWidth, texHeight, Format::R8G8B8A8, pixels);
    
    stbi_image_free(pixels);
    
    return image;
}
