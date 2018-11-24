#include "EngineImpl.h"

#include <Logging/LoggingService.h>
#include <Logging/Logger.h>
#include <Logging/ConsoleWriter.h>

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
    Logging::LoggingServiceLocator::Provide(Logging::CreateLoggingService());
    
    auto consoleWriterService = std::make_unique<Logging::ConsoleWriter>("", "");
    auto engineLogger = std::make_unique<Logging::Logger>("Engine");
    engineLogger->AddWriter(std::move(consoleWriterService));
    Logging::LoggingServiceLocator::Service()->AddLogger(std::move(engineLogger));
    
    LOG_INFORMATION("Created SummitEngine!")
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
