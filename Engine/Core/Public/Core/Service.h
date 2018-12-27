#pragma once

#include <memory>
#include <utility>

namespace Core
{
	template<typename T>
	class ServiceLocatorBase
	{
	public:
	    static void Provide(std::shared_ptr<T> service)
	    {
	        mService = std::move(service);
	    }
	    
	    static std::shared_ptr<T> Service()
	    {
	        return mService;
	    }
	    
	    static bool Available()
	    {
	        return mService != nullptr;
	    }
	    
	private:
	    static std::shared_ptr<T> mService;
	};

	template<typename T>
	std::shared_ptr<T> ServiceLocatorBase<T>::mService = nullptr;
}