#pragma once

#include <PAL/RenderAPI/RenderAPIService.h>

namespace PAL::RenderAPI
{
    class MetalRenderAPI : public IRenderAPI
    {
        class MetalImpl;
        
    public:
        MetalRenderAPI();
        ~MetalRenderAPI();
        
        void Initialize() override;
        std::shared_ptr<IDevice> CreateDevice(DeviceType type) override;
        void DeInitialize() override;
        
    private:
        std::unique_ptr<MetalImpl> mImpl;
    };
}
