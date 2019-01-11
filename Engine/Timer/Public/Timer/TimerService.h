#pragma once

#include <Timer/TimerBase.h>
#include <memory>
#include <functional>

namespace Timer
{
    class TIMER_API ITimerService
    {
    public:
        virtual ~ITimerService() = default;
        
        virtual uint32_t Schedule(uint32_t deltaTime, std::function<void()>&& f, bool repeat) = 0;
    };
    
    TIMER_API std::unique_ptr<ITimerService> CreateTimerService();
    
    class TIMER_API TimerServiceLocator
    {
    public:
        static void Provide(std::unique_ptr<ITimerService> service)
        {
            mService = std::move(service);
        }
        
        static ITimerService& Service()
        {
            if(!mService)
            {
                throw std::runtime_error("Timer service unitialized");
            }
            
            return *mService;
        }
        
        static bool Available()
        {
            return mService != nullptr;
        }
        
    private:
        static std::unique_ptr<ITimerService> mService;
    };
}
