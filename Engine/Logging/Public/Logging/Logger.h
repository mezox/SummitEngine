#pragma once

#include "LoggingBase.h"
#include "Severity.h"

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <sstream>

#define SE_DEFAULT_MSG_FORMAT "%DTI [%MID:%SEV] \"%MNM\" %MSG"
#define SE_DEFAULT_TIME_FORMAT "%d-%m-%Y %H:%M:%S"

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
    
    /*
     * Simple RAII message composer
     */
    class LOGGING_API Composer
    {
    public:
        Composer(Logger& logger, const uint32_t channel, const uint32_t module, const Severity severity);
        virtual ~Composer();
        
        template<typename T>
        Composer& operator<<(const T& msg)
        {
            mOutputStream << msg;
            return *this;
        }
        
        /**
         * Special handling for C-style strings
         */
        Composer& operator<<(const char* pch)
        {
            mOutputStream << pch;
            return *this;
        }
        
    private:
        Logger& mLogger;
        uint32_t mChannel{ 0 };
        uint32_t mModule{ 0 };
        Severity mSeverity{ Severity::Information };
        std::stringstream mOutputStream;
    };
}
