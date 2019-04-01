#pragma once
#include "RendererBase.h"

#include "VertexBuffer.h"
#include "Image.h"
#include "SwapChain.h"
#include "Effect.h"

#include <Math/Vector3.h>
#include <Math/Vector2.h>

#include <string>

namespace Renderer
{
	enum class DeviceType
	{
		External,
		Discrete,
		Integrated
	};
    
    class RENDERER_API FramebufferDesc
    {
    public:
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        void* data{ nullptr };
    };
    
    class RENDERER_API Object3D
    {
    public:
        //VertexBufferPCI<Vector3f, Vector3f, uint16_t> mVertexBuffer;
        VertexBufferPTCI<Vector3f, Vector2f, Vector3f, uint16_t> mVertexBuffer;
    };

	enum class RenderBackend
	{
		Metal,
		Vulkan,
		OpenGL
	};
    
    class Pipeline;
    struct SemaphoreDescriptor;
    struct FenceDescriptor;
    struct EventDescriptor;

	class RENDERER_API IRenderer
	{
	public:
        virtual ~IRenderer() = default;
        
		virtual void Initialize() = 0;
		virtual void Deinitialize() = 0;

        virtual void CreateSwapChain(std::unique_ptr<SwapChainBase>& swapChain, void* nativeHandle, uint32_t width, uint32_t height) = 0;
        virtual void CreateShader(DeviceObject& shader, const std::vector<uint8_t>& code) const = 0;
        virtual void CreatePipeline(Pipeline& pipeline) = 0;
        virtual void CreateCommandBuffers(const Pipeline& pipeline, Object3D& object) = 0;
        virtual void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) = 0;
        virtual void CreateFramebuffer(const FramebufferDesc& desc, DeviceObject& framebuffer) = 0;
        virtual void CreateImage(const ImageDesc& desc, DeviceObject& image) = 0;
        virtual void CreateSampler(const SamplerDesc& desc, DeviceObject& sampler) = 0;
        virtual void CreateTexture(const ImageDesc& desc, const SamplerDesc& samplerDesc, DeviceObject& texture) = 0;
        virtual void CreateSemaphore(const SemaphoreDescriptor& desc, DeviceObject& semaphore) const = 0;
        virtual void CreateFence(const FenceDescriptor& desc, DeviceObject& fence) const = 0;
        virtual void CreateEvent(const EventDescriptor& desc, DeviceObject& event) const = 0;
        
        virtual void MapMemory(const DeviceObject& deviceObject, uint32_t size, void* data) = 0;
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
    
    struct PipelineKey
    {
        
    };
    
    class RENDERER_API Pipeline
    {
    public:
        void Create()
        {
            RendererLocator::GetRenderer().CreatePipeline(*this);
        }
        
    public:
        Effect effect;
        
        DeviceObject mDeviceObject;
    };
}
