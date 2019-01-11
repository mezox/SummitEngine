#pragma once

#include <Event/EventBase.h>
#include <Event/Event.h>
#include <Event/EventHandler.h>
#include <memory>

namespace Event
{
    class EVENT_API IEventService
    {
    public:
        template<class Evt>
        void RegisterEventHandler(EventHandler<Evt>& eh)
        {
            RegisterEventHandlerImpl(eh);
        }
        
        virtual void UnRegisterEventHandler(const HandlerBase& h) = 0;
        
        template<class Evt>
        void FireEvent(Evt e) const
        {
            static size_t hash = typeid(Evt).hash_code();
            auto eventPtr = std::shared_ptr<IEvent>(new Evt(e));
            FireEventImpl(hash, eventPtr);
        }
        
        virtual ~IEventService() = default;
        
    protected:
        virtual void RegisterEventHandlerImpl(HandlerBase& h) = 0;
        virtual void FireEventImpl(size_t eventId, std::shared_ptr<IEvent>& pe) const = 0;
    };
    
    EVENT_API std::unique_ptr<IEventService> CreateEventService();
    
    class EVENT_API EventServiceLocator
    {
    public:
        static void Provide(std::unique_ptr<IEventService> service)
        {
            mService = std::move(service);
        }
        
        static IEventService& Service()
        {
            if(!mService)
            {
                throw std::runtime_error("Event service unitialized");
            }
            
            return *mService;
        }
        
        static bool Available()
        {
            return mService != nullptr;
        }
        
    private:
        static std::unique_ptr<IEventService> mService;
    };
}
