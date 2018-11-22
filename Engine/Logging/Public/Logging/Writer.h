#pragma once

#include "Severity.h"
#include "Tokens.h"

#include <string>
#include <cstdint>
#include <ostream>
#include <vector>

namespace Logging
{
    union ModuleId
    {
        uint32_t uid;
        struct
        {
            char b0;
            char b1;
            char b2;
            char b3;
        } cid;
    };
    
    class WriterBase
    {
    public:
        WriterBase(std::string format, std::string timeFormat);
        virtual ~WriterBase() = default;
        
        WriterBase(const WriterBase& other) = delete;
        WriterBase(WriterBase&& other) noexcept = delete;
        WriterBase& operator=(const WriterBase& other) = delete;
        WriterBase& operator=(WriterBase&& other) noexcept = delete;
        
        void SetMinSeverity(const Severity severity);
        const Severity GetMinSeverity() const;
        
        virtual void Log(const std::string& msg, const uint32_t channel, const Severity severity, const std::string& component) = 0;
        
    protected:
        void WriteMessage(std::ostream& stream, const std::string& msg, const uint32_t channel, const Severity severity, const std::string& component);
        void WriteSeverity(std::ostream& stream, const Severity severity);
        void WriteModuleId(std::ostream& stream, const uint32_t moduleId);
        void WriteTimeStamp(std::ostream& stream);
        
        void ParseTokens(const std::string& format);
        
        
    private:
        std::string mFormat;            // TODO: Get rid of this.
        std::string mTimeFormat;        // TODO: Get rid of this.
        std::vector<Token> mTokens;
        std::vector<std::string> mFormatStrings;
        Severity mMinSeverity{ Severity::Information };
    };
}
