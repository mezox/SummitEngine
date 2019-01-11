#include <Event/EventHandler.h>

namespace Event
{
    HandlerBase::HandlerBase(const bool sync)
        : mExecuteSync(sync)
        , mEventId(0)
    {}
    
    const size_t HandlerBase::GetEventId() const
    {
        return mEventId;
    }
        
    void HandlerBase::Execute(std::shared_ptr<IEvent>& e) const
    {
        if (mExecuteSync)
        {
            Callback(e);
        }
        else
        {
            std::thread t(&HandlerBase::CallbackAsync, this, e);
            t.detach();
        }
    }
        
    void HandlerBase::CallbackAsync(std::shared_ptr<IEvent> e) const
    {
        Callback(e);
    }
}
