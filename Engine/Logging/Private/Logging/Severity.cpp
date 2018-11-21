#include <Logging/Severity.h>

namespace Logging
{
    const char* ToString(const Severity severity)
    {
        switch (severity)
        {
            case Severity::Information:
                return "Info";
            case Severity::Debug:
                return "Debug";
            case Severity::Warning:
                return "Warning";
            case Severity::Error:
                return "Error";
            case Severity::CriticalError:
                return "Critical";
            default:
                break;
        }
        
        return "Undefined";
    }
}
