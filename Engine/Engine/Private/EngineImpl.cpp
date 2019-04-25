#include <Engine/Engine.h>

#include <Core/Templates.h>
#include <Logging/LoggingService.h>
#include <Logging/Logger.h>

#include <PAL/FileSystem/FileSystemService.h>
#include <PAL/FileSystem/File.h>
#include <PAL/RenderAPI/Vulkan/VulkanAPI.h>
#include <Dispatcher/SummitDispatcher.h>

#include <microprofile/microprofile.h>

#include <Renderer/Renderer.h>
#include <Renderer/View.h>

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

std::shared_ptr<SummitEngine> EngineService::mService = nullptr;

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
    
    VulkanAPI::Provide(CreateVulkanRenderAPI());
    Renderer::RendererLocator::Provide(CreateRenderer());
    
    MicroProfileOnThreadCreate("Main");
    MicroProfileSetEnableAllGroups(true);
    MicroProfileSetForceMetaCounters(true);
    
    mRenderer = &RendererLocator::GetRenderer();
}

void SummitEngine::Initialize()
{        
    LoggingServiceLocator::Service().Initialize();
    VulkanAPI::Service().Initialize();
    
    mRenderer->Initialize();
}

void SummitEngine::StartFrame()
{
    MicroProfileFlip(0);
    
    EarlyUpdate(mFrameData);
    
    mActiveSwapChain->AcquireImage();
    
    mFrameData.deltaTime = 0.001f;
    mFrameData.width = mActiveSwapChain->GetActiveFramebuffer().GetWidth();
    mFrameData.height = mActiveSwapChain->GetActiveFramebuffer().GetHeight();
    
    //mGui->StartFrame(mFrameData);
}

void SummitEngine::Update()
{
    StartFrame();
    
    // Begin update phase
    Updatee({});
    
    // --------- RENDER PHASE -------------
    mRenderer->BeginCommandRecording(mActiveSwapChain);
    //mGui->FinishFrame();
    
    Render({});
    
    mRenderer->EndCommandRecording(mActiveSwapChain);
    // --------- END OF RENDER PHASE -------------
    
    EndFrame();
}

void SummitEngine::EndFrame()
{
    ScopedIncrement<uint32_t> frameId(mFrameId);
    mActiveSwapChain->SwapBuffers();
}

void SummitEngine::DeInitialize()
{
    // Destroy default render pass
    
    Renderer::RendererLocator::GetRenderer().Deinitialize();
    //Core::DispatcherService::Provide(nullptr);
    VulkanAPI::Service().DeInitialize();
    
    MicroProfileShutdown();
}

void SummitEngine::SetActiveSwapChain(Renderer::SwapChainBase* swapChain)
{
    mActiveSwapChain = swapChain;
}

void SummitEngine::RenderObject(Object3D& object, Renderer::Pipeline& pipeline)
{
    mRenderer->Render(object.mVertexBuffer, pipeline);
    
    //mRenderer->RenderGui(mGui->mVertexBuffer, mGui->mGuiPipeline);
}

void SummitEngine::Run()
{
    Core::DispatcherService::Service().Schedule(10000, [this]{ Update(); }, true);
}

void SummitEngine::SetMainView(Renderer::View* view)
{
    mGui = std::make_unique<UI::Gui>(*view);
}
