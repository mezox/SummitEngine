#pragma once

#include "Severity.h"

#include <string>

namespace Logging
{
    class AppenderBase
    {
    public:
        AppenderBase() = default;
        virtual ~AppenderBase() = default;
        
        AppenderBase(const AppenderBase& other) = delete;
        AppenderBase(AppenderBase&& other) noexcept = delete;
        AppenderBase& operator=(const AppenderBase& other) = delete;
        AppenderBase& operator=(AppenderBase&& other) noexcept = delete;
        
        void SetMinSeverity(const Severity severity);
        const Severity GetMinSeverity() const;
        
        virtual void Log(const std::string& msg, const Severity severity) const = 0;
        
    private:
        Severity mMinSeverity{ Severity::Information };
    };
}
