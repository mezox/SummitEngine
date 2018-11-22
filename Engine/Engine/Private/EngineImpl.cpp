#include "EngineImpl.h"

#include <Logging/Logger.h>

MLOG_SINIT("Engine")

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('C','O','R','E')

using namespace Engine;

std::shared_ptr<IEngine> EngineServiceLocator::mService = nullptr;

std::shared_ptr<IEngine> Engine::CreateEngineService()
{
	return std::make_shared<SummitEngine>();
}

SummitEngine::SummitEngine()
{
    LOG_INFORMATION("Created Engine!")
}

void SummitEngine::Initialize()
{
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
