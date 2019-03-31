#pragma once

#include "RendererBase.h"
#include "DeviceObject.h"
#include "Resources/Framebuffer.h"
#include "Resources/DeviceResource.h"

#include <cstdint>
#include <vector>
#include <memory>

namespace Renderer
{
    /*!
     * @brief Base class representing swap chain.
     */
    class RENDERER_API SwapChainBase : public DeviceResource
    {
    public:
        /*!
         * @brief Base class representing swap chain.
         */
        explicit SwapChainBase(DeviceObject&& deviceObject);
        
        /*!
         @brief Swap chain destructor.
         */
        virtual ~SwapChainBase() = default;
        
        SwapChainBase(SwapChainBase&& other) = delete;
        SwapChainBase& operator=(SwapChainBase&& other) = delete;
        
        void AddFramebuffer(Framebuffer&& fb) { mFramebuffers.push_back(std::move(fb)); }
        void SetDepthAttachment(std::shared_ptr<Attachment> depth) { mDepthAttachment = std::move(depth); }
        
        virtual void Destroy() = 0;
        virtual void SwapBuffers() = 0;

    protected:
        uint32_t mImageIndex{ 0 };
        std::vector<Framebuffer> mFramebuffers;
        std::shared_ptr<Attachment> mDepthAttachment;
    };
}
