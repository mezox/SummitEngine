#include "TimerServiceImpl.h"

#include <Logging/LoggingService.h>
#include <pthread.h>

#include <microprofile/microprofile.h>

namespace Timer
{
    std::unique_ptr<ITimerService> TimerServiceLocator::mService = nullptr;
    
    std::unique_ptr<ITimerService> Timer::CreateTimerService()
    {
        return std::make_unique<TimerServiceImpl>();
    }
    
    TimerServiceImpl::TimerServiceImpl()
    {
    }
    
    uint32_t TimerServiceImpl::Schedule(uint32_t deltaTime, std::function<void()>&& f, bool repeat)
    {
        if(!mIsRunning)
        {
            CreateThread();
        }
        
        mFunc = f;
        return 0;
    }
    
    void TimerServiceImpl::CreateThread()
    {
        mThread = std::thread([this](){
            pthread_setname_np("TimerService");
            MicroProfileOnThreadCreate("Timer");
            Worker();
        });
    }
    
    void TimerServiceImpl::Worker()
    {
        uint32_t secondsRunning = 0;
        
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        
        while(1)
        {            
            mFunc();
        }
    }
}
