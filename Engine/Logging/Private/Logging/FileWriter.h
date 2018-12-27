#pragma once

#include "Writer.h"
#include <PAL/FileSystem/File.h>

namespace Logging
{    
    class FileWriter : public WriterBase
    {
    public:
        FileWriter(std::string file, std::string format, std::string timeFormat);
        ~FileWriter();
        
        void Log(const std::string& msg, const uint32_t channel, const Severity severity, const std::string& component) override;
        
    private:
        std::unique_ptr<PAL::FileSystem::File> mLogFile;
        uint32_t mFlushTriggerLines{ 1 };
    };
}
