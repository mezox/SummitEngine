#pragma once

#include <Timer/TimerService.h>
#include <unordered_map>
#include <thread>

namespace Timer
{
    class TimerServiceImpl : public ITimerService
    {
    public:
        TimerServiceImpl();
        uint32_t Schedule(uint32_t deltaTime, std::function<void()>&& f, bool repeat) override;
        
    private:
        void CreateThread();
        void Worker();
        
    private:
        std::thread mThread;
        std::function<void()> mFunc;
        
        bool mIsRunning{ false };
    };
}
