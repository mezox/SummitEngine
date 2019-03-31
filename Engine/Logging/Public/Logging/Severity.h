#pragma once

namespace Logging
{
    enum class Severity
    {
        Information,
        Debug,
        Warning,
        Error,
        CriticalError
    };
    
    const char* ToString(const Severity);
}
