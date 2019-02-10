#pragma once

#include "WindowEvent.h"
#include <Engine/EngineBase.h>
#include <Event/EventHandler.h>
    
#include <vector>
    
namespace Application
{
    class Window;
}

namespace Renderer
{
    class IRenderer;
}
    
namespace Engine
{
    class ENGINE_API SummitEngine
    {
    public:
        SummitEngine();
        ~SummitEngine() {}
        
        Application::Window* CreateWindow(const char* title, uint32_t width, uint32_t height) const;
        void RegisterWindow(Application::Window* window);
        
        void Initialize();
        void StartFrame();
        void Update();
        void EndFrame();
        void DeInitialize();
        
        void Run();
        
    private:
        void OnWindowResize(const Application::WindowResizeEvent& event);
        void OnWindowMove(const Application::WindowMoveEvent& event);
        
    private:
        std::vector<Application::Window*> mWindows;
        
        Event::EventHandlerFunc<SummitEngine, Application::WindowResizeEvent> mWindowResizeHandler;
        Event::EventHandlerFunc<SummitEngine, Application::WindowMoveEvent> mWindowMoveHandler;
    };

    ENGINE_API std::shared_ptr<SummitEngine> CreateEngineService();

	class ENGINE_API EngineServiceLocator
	{
	public:
		static void Provide(std::shared_ptr<SummitEngine> service)
		{
			mService = std::move(service);
		}

		static std::shared_ptr<SummitEngine> Service()
		{
			return mService;
		}

		static bool Available()
		{
			return !mService;
		}

	private:
		static std::shared_ptr<SummitEngine> mService;
	};
}
