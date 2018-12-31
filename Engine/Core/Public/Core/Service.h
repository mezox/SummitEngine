#pragma once

#include <memory>
#include <utility>
#include <exception>

namespace Core
{
    struct service_locator_exception : public std::runtime_error { using std::runtime_error::runtime_error; };
    
	template<class T>
	class ServiceLocatorBase
	{
	public:
	    static void Provide(std::unique_ptr<T> service)
	    {
	        service = std::move(service);
	    }
	    
	    static T& Service()
	    {
            if(!mService)
            {
                throw service_locator_exception("Service is uninitialized!");
            }
            
	        return *mService;
	    }
	    
	    static bool Available()
	    {
            return mService != nullptr;
	    }
	    
	protected:
        /* Use Singleton initialization to instead of static member
         * to prevent undefined behaviour caused by templatization of the class */
//        static std::unique_ptr<T>& Storage()
//        {
//            static std::unique_ptr<T> service;
//            return service;
//        }
        
        static std::unique_ptr<T> mService;
	};
    
    template<typename T>
    std::unique_ptr<T> Core::ServiceLocatorBase<T>::mService = nullptr;
}
