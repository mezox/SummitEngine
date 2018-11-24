#include <Logging/Logger.h>
#include <Logging/Writer.h>

#include <iostream>
#include <algorithm>

namespace Logging
{
    Logger::Logger(std::string name)
        : mName(std::move(name))
    {
    }
    
    Logger::~Logger()
    {
        Reset();
    }
    
    void Logger::Log(const std::string& msg, const uint32_t channel, const Severity severity)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        for(auto& writer : mWriters)
        {
            writer->Log(msg, channel, severity, mName);
        }
    }
    
    const std::string& Logger::GetName() const noexcept
    {
        return mName;
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
