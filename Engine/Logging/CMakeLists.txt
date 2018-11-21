cmake_minimum_required(VERSION 3.5.0)

project(Logging)

include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/Logging/Logger.cpp
	Private/Logging/Severity.cpp
)

set(PUBLIC_SOURCES
	Public/Logging/Logger.h
	Public/Logging/Severity.h
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
