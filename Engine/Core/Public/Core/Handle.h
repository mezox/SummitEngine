#pragma once

#include <functional>

template<typename T>
class MovableHandle
{
public:
    MovableHandle() = default;
    MovableHandle(const MovableHandle& other) = delete;
    MovableHandle& operator=(const MovableHandle& other) = delete;
    
    MovableHandle(MovableHandle&& other)
    {
        mHandle = other.mHandle;
        other.mHandle = {};
    }
    
    MovableHandle& operator=(MovableHandle&& other)
    {
        mHandle = other.mHandle;
        other.mHandle = {};
        return *this;
    }
    
    MovableHandle(T& directHandle)
    : mHandle(directHandle)
    {}
    
    MovableHandle(T&& directHandle)
    : mHandle(std::move(directHandle))
    {}
    
    const T& Get() const
    {
        return mHandle;
    }
    
    T& Get()
    {
        return mHandle;
    }
    
private:
    T mHandle{};
};

template<typename T>
class ManagedHandle
{
public:
    using DeleterType = std::function<void(const T&)>;
    
public:
    ManagedHandle() = delete;
    ManagedHandle(const ManagedHandle& other) = delete;
    ManagedHandle operator=(const ManagedHandle& other) = delete;
    
    ManagedHandle(T& handle, DeleterType&& deleter)
    : mHandle(handle)
    , mDeleter(std::move(deleter))
    {}
    
    ManagedHandle(ManagedHandle&& other)
    {
        mHandle = std::move(other.mHandle);
        mDeleter = std::move(other.mDeleter);
        other.mDeleter = nullptr;
    }
    
    ManagedHandle& operator=(ManagedHandle&& other)
    {
        mHandle = std::move(other.mHandle);
        mDeleter = std::move(other.mDeleter);
        other.mDeleter = nullptr;
        return *this;
    }
    
    const T& Get() const
    {
        return mHandle.Get();
    }
    
    T& Get()
    {
        return mHandle.Get();
    }
    
    ~ManagedHandle()
    {
        if(mDeleter)
        {
            mDeleter(mHandle.Get());
        }
    }
    
private:
    MovableHandle<T> mHandle;
    std::function<void(const T&)> mDeleter;
};
