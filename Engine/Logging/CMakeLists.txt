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
    Public/Logging/LoggingBase.h
    Public/Logging/LoggingService.h
	Public/Logging/Logger.h
	Public/Logging/Severity.h
)

set(PRECOMPILED_SOURCE Private/Logging/pch.cpp)
set(PRECOMPILED_HEADER Private/Logging/pch.h)

# Platform agnostic dependencies
set(DEPENDENCIES
    # External
    nlohmann_json

    # Internal
    FileSystem
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
	${PRECOMPILED_SOURCE}
	${PRECOMPILED_HEADER}
)

target_set_precompiled_header(${PROJECT_NAME} ${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})
