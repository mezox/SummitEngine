#include "EngineImpl.h"

#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include <PAL/FileSystem/FileSystemService.h>
#include <PAL/RenderAPI/VulkanAPI.h>
#include <Event/EventService.h>
#include <Timer/TimerService.h>
#include "VulkanRendererImpl.h"

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('C','O','R','E')

using namespace Engine;
using namespace Logging;
using namespace Event;
using namespace Timer;
using namespace PAL::FileSystem;
using namespace PAL::RenderAPI;


std::shared_ptr<IEngine> EngineServiceLocator::mService = nullptr;

std::shared_ptr<IEngine> Engine::CreateEngineService()
{
	return std::make_shared<SummitEngine>();
}

SummitEngine::SummitEngine()
{
    // Initialize platform abstraction layers
    FileSystemServiceLocator::Provide(CreateFileSystemService());
    FileSystemServiceLocator::Service().Initialize();
    LoggingServiceLocator::Provide(CreateLoggingService());
    EventServiceLocator::Provide(CreateEventService());
    TimerServiceLocator::Provide(CreateTimerService());
    
    VulkanAPIServiceLocator::Provide(CreateVulkanRenderAPI());
	Renderer::RendererServiceLocator::Provide(Renderer::CreateRenderer());
    
    mWindowResizeHandler = EventHandlerFunc(true, this, &SummitEngine::OnWindowResize);
    mWindowMoveHandler = EventHandlerFunc(true, this, &SummitEngine::OnWindowMove);
}

void SummitEngine::Initialize()
{        
    LoggingServiceLocator::Service().Initialize();
    VulkanAPIServiceLocator::Service().Initialize();
	Renderer::RendererServiceLocator::Service().Initialize();
    
    EventServiceLocator::Service().RegisterEventHandler(mWindowResizeHandler);
    EventServiceLocator::Service().RegisterEventHandler(mWindowMoveHandler);

	mWindow = std::make_unique<App::Window>("SummitEngine", 1280, 720);
}

void SummitEngine::StartFrame()
{
}

void SummitEngine::Update()
{
	if(mWindow)
		mWindow->Update();

	if(mSecondWindow)
		mSecondWindow->Update();
}

void SummitEngine::EndFrame()
{
}

void SummitEngine::DeInitialize()
{
    EventServiceLocator::Service().UnRegisterEventHandler(mWindowMoveHandler);
    EventServiceLocator::Service().UnRegisterEventHandler(mWindowResizeHandler);
    
    EventServiceLocator::Provide(nullptr);
    TimerServiceLocator::Provide(nullptr);
}
                                                                       
void SummitEngine::OnWindowResize(const App::WindowResizeEvent& event)
{
    LOG(Debug) << "SummitEngine: Window resized: " << event.width << ", " << event.height;
}

void SummitEngine::OnWindowMove(const App::WindowMoveEvent& event)
{
    LOG(Debug) << "SummitEngine: Window moved: " << event.originX << ", " << event.originY;
}
