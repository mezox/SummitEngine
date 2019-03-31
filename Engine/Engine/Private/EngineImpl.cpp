#include <Engine/Engine.h>
#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include <PAL/FileSystem/FileSystemService.h>
#include <PAL/FileSystem/File.h>
#include <PAL/RenderAPI/VulkanAPI.h>
#include <Dispatcher/SummitDispatcher.h>

#include <microprofile/microprofile.h>

#include <Renderer/Renderer.h>

#include <Engine/Application.h>


#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('C','O','R','E')

MICROPROFILE_DEFINE(MAIN, "MAIN", "Main", 0xff0000);

using namespace Summit;
using namespace Renderer;
using namespace Logging;
using namespace Core;
using namespace PAL::FileSystem;
using namespace PAL::RenderAPI;

std::shared_ptr<SummitEngine> EngineServiceLocator::mService = nullptr;

std::shared_ptr<SummitEngine> Summit::CreateEngineService()
{
	return std::make_shared<SummitEngine>();
}

SummitEngine::SummitEngine()
{
    FileSystemServiceLocator::Provide(CreateFileSystemService());
    FileSystemServiceLocator::Service().Initialize();
    LoggingServiceLocator::Provide(CreateLoggingService());
    Core::DispatcherService::Provide(CreateSummitDispatcher());
    
    VulkanAPIServiceLocator::Provide(CreateVulkanRenderAPI());
    Renderer::RendererLocator::Provide(CreateRenderer());
    
    MicroProfileOnThreadCreate("Main");
    MicroProfileSetEnableAllGroups(true);
    MicroProfileSetForceMetaCounters(true);
}

void SummitEngine::Initialize()
{        
    LoggingServiceLocator::Service().Initialize();
    VulkanAPIServiceLocator::Service().Initialize();
    Renderer::RendererLocator::GetRenderer().Initialize();
}

void SummitEngine::StartFrame()
{
}

void SummitEngine::Update()
{
    Updatee({});
    
    MicroProfileFlip(0);
//    MICROPROFILE_SCOPEI("SummitEngine", "Update", MP_GREEN3);
//    for(int i = 0; i < 100000; i++)
//    {
//        int* a = new int(5);
//        delete a;
//    }
    
//    for(auto window : mWindows)
//    {
//        window->Update();
//        
//    }
}

void SummitEngine::EndFrame()
{
}

void SummitEngine::DeInitialize()
{
    Renderer::RendererLocator::GetRenderer().Deinitialize();
    Core::DispatcherService::Provide(nullptr);
    
    MicroProfileShutdown();
}

void SummitEngine::Run()
{
    Core::DispatcherService::Service().Schedule(1000, [this]{ Update(); }, true);
}
