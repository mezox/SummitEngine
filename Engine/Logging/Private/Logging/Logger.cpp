#include <Logging/Logger.h>
#include <Logging/Appender.h>

#include <iostream>
#include <algorithm>

namespace Logging
{
    Logger::Logger() = default;
    Logger::~Logger() = default;
    
    void Logger::Log(const std::string& msg, const Severity severity)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for(auto& appender : mAppenders)
        {
            appender->Log(msg, severity);
        }
    }
    
    void Logger::AddAppender(std::unique_ptr<AppenderBase> appender)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mAppenders.push_back(std::move(appender));
    }
    
    void Logger::Reset()
    {
        mAppenders.clear();
    }
}
