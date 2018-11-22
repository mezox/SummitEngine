#include <Logging/FileWriter.h>

namespace Logging
{
    FileWriter::FileWriter(std::string format, std::string timeFormat)
        : WriterBase(std::move(format), std::move(timeFormat))
    {
        
    }
    
    void FileWriter::Log(const std::string &msg, const uint32_t channel, const Severity severity, const std::string& component)
    {
        
    }
}
