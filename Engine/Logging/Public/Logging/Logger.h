#pragma once

#include "Severity.h"

#include <string>

namespace Logging
{
    class Logger
    {
    public:
        static void Log(const std::string& msg, const Severity severity);
    };
}
