#pragma once

#include <Engine/EngineBase.h>
#include <Event/Signal.h>
#include "Gui.h"

#include <Renderer/View.h>
#include <Renderer/RenderPass.h>

#include <Renderer/DeviceObject.h>

namespace Renderer
{
    class View;
    class Object3D;
    class IRenderer;
    class SwapChainBase;
}

namespace Summit
{
    /*!
     * @brief Single render frame data.
     */
    struct ENGINE_API FrameData
    {
        /*!
         * @brief Time since last frame in miliseconds.
         */
        float deltaTime{ 0.0f };
        
        /*!
         * @brief Current view width in pixels.
         */
        float width{ 0.0f };
        
        /*!
         * @brief Current view height in pixels.
         */
        float height{ 0.0f };
        
        
        uint32_t acquiredImageIndex{ 0 };
        Renderer::DeviceObject imageAvailableSemaphore;
        Renderer::DeviceObject renderFinishedSemaphore;
    };
    
    class ENGINE_API SummitEngine
    {
    public:
        SummitEngine();
        ~SummitEngine() {}
        
        void Initialize();
        void DeInitialize();
        
        void RegisterRenderPass(Renderer::RenderPass& renderPass);
        
        void RenderObject(Renderer::Object3d& object, Renderer::Pipeline& pipeline);
        void SetActiveSwapChain(Renderer::SwapChainBase* swapChain);
        
        void Run();
        
        void SetMainView(Renderer::View* view);
        
        Renderer::IRenderer& GetRenderer() const { return *mRenderer; }
        
    public:
        sigslot::signal<const FrameData&> EarlyUpdate;
        sigslot::signal<const FrameData&> Updatee;
        sigslot::signal<const FrameData&> LateUpdate;
        
    private:
        void StartFrame();
        void Update();
        void EndFrame();
        
    private:
        uint32_t mFrameId{ 0 };
        
        Renderer::IRenderer* mRenderer{ nullptr };
        Renderer::SwapChainBase* mActiveSwapChain{ nullptr };
        
        std::unique_ptr<UI::Gui> mGui;
        
        FrameData mFrameData;
        
        std::vector<Renderer::RenderPass*> mRenderPasses;
    };

    ENGINE_API std::shared_ptr<SummitEngine> CreateEngineService();

	class ENGINE_API EngineService
	{
	public:
		static void Provide(std::shared_ptr<SummitEngine> service)
		{
			mService = std::move(service);
		}

		static SummitEngine& Get()
		{
            if(!mService)
            {
                throw std::runtime_error("Attempt to get unitialized engine service!");
            }
            
			return *mService;
		}

		static bool Available()
		{
			return !mService;
		}

	private:
		static std::shared_ptr<SummitEngine> mService;
	};
}
