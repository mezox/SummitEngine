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
        VertexBufferPTCI<Vector3f, Vector2f, Vector3f, uint32_t> mVertexBuffer;
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
    struct RenderPassDescriptor;

	class RENDERER_API IRenderer
	{
	public:
        virtual ~IRenderer() = default;
        
		virtual void Initialize() = 0;
		virtual void Deinitialize() = 0;

        virtual DeviceObject CreateSurface(void* nativeViewHandle) const = 0;
        virtual void CreateSwapChain(std::unique_ptr<SwapChainBase>& swapChain, const DeviceObject& surface, uint32_t width, uint32_t height) = 0;
        virtual void CreateShader(DeviceObject& shader, const std::vector<uint8_t>& code) const = 0;
        virtual void CreatePipeline(Pipeline& pipeline) = 0;
        virtual void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) = 0;
        virtual void CreateFramebuffer(const FramebufferDesc& desc, DeviceObject& framebuffer) = 0;
        virtual void CreateImage(const ImageDesc& desc, DeviceObject& image) = 0;
        virtual void CreateSampler(const SamplerDesc& desc, DeviceObject& sampler) = 0;
        virtual void CreateTexture(const ImageDesc& desc, const SamplerDesc& samplerDesc, DeviceObject& texture) = 0;
        virtual DeviceObject CreateSemaphore(const SemaphoreDescriptor& desc) const = 0;
        virtual DeviceObject CreateFence(const FenceDescriptor& desc) const = 0;
        virtual DeviceObject CreateEvent(const EventDescriptor& desc) const = 0;
        virtual void MapMemory(const DeviceObject& deviceObject, uint32_t size, void* data) = 0;
        virtual void UnmapMemory(const DeviceObject& deviceObject) const = 0;
        virtual void CreateRenderPass(const RenderPassDescriptor& desc, DeviceObject& deviceObject) const = 0;
        virtual void Render(const VertexBufferBase& vb, const Pipeline& pipeline) = 0;
        virtual void RenderGui(const VertexBufferBase& vb, const Pipeline& pipeline) = 0;
        
        // Release
        virtual void DestroyDeviceObject(DeviceObject& buffer) const = 0;
        
        virtual void BeginCommandRecording(SwapChainBase* swapChain) = 0;
        virtual void EndCommandRecording(SwapChainBase* swapChain) = 0;
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
        bool depthTestEnabled{ false };
        
        DeviceObject mDeviceObject;
    };
}
