#include <Logging/Appender.h>

namespace Logging
{
    void AppenderBase::SetMinSeverity(const Severity severity)
    {
        mMinSeverity = severity;
    }
    
    const Severity AppenderBase::GetMinSeverity() const
    {
        return mMinSeverity;
    }
}
