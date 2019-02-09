#include "EngineImpl.h"

#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include <PAL/FileSystem/FileSystemService.h>
#include <PAL/FileSystem/File.h>
#include <PAL/RenderAPI/VulkanAPI.h>
#include <Event/EventService.h>
#include <Timer/TimerService.h>
#include "VulkanRendererImpl.h"

#include <microprofile/microprofile.h>

#include <Engine/Window.h>
#include <Engine/WindowEvent.h>
#include <Engine/Renderer.h>


#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('C','O','R','E')

MICROPROFILE_DEFINE(MAIN, "MAIN", "Main", 0xff0000);

using namespace Engine;
using namespace Logging;
using namespace Event;
using namespace Timer;
using namespace PAL::FileSystem;
using namespace PAL::RenderAPI;


std::shared_ptr<SummitEngine> EngineServiceLocator::mService = nullptr;

std::shared_ptr<SummitEngine> Engine::CreateEngineService()
{
	return std::make_shared<SummitEngine>();
}

SummitEngine::SummitEngine()
{
    FileSystemServiceLocator::Provide(CreateFileSystemService());
    FileSystemServiceLocator::Service().Initialize();
    LoggingServiceLocator::Provide(CreateLoggingService());
    EventServiceLocator::Provide(CreateEventService());
    TimerServiceLocator::Provide(CreateTimerService());
    
    VulkanAPIServiceLocator::Provide(CreateVulkanRenderAPI());
    mRenderer = std::make_unique<Renderer::VulkanRenderer>();
    
    mWindowResizeHandler = EventHandlerFunc(true, this, &SummitEngine::OnWindowResize);
    mWindowMoveHandler = EventHandlerFunc(true, this, &SummitEngine::OnWindowMove);
    
    MicroProfileOnThreadCreate("Main");
    MicroProfileSetEnableAllGroups(true);
    MicroProfileSetForceMetaCounters(true);
}

void SummitEngine::Initialize()
{        
    LoggingServiceLocator::Service().Initialize();
    VulkanAPIServiceLocator::Service().Initialize();
    mRenderer->Initialize();
    
    EventServiceLocator::Service().RegisterEventHandler(mWindowResizeHandler);
    EventServiceLocator::Service().RegisterEventHandler(mWindowMoveHandler);
}

Application::Window* SummitEngine::CreateWindow(const char* title, uint32_t width, uint32_t height) const
{
    auto windowPtr = new Application::Window(*mRenderer.get(), title, width, height);
    return windowPtr;
}

void SummitEngine::RegisterWindow(Application::Window* window)
{
    mWindows.push_back(window);
}

void SummitEngine::StartFrame()
{
}

void SummitEngine::Update()
{
    MicroProfileFlip(0);
//    MICROPROFILE_SCOPEI("SummitEngine", "Update", MP_GREEN3);
//    for(int i = 0; i < 100000; i++)
//    {
//        int* a = new int(5);
//        delete a;
//    }
    
    
    for(auto window : mWindows)
		window->Update();
}

void SummitEngine::EndFrame()
{
}

void SummitEngine::DeInitialize()
{
    mRenderer.reset();
    
    EventServiceLocator::Service().UnRegisterEventHandler(mWindowMoveHandler);
    EventServiceLocator::Service().UnRegisterEventHandler(mWindowResizeHandler);
    
    EventServiceLocator::Provide(nullptr);
    TimerServiceLocator::Provide(nullptr);
    
    MicroProfileShutdown();
}

Renderer::IRenderer& SummitEngine::GetRenderer()
{
    return *mRenderer.get();
}

void SummitEngine::Run()
{
    TimerServiceLocator::Service().Schedule(1000, [this]{ Update(); }, true);
}
                                                                       
void SummitEngine::OnWindowResize(const Application::WindowResizeEvent& event)
{
    LOG(Debug) << "SummitEngine: Window resized: " << event.width << ", " << event.height;
}

void SummitEngine::OnWindowMove(const Application::WindowMoveEvent& event)
{
    LOG(Debug) << "SummitEngine: Window moved: " << event.originX << ", " << event.originY;
}
