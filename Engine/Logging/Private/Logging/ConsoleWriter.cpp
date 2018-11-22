#include <Logging/ConsoleWriter.h>

#include <iostream>

namespace Logging
{
    ConsoleWriter::ConsoleWriter(std::string format, std::string timeFormat)
        : WriterBase(std::move(format), std::move(timeFormat))
    {
        
    }
    
    void ConsoleWriter::Log(const std::string &msg, const uint32_t channel, const Severity severity, const std::string& component)
    {
        WriteMessage(std::cout, msg, channel, severity, component);
    }
}
