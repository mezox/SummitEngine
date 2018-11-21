#include <Logging/Writer.h>
#include <Logging/Severity.h>

namespace Logging
{    
    void WriterBase::SetMinSeverity(const Severity severity)
    {
        mMinSeverity = severity;
    }
    
    const Severity WriterBase::GetMinSeverity() const
    {
        return mMinSeverity;
    }
}
