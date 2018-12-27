#pragma once

#include <Logging/LoggingBase.h>
#include "Writer.h"

namespace Logging
{
    class LOGGING_API ConsoleWriter : public WriterBase
    {
    public:
        ConsoleWriter(std::string format, std::string timeFormat);
        
        void Log(const std::string& msg, const uint32_t channel, const Severity severity, const std::string& component) override;
    };
}
