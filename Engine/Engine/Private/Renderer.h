#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>
#include <Core/Service.h>

namespace Renderer
{
	class RendererResource
	{
	public:
		virtual ~RendererResource() = default;
	};

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

	class IRenderer
	{
	public:
		virtual void Initialize() = 0;
		virtual void Deinitialize() = 0;

		virtual std::shared_ptr<IDevice> CreateDevice(DeviceType type) = 0;
		/*void CreateSwapChain(std::unique_ptr<PAL::RenderAPI::RendererResource>& swapChain, std::unique_ptr<PAL::RenderAPI::RendererResource>& surface, uint32_t width, uint32_t height) const {}
*/
		virtual void CreateWindowSurface(std::unique_ptr<RendererResource>& surface, void* nativeHandle) const = 0;

	private:
	};
	
	std::unique_ptr<IRenderer> CreateRenderer();
    
    class RendererServiceLocator
    {
    public:
        static void Provide(std::unique_ptr<IRenderer> service)
        {
            mService = std::move(service);
        }
        
        static IRenderer& Service()
        {
            if(!mService)
            {
                throw std::runtime_error("FileSystem service unitialized");
            }
            
            return *mService;
        }
        
        static bool Available()
        {
            return mService != nullptr;
        }
        
    private:
        static std::unique_ptr<IRenderer> mService;
    };
}
