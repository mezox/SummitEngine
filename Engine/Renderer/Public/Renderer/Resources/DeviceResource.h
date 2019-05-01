#pragma once

#include <Renderer/RendererBase.h>
#include <Renderer/DeviceObject.h>
#include <Rednerer/Resources/Types.h>

namespace Renderer
{
	/*!
     * @brief    Class representing graphics API agnostic device resource object. MoveOnly.
     */
    class RENDERER_API DeviceResource
    {
    public:        
        /*!
         @brief Constructs DeviceResource object.
         @param deviceObject Device object handler.
         */
        DeviceResource(DeviceObject&& deviceObject) : mDeviceResource(std::move(deviceObject)) {}
        
        /*!
         @brief DeviceResource destructor.
         */
        virtual ~DeviceResource() = default;
        
        DeviceResource() = default;
        
        DeviceResource(const DeviceResource& other) = delete;
        DeviceResource(DeviceResource&& other) = default;
        DeviceResource& operator=(const DeviceResource& other) = delete;
        DeviceResource& operator=(DeviceResource&& other) = default;
        
        /*!
         @brief Returns read-only device object handler.
         */
        [[nodiscard]] const DeviceObject& GetDeviceObject() const { return mDeviceResource; }
        
        /*!
         @brief Sets device object handler
         */
        void SetDeviceObject(DeviceObject&& deviceObject) { mDeviceResource = std::move(deviceObject); }
        
        [[nodiscard]] DeviceObject& GetDeviceObject() { return mDeviceResource; }
        
        //TODO: make private
    protected:
        /*!
         @brief Device object handler.
         */
        DeviceObject mDeviceResource;
    };

}
