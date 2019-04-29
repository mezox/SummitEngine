#include <Renderer/Image.h>

using namespace Renderer;

ImageDesc::ImageDesc(const uint32_t w) noexcept : width(w), mipMapLevels(1), type(ImageType::Image1D) {}
ImageDesc::ImageDesc(const uint32_t w, const uint32_t h) noexcept : width(w), height(h), mipMapLevels(1), type(ImageType::Image2D) {}
ImageDesc::ImageDesc(const uint32_t w, const uint32_t h, const uint32_t d) noexcept : width(w), height(h), depth(d), mipMapLevels(1), type(ImageType::Image3D) {}

Image::Image(const uint32_t w, const uint32_t h, const Format format, void* data)
    : mWidth(w)
    , mHeight(h)
    , mFormat(format)
{}
