cmake_minimum_required(VERSION 3.6.0)

project(Logging)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_LOGGING_EXPORT")
endif()

# Include directories
include_directories(
	"Public"
	"Private"
)

# Platform agnostic dependencies
set(EXTERNAL_DEPENDENCIES
	nlohmann_json
)

set(DEPENDENCIES
	FileSystem
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
    Private/Logging/Tokens.h
	Private/Logging/Writer.h
    Private/Logging/FileWriter.h
    Private/Logging/ConsoleWriter.h
)

set(PUBLIC_SOURCES
    Public/Logging/LoggingService.h
	Public/Logging/Logger.h
	Public/Logging/Severity.h
)

set(PRECOMPILED_SOURCE Private/Logging/pch.cpp)
set(PRECOMPILED_HEADER Private/Logging/pch.h)

configure_platform_file(${PROJECT_NAME} Public/Logging  ${PUBLIC_SOURCES})

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
	${PRECOMPILED_SOURCE}
	${PRECOMPILED_HEADER}
)

target_set_precompiled_header(${PROJECT_NAME} ${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})
