#pragma once

#include "Severity.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>

namespace Logging
{
    class WriterBase;
    
    class Logger
    {
    public:
        Logger();
        ~Logger();
        
        void Log(const std::string& msg, const uint32_t channel, const Severity severity);
        
        void AddWriter(std::unique_ptr<WriterBase> writer);
        void Reset();
        
    private:
        std::mutex mMutex;
        std::vector<std::unique_ptr<WriterBase>> mWriters;
    };
}
