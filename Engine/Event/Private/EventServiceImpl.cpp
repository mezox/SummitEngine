#include "EventServiceImpl.h"

#include <Logging/LoggingService.h>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('E','V','N','T')

namespace Event
{
    std::unique_ptr<IEventService> EventServiceLocator::mService{ nullptr };

    std::unique_ptr<IEventService> CreateEventService()
    {
        return std::make_unique<EventServiceImpl>();
    }

    void EventServiceImpl::RegisterEventHandlerImpl(HandlerBase& h)
    {
        if (h.GetEventId())
        {
            auto ret = mHandlers.insert(std::make_pair(h.GetEventId(), std::vector<const HandlerBase*>()));
            ret.first->second.push_back(&h);
        }
        else
        {
            LOG(Error) << "EventService: Failed to register event handler!";
        }
    }

    void EventServiceImpl::FireEventImpl(size_t eventId, std::shared_ptr<IEvent>& pe) const
    {
        auto it = mHandlers.find(eventId);
        if (it != mHandlers.end())
        {
            for (auto hbs = it->second.begin(); hbs != it->second.end(); ++hbs)
            {
                (*hbs)->Execute(pe);
            }
        }
        else
        {
            LOG(Error) << "EventService: Unknown event!";
        }
    }

    void EventServiceImpl::UnRegisterEventHandler(const HandlerBase& h)
    {
        const auto handlerIt = mHandlers.find(h.GetEventId());
        if (handlerIt != mHandlers.end())
        {
            auto& [eventId, handlerArray] = *handlerIt;
            std::vector<const HandlerBase*>::iterator hbs;
            while ((hbs = std::find(handlerArray.begin(), handlerArray.end(), &h)) != handlerArray.end())
            {
                handlerIt->second.erase(hbs);
            }
        }
        else
        {
            LOG(Error) << "EventService: Failed to unregister event handler!";
        }
    }
}
