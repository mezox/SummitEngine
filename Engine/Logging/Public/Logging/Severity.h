#pragma once

#include <cstdint>

namespace Logging
{
    enum class Severity : uint8_t
    {
        Information,
        Debug,
        Warning,
        Error,
        CriticalError
    };
    
    const char* ToString(const Severity);
}
