#include <Logging/pch.h>
#include <Logging/FileWriter.h>

namespace Logging
{
    FileWriter::FileWriter(std::string file, std::string format, std::string timeFormat)
        : WriterBase(std::move(format), std::move(timeFormat))
    {
        mLogFile = std::make_unique<PAL::FileSystem::File>(file);
        mLogFile->Open(PAL::FileSystem::EFileAccessMode::Write);
    }
    
    FileWriter::~FileWriter()
    {
        mLogFile->Close();
    }
    
    void FileWriter::Log(const std::string &msg, const uint32_t channel, const Severity severity, const std::string& component)
    {
        using namespace PAL::FileSystem;
        
        if(!mLogFile->IsOpened())
            return;
        
        const auto formattedMsg = FormatMessage(msg, channel, severity, component);
        
        if (mLogFile->Write((void*)formattedMsg.c_str(), formattedMsg.size()))
        {
            if (++mFlushTriggerLines > 1)
            {
                mLogFile->Flush();
                mFlushTriggerLines = 0;
            }
        }
    }
}
