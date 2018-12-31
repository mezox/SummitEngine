#include <Logging/pch.h>
#include <Logging/LoggingServiceImpl.h>
#include <Logging/Logger.h>
#include <Logging/ConsoleWriter.h>
#include <Logging/FileWriter.h>

#include <PAL/FileSystem/File.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Logging
{
    namespace Detail
    {
        struct WriterSettings
        {
            // Shared settings
            std::string type;
            std::string messageFormat;
            std::string timeFormat;
            Severity severity;
            uint32_t channel;
            
            // File settings
            std::string filePath;
            bool appendToFile;
            uint32_t flushAfterLines;
        };
        
        void to_json(json& jsonObject, const WriterSettings& settings)
        {
            jsonObject = json {
                {"type", settings.type},
                {"message_format", settings.messageFormat},
                {"time_format", settings.timeFormat},
                {"channel", settings.channel},
                {"file", settings.filePath}
            };
            
            switch (settings.severity)
            {
                case Severity::Information:
                    jsonObject["severity"] = "Information";
                    break;
                case Severity::Debug:
                    jsonObject["severity"] = "Debug";
                    break;
                case Severity::Error:
                    jsonObject["severity"] = "Error";
                    break;
                default:
                    break;
            }
        }
        
        void from_json(const json& jsonObject, WriterSettings& settings)
        {
            jsonObject.count("type") ? jsonObject.at("type").get_to(settings.type) : settings.type = "console";
            jsonObject.count("message_format") ? jsonObject.at("message_format").get_to(settings.messageFormat) : settings.messageFormat = SE_DEFAULT_MSG_FORMAT;
            jsonObject.count("time_format") ? jsonObject.at("time_format").get_to(settings.timeFormat) : settings.timeFormat = SE_DEFAULT_TIME_FORMAT;
            jsonObject.count("channel") ? jsonObject.at("channel").get_to(settings.channel) : settings.channel = 0;
            jsonObject.count("file") ? jsonObject.at("file").get_to(settings.filePath) : settings.filePath = "";
        }
        
        std::unique_ptr<WriterBase> CreateWriter(const WriterSettings& settings)
        {
            if(settings.type == "console")
                return std::make_unique<ConsoleWriter>(settings.messageFormat, settings.timeFormat);
            else if(settings.type == "file")
                return std::make_unique<FileWriter>(settings.filePath, settings.messageFormat, settings.timeFormat);
            
            return nullptr;
        }
        
        void ParseWriter(Logger& logger, const json& writerJson)
        {
            WriterSettings settings = writerJson;
            auto consoleWriterService = CreateWriter(settings);
            logger.AddWriter(std::move(consoleWriterService));
        }
        
        void ParseLogger(const json& jsonObject)
        {
            std::string name;
            jsonObject.at("name").get_to(name);
            
            if(!name.empty())
            {
                auto logger = std::make_unique<Logger>(name);
                
                const auto& writersArray = jsonObject.at("writers");
                if(writersArray.is_array())
                {
                    for(const auto& writer : writersArray)
                    {
                        Detail::ParseWriter(*logger.get(), writer);
                    }
                }
                
                Logging::LoggingServiceLocator::Service().AddLogger(std::move(logger));
            }
        }
    }

    std::unique_ptr<ILoggingService> LoggingServiceLocator::mService = nullptr;

    std::unique_ptr<ILoggingService> CreateLoggingService()
    {
        return std::make_unique<LoggingServiceImpl>();
    }

    LoggingServiceImpl::LoggingServiceImpl()
    {
        
    }

    LoggingServiceImpl::~LoggingServiceImpl()
    {
        
    }
    
    void LoggingServiceImpl::Initialize()
    {
        using namespace PAL::FileSystem;
        
#ifndef _WIN32
        File configFile("/Users/tomaskubovcik/Dev/SummitEngine/logging_config.json");
#else
		File configFile("C:/Users/Tomas/Dev/SummitEngine/logging_config.json");
#endif
        configFile.Open(EFileAccessMode::Read);
        if(configFile.IsOpened())
        {
            const auto data = configFile.Read();
            json jsonData = json::parse(data);
            Detail::ParseLogger(jsonData);
        }
    }
    
    void LoggingServiceImpl::Deinitialize()
    {
    }

    Logger* LoggingServiceImpl::GetLogger(const std::string& name) const
    {
        auto result = std::find_if(mLoggers.begin(), mLoggers.end(), [&name](const auto& logger){
            return logger.second->GetName() == name;
        });
        
        if(result != mLoggers.end())
        {
            return result->second.get();
        }
        
        return nullptr;
    }

    void LoggingServiceImpl::AddLogger(std::unique_ptr<Logger> logger)
    {
        const auto& loggerName = logger->GetName();
        auto result = std::find_if(mLoggers.begin(), mLoggers.end(), [&loggerName](const auto& logger){
            return logger.second->GetName() == loggerName;
        });
        
        if(result != mLoggers.end())
        {
            mLoggers[loggerName].swap(logger);
        }
        else
        {
            mLoggers[loggerName] = std::move(logger);
        }
    }

    void LoggingServiceImpl::RemoveLogger(const std::string& name)
    {
//        std::remove_if(mLoggers.begin(), mLoggers.end(), [&name](auto& logger) {
//            return logger.second->GetName() == name;
//        });
    }
}
