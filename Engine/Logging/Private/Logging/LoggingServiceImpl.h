#pragma once

#include <Logging/LoggingService.h>

namespace Logging
{
	class LoggingServiceImpl : public ILoggingService
	{
	public:
		LoggingServiceImpl();
        ~LoggingServiceImpl();
        
        void Initialize() override;
        void Deinitialize() override;
        
        Logger* GetLogger(const std::string& name) const override;

		void AddLogger(std::unique_ptr<Logger> logger) override;
		void RemoveLogger(const std::string& name) override;
        
	private:
        std::unordered_map<std::string, std::unique_ptr<Logger>> mLoggers;
	};
}
