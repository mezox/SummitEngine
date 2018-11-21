#include <Logging/Logger.h>

#include <iostream>

namespace Logging
{
    void Logger::Log(const std::string& msg, const Severity severity)
    {
        std::cout << "[" << ToString(severity) << "] " << msg << "\n";
    }
}
