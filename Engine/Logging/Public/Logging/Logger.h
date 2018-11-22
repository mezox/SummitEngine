#pragma once

#include "Severity.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <sstream>

#define MLOG_SINIT(componentName) \
namespace Logging \
{ \
    Logger& Logger::getLogger() \
    { \
        static Logger lg(componentName); \
        return lg; \
    } \
}

#define LOG_MSG_MODULE(m, id, severity) \
        std::ostringstream ostr; \
        ostr << m << std::endl; \
        Logging::Logger::getLogger().Log(ostr.str(), id, severity);

#define LOG_INFORMATION(m) LOG_MSG_MODULE(m, LOG_MODULE_ID, Logging::Severity::Information)

/// 4 byte composed of chars can be used to identify module like id = LOG_MODULE_4BYTE('C','O','R','E')
#define LOG_MODULE_4BYTE(b1,b2,b3,b4) ((int)(b1) + ((int)(b2) << 8) + ((int)(b3) << 16) + ((int)(b4) << 24))

/// Logger macros
#ifndef LOG_MODULE_ALL
#define LOG_MODULE_ALL 0
#endif

/// Default module id, can be re-defined for each module. Define only if not previously defined!
#ifndef LOG_MODULE_ID
#define LOG_MODULE_ID LOG_MODULE_ALL
#endif

namespace Logging
{
    class WriterBase;
    
    class Logger
    {
    public:
        void Log(const std::string& msg, const uint32_t channel, const Severity severity);
        
        static Logger& getLogger();
        
        void AddWriter(std::unique_ptr<WriterBase> writer);
        void Reset();
        
    private:
        Logger(const std::string& componentName);
        ~Logger();
        
    private:
        std::mutex mMutex;
        std::string mComponentName;
        std::vector<std::unique_ptr<WriterBase>> mWriters;
    };
}
