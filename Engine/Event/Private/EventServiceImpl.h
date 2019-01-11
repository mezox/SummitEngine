#pragma once

#include <Event/EventService.h>
#include <unordered_map>

namespace Event
{
    class EventServiceImpl : public IEventService
    {
    public:
        EventServiceImpl() = default;
        virtual ~EventServiceImpl() = default;
        
    protected:
        virtual void RegisterEventHandlerImpl(HandlerBase& h);
        virtual void UnRegisterEventHandler(const HandlerBase& h);
        virtual void FireEventImpl(size_t eventId, std::shared_ptr<IEvent>& pe) const;
        
    private:
        std::unordered_map<size_t, std::vector<const HandlerBase*>> mHandlers;
    };
}
