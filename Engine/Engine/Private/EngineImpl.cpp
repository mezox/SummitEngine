#include "EngineImpl.h"

#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include <PAL/FileSystem/FileSystemService.h>
#include <PAL/RenderAPI/VulkanAPI.h>
#include "VulkanRendererImpl.h"

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('C','O','R','E')

using namespace Engine;
using namespace Logging;
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
    VulkanAPIServiceLocator::Provide(CreateVulkanRenderAPI());
	Renderer::RendererServiceLocator::Provide(Renderer::CreateRenderer());
}

void SummitEngine::Initialize()
{        
    LoggingServiceLocator::Service().Initialize();
    VulkanAPIServiceLocator::Service().Initialize();
	Renderer::RendererServiceLocator::Service().Initialize();

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
}
