#pragma once

#include <Engine/Engine.h>
#include <Event/EventHandler.h>
#include "Window.h"
#include "WindowEvent.h"

namespace Engine
{
    class EngineUpdateEvent : public Event::IEvent
    {
    public:
    };
    
    class EngineRenderEvent : public Event::IEvent
    {
    public:
    };
    
	class SummitEngine : public IEngine
	{
	public:
		SummitEngine();

        virtual void Initialize() override;
		virtual void StartFrame() override;
		virtual void Update() override;
		virtual void EndFrame() override;
        virtual void DeInitialize() override;
        
    private:
        void OnWindowResize(const App::WindowResizeEvent& event);
        void OnWindowMove(const App::WindowMoveEvent& event);
        
    private:
        std::unique_ptr<App::Window> mWindow;
        std::unique_ptr<App::Window> mSecondWindow;

        Event::EventHandlerFunc<SummitEngine, App::WindowResizeEvent> mWindowResizeHandler;
        Event::EventHandlerFunc<SummitEngine, App::WindowMoveEvent> mWindowMoveHandler;
	};
}
