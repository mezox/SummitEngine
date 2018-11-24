#include <Logging/LoggingServiceImpl.h>
#include <Logging/Logger.h>

using namespace Logging;

std::shared_ptr<ILoggingService> LoggingServiceLocator::mService = nullptr;

std::shared_ptr<ILoggingService> Logging::CreateLoggingService()
{
    return std::make_shared<LoggingServiceImpl>();
}

LoggingServiceImpl::LoggingServiceImpl()
{
    
}

LoggingServiceImpl::~LoggingServiceImpl()
{
    
}

Logger* LoggingServiceImpl::GetLogger(const std::string& name) const
{
    auto result = std::find_if(mLoggers.begin(), mLoggers.end(), [&name](const auto& logger){
        return logger.second->GetName() == name;
    });
    
    if(result != mLoggers.end())
    {
        return result->second.get();
    }
    
    return nullptr;
}

void LoggingServiceImpl::AddLogger(std::unique_ptr<Logger> logger)
{
    const auto& loggerName = logger->GetName();
    auto result = std::find_if(mLoggers.begin(), mLoggers.end(), [&loggerName](const auto& logger){
        return logger.second->GetName() == loggerName;
    });
    
    if(result != mLoggers.end())
    {
        mLoggers[loggerName].swap(logger);
    }
    else
    {
        mLoggers[loggerName] = std::move(logger);
    }
}

void LoggingServiceImpl::RemoveLogger(const std::string& name)
{
//    std::remove_if(mLoggers.begin(), mLoggers.end(), [&name](auto& logger) {
//        return logger.second->GetName() == name;
//    });
}
