#include <Dispatcher/SummitDispatcher.h>
#include <Logging/LoggingService.h>

#include <microprofile/microprofile.h>

#include <unordered_map>
#include <pthread.h>

namespace Core
{
    std::unique_ptr<SummitDispatcher> DispatcherService::mService = nullptr;
    
    std::unique_ptr<SummitDispatcher> CreateSummitDispatcher()
    {
        return std::make_unique<SummitDispatcher>();
    }
    
    SummitDispatcher::SummitDispatcher()
    {
    }
    
    SummitDispatcher::~SummitDispatcher()
    {
        mIsRunning = false;
        mThread.join();
    }
    
    uint32_t SummitDispatcher::Schedule(uint32_t deltaTime, std::function<void()>&& f, bool repeat)
    {
        if(!mIsRunning)
        {
            CreateThread();
        }
        
        mFunc = f;
        return 0;
    }
    
    void SummitDispatcher::Post(std::function<void()>&& f)
    {
        auto taskPtr = std::make_shared<std::function<void()>>(std::move(f));
        mQueue.Push(std::move(taskPtr));
    }
    
    void SummitDispatcher::CreateThread()
    {
        mIsRunning = true;
        
        mThread = std::thread([this](){
            pthread_setname_np("SummitDispatcher");
            MicroProfileOnThreadCreate("Dispatcher");
            Worker();
        });
    }
    
    void SummitDispatcher::Worker()
    {
        uint32_t mCnt{ 0 };
        
        while(mIsRunning)
        {
            mCnt++;
            
            //if(mQueue.Size() > 0)
            //    LOG(Debug) << "Queue Size: " << mQueue.Size();
            
            auto funcPtr = mQueue.Pop();
            if(funcPtr)
            {
                auto func = *funcPtr.get();
                func();
            }
            
            if(mCnt % 1000000 == 0)
            {
                mFunc();
                mCnt = 0;
            }
        }
    }
}
