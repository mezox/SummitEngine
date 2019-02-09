#pragma once

#include <Core/Service.h>
#include "LoggingBase.h"
#include "Logger.h"

#define LOG_MSG_MODULE(id, severity) \
    if(Logging::LoggingServiceLocator::Service().GetLogger(LOGGER_ID)) \
        Logging::Composer(*Logging::LoggingServiceLocator::Service().GetLogger(LOGGER_ID), 0, id, severity)

#define LOG(severity) LOG_MSG_MODULE(LOG_MODULE_ID, Logging::Severity::severity)

/// 4 byte composed of chars can be used to identify module like id = LOG_MODULE_4BYTE('C','O','R','E')
#define LOG_MODULE_4BYTE(b1,b2,b3,b4) ((int)(b1) + ((int)(b2) << 8) + ((int)(b3) << 16) + ((int)(b4) << 24))

/// Logger macros
#ifndef LOG_MODULE_ALL
#define LOG_MODULE_ALL 0
#endif

/// Default module id, can be re-defined for each module. Define only if not previously defined!
#ifndef LOG_MODULE_ID
#define LOG_MODULE_ID LOG_MODULE_ALL
#endif

/// Default module id, can be re-defined for each module. Define only if not previously defined!
#ifndef LOGGER_ID
#define LOGGER_ID "SummitEngine"
#endif

namespace Logging
{
	class LOGGING_API ILoggingService
	{
	public:
		virtual ~ILoggingService() = default;
        
        /**
         * @brief   Initializes logging service.
         */
        virtual void Initialize() = 0;
        
        /**
         * @brief   Deinitializes logging service.
         */
        virtual void Deinitialize() = 0;

        /**
         * @brief   Returns logger by its name.
         *
         * @param   name Logger name.
         */
        virtual Logger* GetLogger(const std::string& name) const = 0;
        
		/**
		 * @brief	Adds new logger.
		 *
         * @param logger The Logger.
		 */
        virtual void AddLogger(std::unique_ptr<Logger> logger) = 0;

		/**
		 * @brief	Removes the logger identified by its name.
		 * @param	name Logger name
		 */
		virtual void RemoveLogger(const std::string& name) = 0;
	};

    LOGGING_API std::unique_ptr<ILoggingService> CreateLoggingService();

    class LOGGING_API LoggingServiceLocator
    {
    public:
        static void Provide(std::unique_ptr<ILoggingService> service)
        {
            mService = std::move(service);
        }

        static ILoggingService& Service()
        {
            if(!mService)
            {
                throw std::runtime_error("Logging Service unitialized!");
            }

            return *mService;
        }

        static bool Available()
        {
            return mService != nullptr;
        }

    private:
        static std::unique_ptr<ILoggingService> mService;
    };
}
