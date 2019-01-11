#include "TimerServiceImpl.h"

#include <Logging/LoggingService.h>
#include <pthread.h>

namespace Timer
{
    std::unique_ptr<ITimerService> TimerServiceLocator::mService = nullptr;
    
    std::unique_ptr<ITimerService> Timer::CreateTimerService()
    {
        return std::make_unique<TimerServiceImpl>();
    }
    
    TimerServiceImpl::TimerServiceImpl()
    {
        CreateThread();
    }
    
    uint32_t TimerServiceImpl::Schedule(uint32_t deltaTime, std::function<void()>&& f, bool repeat)
    {
        return 0;
    }
    
    void TimerServiceImpl::CreateThread()
    {
        mThread = std::thread([this](){
            pthread_setname_np("TimerService");
            Worker();
        });
    }
    
    void TimerServiceImpl::Worker()
    {
        uint32_t secondsRunning = 0;
        
        while(1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            LOG(Debug) << "Engine running for: " << secondsRunning << " seconds";
            ++secondsRunning;
        }
    }
}
