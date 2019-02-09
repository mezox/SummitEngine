#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>
#include <Core/Service.h>
#include <Engine/Engine.h>

#include <Engine/VertexBuffer.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>

#include <string>

namespace Renderer
{
    enum class ImgFormat
    {
        
    };
    
	class RendererResource
	{
	public:
		virtual ~RendererResource() = default;
	};
    
    class SwapChainResource : public RendererResource
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

	class RENDERAPI_API IDevice
	{
	public:
		virtual ~IDevice() = default;
	};
    
    class RENDERAPI_API Object3D
    {
    public:
        VertexBufferPCI<Vector2f, Vector3f, uint16_t> mVertexBuffer;
    };

	enum class RenderBackend
	{
		Metal,
		Vulkan,
		OpenGL
	};
    
    class Pipeline;

	class IRenderer
	{
	public:
		virtual void Initialize() = 0;
		virtual void Deinitialize() = 0;
        
        virtual void SetImageFormat(ImgFormat format) = 0;

        virtual void CreateSwapChain(std::unique_ptr<SwapChainResource>& swapChain, void* nativeHandle, uint32_t width, uint32_t height) = 0;
        virtual void CreateShader(std::unique_ptr<RendererResource>& shader, const std::vector<uint8_t>& code) const = 0;
        virtual void CreatePipeline(std::unique_ptr<RendererResource>& pipeline, const std::string& vs, const std::string& fs) const = 0;
        virtual void CreateCommandBuffers(const Pipeline& pipeline, Object3D& object) = 0;
        virtual void CreateBuffer(const BufferDesc& desc, std::unique_ptr<RendererResource>& buffer) const = 0;

	private:
	};
    
    class Pipeline
    {
    public:
        void Create()
        {
            Engine::EngineServiceLocator::Service()->GetRenderer().CreatePipeline(pipelineResource, vertexShaderFile, fragmentShaderFile);
        }
        
    public:
        std::string vertexShaderFile;
        std::string fragmentShaderFile;
        
        std::unique_ptr<Renderer::RendererResource> pipelineResource;
    };
}
