#pragma once

#include "LoggingBase.h"
#include "Severity.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <sstream>

namespace Logging
{
    class WriterBase;
    
    class LOGGING_API Logger
    {
    public:
        Logger(std::string name);
        ~Logger();
        
        void Log(const std::string& msg, const uint32_t channel, const Severity severity);
        
        void AddWriter(std::unique_ptr<WriterBase> writer);
        void Reset();
        
        const std::string& GetName() const noexcept;
        
    private:
        std::mutex mMutex;
        std::string mName;
        std::vector<std::unique_ptr<WriterBase>> mWriters;
    };
}
