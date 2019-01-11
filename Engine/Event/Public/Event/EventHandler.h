#pragma once

#include <Event/Event.h>
#include <typeinfo>
#include <thread>

namespace Event
{
	class EVENT_API HandlerBase
    {
    public:
        HandlerBase() = delete;
        virtual ~HandlerBase() = default;
        
        const size_t GetEventId() const;
        
        virtual void Execute(std::shared_ptr<IEvent>& e) const;
        
    protected:
        HandlerBase(bool sync);
        
        virtual void Callback(std::shared_ptr<IEvent>& e) const = 0;
        void CallbackAsync(std::shared_ptr<IEvent> e) const;
        
    protected:
        bool        mExecuteSync{ true };
        size_t      mEventId{ 0 };
    };
    
    template<class Event>
    class EventHandler : public HandlerBase
    {
    public:
        EventHandler() = delete;
        
        EventHandler(const bool sync)
            : HandlerBase(sync)
        {
            mEventId = typeid(Event).hash_code();
        };
        
        virtual ~EventHandler() = default;
        
        virtual void OnEvent(const Event &e) const = 0;
        
    private:
        void Callback(std::shared_ptr<IEvent>& e) const override
        {
            const Event &evt = dynamic_cast<const Event&>(*e);
            OnEvent(evt);
        };
    };
    
    template<typename Listener, typename Event>
    class EventHandlerFunc : public EventHandler<Event>
    {
        typedef void (Listener::*t_cb)(const Event& e);
        
    public:
        EventHandlerFunc()
            :   EventHandler<Event>(true)
        {}
        
        EventHandlerFunc(const bool sync, Listener* listener, t_cb func)
            :   EventHandler<Event>(sync)
            ,   mListener(listener)
            ,   mCallbackFunc(func)
        {}
        
        virtual ~EventHandlerFunc() = default;
        
        void OnEvent(const Event &e) const override
        {
            if (mListener)
            {
                (mListener->*mCallbackFunc)(e);
            }
        }
        
    private:
        Listener* mListener{ nullptr };
        t_cb mCallbackFunc{ nullptr };
    };
}