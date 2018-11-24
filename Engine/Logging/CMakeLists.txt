cmake_minimum_required(VERSION 3.5.0)

project(Logging)

if (${BUILD_SHARED_LIBS})
	add_definitions("-D_LOGGING_EXPORT")
endif()

include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/Logging/LoggingServiceImpl.h
	Private/Logging/LoggingServiceImpl.cpp
	Private/Logging/Logger.cpp
	Private/Logging/Severity.cpp
	Private/Logging/Writer.cpp
    Private/Logging/ConsoleWriter.cpp
    Private/Logging/FileWriter.cpp
)

set(PUBLIC_SOURCES
    Public/Logging/LoggingBase.h
    Public/Logging/LoggingService.h
	Public/Logging/Logger.h
	Public/Logging/Severity.h
    Public/Logging/Tokens.h
	Public/Logging/Writer.h
    Public/Logging/FileWriter.h
    Public/Logging/ConsoleWriter.h
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
