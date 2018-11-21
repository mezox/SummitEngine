#pragma once

#include "Severity.h"

#include <string>

namespace Logging
{
    class WriterBase
    {
    public:
        WriterBase() = default;
        virtual ~WriterBase() = default;
        
        WriterBase(const WriterBase& other) = delete;
        WriterBase(WriterBase&& other) noexcept = delete;
        WriterBase& operator=(const WriterBase& other) = delete;
        WriterBase& operator=(WriterBase&& other) noexcept = delete;
        
        void SetMinSeverity(const Severity severity);
        const Severity GetMinSeverity() const;
        
        virtual void Log(const std::string& msg, const uint32_t channel, const Severity severity) const = 0;
        
    private:
        Severity mMinSeverity{ Severity::Information };
    };
}
