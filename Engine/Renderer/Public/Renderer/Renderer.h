#pragma once
#include "RendererBase.h"

#include <Renderer/VertexBuffer.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>

#include <string>

namespace Renderer
{
	class RENDERER_API RendererResource
	{
	public:
		virtual ~RendererResource() = default;
	};
    
    class RENDERER_API SwapChainResource : public RendererResource
    {
    public:
        virtual void Initialize(const uint32_t width, const uint32_t height) = 0;
        virtual void Destroy() = 0;
        virtual void SwapBuffers() = 0;
    };

	enum class DeviceType
	{
		External,
		Discrete,
		Integrated
	};
    
    class RENDERER_API Object3D
    {
    public:
        VertexBufferPCI<Vector3f, Vector3f, uint16_t> mVertexBuffer;
    };

	enum class RenderBackend
	{
		Metal,
		Vulkan,
		OpenGL
	};
    
    class Pipeline;

	class RENDERER_API IRenderer
	{
	public:
		virtual void Initialize() = 0;
		virtual void Deinitialize() = 0;
        
        virtual void UpdateCamera(uint32_t imageIndex) = 0;

        virtual void CreateSwapChain(std::unique_ptr<SwapChainResource>& swapChain, void* nativeHandle, uint32_t width, uint32_t height) = 0;
        virtual void CreateShader(std::unique_ptr<RendererResource>& shader, const std::vector<uint8_t>& code) const = 0;
        virtual void CreatePipeline(DeviceObject& object, const std::string& vs, const std::string& fs) const = 0;
        virtual void CreateCommandBuffers(const Pipeline& pipeline, Object3D& object) = 0;
        virtual void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) const = 0;
	};
    
    RENDERER_API std::unique_ptr<IRenderer> CreateRenderer();
    
    class RENDERER_API RendererLocator
    {
    public:
        static void Provide(std::unique_ptr<IRenderer> service)
        {
            mService = std::move(service);
        }
        
        static IRenderer& GetRenderer()
        {
            if(!mService)
            {
                throw std::runtime_error("Renderer unitialized!");
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
    
    class RENDERER_API Pipeline
    {
    public:
        void Create()
        {
            RendererLocator::GetRenderer().CreatePipeline(mDeviceObject, vertexShaderFile, fragmentShaderFile);
        }
        
    public:
        std::string vertexShaderFile;
        std::string fragmentShaderFile;
        
        DeviceObject mDeviceObject;
    };
}
