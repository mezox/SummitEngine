#pragma once

#include <Core/Service.h>
#include <PAL/RenderAPI/RenderAPIBase.h>

namespace PAL::RenderAPI
{
    enum class DeviceType
    {
        External,
        Discrete,
        Integrated
    };
    
    class RENDERAPI_API IDevice
    {
    public:
        virtual ~IDevice() = default;
    };
    
    enum class RenderBackend
    {
        Metal,
        Vulkan,
        OpenGL
    };
    
    class RENDERAPI_API IRenderAPI
    {
    public:
        virtual ~IRenderAPI() = default;
        
        /**
         * @brief   Initializes platform's render api.
         */
        virtual void Initialize() = 0;
        
        /**
         * @brief   Creates new device.
         */
        virtual std::shared_ptr<IDevice> CreateDevice(DeviceType type) = 0;
        
        /**
         * @brief   Deinitializes platform's render api.
         */
        virtual void DeInitialize() = 0;
    };
    
    class RENDERAPI_API RenderAPIServiceLocator : public Core::ServiceLocatorBase<IRenderAPI>
    {
    public:
        RenderAPIServiceLocator() = default;
    };
    
    
    RENDERAPI_API std::shared_ptr<IRenderAPI> CreateRenderAPI(RenderBackend backend);
}
