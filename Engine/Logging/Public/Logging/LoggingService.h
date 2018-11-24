#pragma once

#include "LoggingBase.h"
#include "Logger.h"
#include <memory>
#include <string>
#include <sstream>

#define LOG_MSG_MODULE(m, id, severity) \
std::ostringstream ostr; \
ostr << m << std::endl; \
auto logger = Logging::LoggingServiceLocator::Service()->GetLogger(LOGGER_ID); \
if(logger) \
    logger->Log(ostr.str(), id, severity);

#define LOG_INFORMATION(m) LOG_MSG_MODULE(m, LOG_MODULE_ID, Logging::Severity::Information)

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
#define LOGGER_ID "Engine"
#endif

namespace Logging
{
	class LOGGING_API ILoggingService
	{
	public:
		virtual ~ILoggingService() = default;

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

    LOGGING_API std::shared_ptr<ILoggingService> CreateLoggingService();

	class LOGGING_API LoggingServiceLocator
	{
	public:
		static void Provide(std::shared_ptr<ILoggingService> service)
		{
			mService = std::move(service);
		}

		static std::shared_ptr<ILoggingService> Service()
		{
			return mService;
		}

		static bool Available()
		{
			return !mService;
		}

	private:
		static std::shared_ptr<ILoggingService> mService;
	};
}
