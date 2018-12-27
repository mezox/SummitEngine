#include "EngineImpl.h"

#include <nlohmann/json.hpp>

#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include <PAL/FileSystem/FileSystemService.h>
#include <PAL/RenderAPI/RenderAPIService.h>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('C','O','R','E')

using json = nlohmann::json;

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
    FileSystemServiceLocator::Service()->Initialize();
    
    LoggingServiceLocator::Provide(CreateLoggingService());
    RenderAPIServiceLocator::Provide(CreateRenderAPI(RenderBackend::Vulkan));
}

void SummitEngine::Initialize()
{
    mWindow = std::make_unique<App::Window>("SummitEngine", 1280, 720);
    
    //LOG_INFORMATION("Created default window")
    
    //mSecondWindow = std::make_unique<App::Window>("Second", 640, 320);
    
    FileSystemServiceLocator::Service()->Initialize();
    
    LoggingServiceLocator::Service()->Initialize();
    
    RenderAPIServiceLocator::Service()->Initialize();
    
    RenderAPIServiceLocator::Service()->CreateDevice(DeviceType::Integrated);
    
    LOG(Information) << "Alleluja";
}

void SummitEngine::StartFrame()
{
}

void SummitEngine::Update()
{
}

void SummitEngine::EndFrame()
{
}

void SummitEngine::DeInitialize()
{
}
