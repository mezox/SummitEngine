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
        virtual ~SwapChainBase();
        
        SwapChainBase(SwapChainBase&& other) = delete;
        SwapChainBase& operator=(SwapChainBase&& other) = delete;
        
        void AddFramebuffer(Framebuffer&& fb);
        void SetDepthAttachment(std::shared_ptr<Attachment> depth);
        
        const Framebuffer& GetActiveFramebuffer() const;
        
        virtual void Destroy() = 0;
        virtual void SwapBuffers() = 0;
        virtual bool AcquireImage() = 0;

    protected:
        std::vector<Framebuffer> mFramebuffers;
        std::shared_ptr<Attachment> mDepthAttachment;
        uint32_t mAcquiredImageIndex{ 0 };
    };
}
