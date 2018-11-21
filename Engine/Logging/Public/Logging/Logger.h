#pragma once

#include "Severity.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>

namespace Logging
{
    class AppenderBase;
    
    class Logger
    {
    public:
        Logger();
        ~Logger();
        
        void Log(const std::string& msg, const Severity severity);
        
        void AddAppender(std::unique_ptr<AppenderBase> appender);
        void Reset();
        
    private:
        std::mutex mMutex;
        std::vector<std::unique_ptr<AppenderBase>> mAppenders;
    };
}
