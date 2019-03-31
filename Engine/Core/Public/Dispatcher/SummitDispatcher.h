#pragma once

#include <CoreBase.h>

#include <memory>
#include <functional>
#include <thread>
#include <deque>
#include <mutex>
#include <algorithm>
#include <functional>

namespace Core
{
    using Task = std::function<void()>;
    
    template<typename T>
    class Queue
    {
    public:
        void Push(T&& item)
        {
            std::unique_lock lock(mMutex);
            mQueue.push_back(std::move(item));
        }
        
        T Pop()
        {
            std::unique_lock lock(mMutex);
            
            if(mQueue.empty())
                return nullptr;
            
            T result = std::move(mQueue.front());
            mQueue.pop_front();
            
            return result;
        }
        
    private:
        std::deque<T> mQueue;
        std::mutex mMutex;
        bool mIsRunning{ true };
    };
    
    class CORE_API SummitDispatcher
    {
    public:
        SummitDispatcher();
        uint32_t Schedule(uint32_t deltaTime, std::function<void()>&& f, bool repeat);
        void Post(std::function<void()>&& f);
        
    private:
        void CreateThread();
        void Worker();
            
    private:
        std::thread mThread;
        std::function<void()> mFunc;
        bool mIsRunning{ false };
        
        Queue<std::shared_ptr<Task>> mQueue;
    };
    
    CORE_API std::unique_ptr<SummitDispatcher> CreateSummitDispatcher();
    
    class CORE_API DispatcherService
    {
    public:
        static void Provide(std::unique_ptr<SummitDispatcher> service)
        {
            mService = std::move(service);
        }
        
        static SummitDispatcher& Service()
        {
            if(!mService)
            {
                throw std::runtime_error("SummitDispatcher service unitialized");
            }
            
            return *mService;
        }
        
        static bool Available()
        {
            return mService != nullptr;
        }
        
    private:
        static std::unique_ptr<SummitDispatcher> mService;
    };
}
