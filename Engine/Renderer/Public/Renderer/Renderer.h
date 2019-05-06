#pragma once
#include "RendererBase.h"

#include "VertexBuffer.h"
#include "Image.h"
#include "SwapChain.h"
#include "Effect.h"
#include "RenderPass.h"

#include <Math/Vector3.h>
#include <Math/Vector2.h>

#include <string>

#include <Renderer/Resources/Types.h>

namespace Renderer
{
	enum class DeviceType
	{
		External,
		Discrete,
		Integrated
	};

	enum class RenderBackend
	{
		Metal,
		Vulkan,
		OpenGL
	};
    
    class View;
    class Pipeline;
    class Object3d;
    class RenderPass;
    class Framebuffer;
    struct SemaphoreDescriptor;
    struct FenceDescriptor;
    struct EventDescriptor;
    struct SamplerDesc;
    struct RenderPassDescriptor;
    
    enum class CmdRecordResult
    {
        Success,
        RPFramebufferUnavailable,
        Failed,
    };

	class RENDERER_API IRenderer
	{
	public:
        virtual ~IRenderer() = default;
        
		virtual void Initialize() = 0;
		virtual void Deinitialize() = 0;

        virtual DeviceObject CreateSurface(void* nativeViewHandle) const = 0;
        virtual std::unique_ptr<SwapChainBase> CreateSwapChain(const DeviceObject& surface, const DeviceObject& renderPass, uint32_t width, uint32_t height) = 0;
        virtual void CreateShader(DeviceObject& shader, const std::vector<uint8_t>& code) const = 0;
        virtual void CreatePipeline(Pipeline& pipeline, const DeviceObject& renderPass) = 0;
        virtual void CreateBuffer(const BufferDesc& desc, DeviceObject& buffer) = 0;
        virtual void CreateFramebuffer(Framebuffer& desc, const RenderPass& renderPass) = 0;
        virtual DeviceObject CreateImage(const ImageDesc& desc) = 0;
        virtual void CreateTexture(const ImageDesc& desc, const SamplerDesc& samplerDesc, DeviceObject& texture) = 0;
        virtual DeviceObject CreateSemaphore(const SemaphoreDescriptor& desc) const = 0;
        virtual DeviceObject CreateFence(const FenceDescriptor& desc) const = 0;
        virtual DeviceObject CreateEvent(const EventDescriptor& desc) const = 0;
        virtual void MapMemory(const DeviceObject& deviceObject, uint32_t size, void* data) = 0;
        virtual void UnmapMemory(const DeviceObject& deviceObject) const = 0;
        virtual void CreateRenderPass(RenderPass& renderPass) const = 0;
        virtual void Render(const Object3d& vb, const Pipeline& pipeline) = 0;
        virtual void RenderGui(const VertexBufferBase& vb, const Pipeline& pipeline) = 0;
        
        // Release
        virtual void DestroyDeviceObject(DeviceObject& buffer) const = 0;
        
        
        // Command recording
        /*!
         @brief Enables recording of render & compute commands to primary command buffer.
         @return Result code of command recording.
         */
        virtual CmdRecordResult BeginCommandRecording() = 0;
        
        /*!
         @brief Starts new rendering pass.
         @param renderPass Pointer to the renderPass object.
         @param framebuffer Pointer to the framebuffer used for rendering.
         @return Result code of command recording.
         */
        virtual CmdRecordResult BeginRenderPass(const RenderPass& renderPass) = 0;
        
        virtual CmdRecordResult NextSubpass() = 0;
        
        
        virtual CmdRecordResult SetViewport(const Rectangle<float>& viewport) = 0;
        virtual CmdRecordResult SetScissor(const Rectangle<uint32_t>& scissor) = 0;
        
        /*!
         @brief Ends most recently started rendering pass.
         @return Result code of command recording.
         */
        virtual CmdRecordResult EndRenderPass() = 0;
        
        /*!
         @brief Disables recording of commands to primary command buffer.
         @return Result code of command recording.
         */
        virtual CmdRecordResult EndCommandRecording(SwapChainBase* swapChain) = 0;
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
    
    /*!
     @brief Enumerator of framebuffer attachment types.
     */
    enum class DepthStencilPropertyMask : uint8_t
    {
        Undefined = 0x00000000,
        Color = 0x00000001,
        Stencil = 0x00000002,
        Depth = 0x00000004
    };
    
    class RENDERER_API Pipeline
    {
    public:
        Effect effect;
        bool depthTestEnabled{ false };
        bool depthWriteEnabled{ false };
        bool useDepth{ false };
        
        DeviceObject mDeviceObject;
        Vector2f mViewPort;
        Vector2f mOffset;
        uint32_t mSubpassIndex{ 0 };
    };
}
