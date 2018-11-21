cmake_minimum_required(VERSION 3.5.0)

project(Logging)

include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/Logging/Logger.cpp
)

set(PUBLIC_SOURCES
	Public/Logging/Logger.h
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${PUBLIC_SOURCES})