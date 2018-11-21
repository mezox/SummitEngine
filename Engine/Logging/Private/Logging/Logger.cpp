#include <Logging/Logger.h>
#include <Logging/Writer.h>

#include <iostream>
#include <algorithm>

namespace Logging
{
    Logger::Logger() = default;
    Logger::~Logger() = default;
    
    void Logger::Log(const std::string& msg, const uint32_t channel, const Severity severity)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for(auto& writer : mWriters)
        {
            writer->Log(msg, channel, severity);
        }
    }
    
    void Logger::AddWriter(std::unique_ptr<WriterBase> writer)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mWriters.push_back(std::move(writer));
    }
    
    void Logger::Reset()
    {
        mWriters.clear();
    }
}
