cmake_minimum_required(VERSION 3.6.0)

project(Core)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_CORE_EXPORT")
endif()

# Include directories
include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES

)

set(PUBLIC_SOURCES
    Public/Core/Service.h
)

# set(PRECOMPILED_SOURCE Private/Logging/pch.cpp)
# set(PRECOMPILED_HEADER Private/Logging/pch.h)

# Platform agnostic dependencies
set(DEPENDENCIES
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public/>)