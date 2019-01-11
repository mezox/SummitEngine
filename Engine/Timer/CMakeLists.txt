cmake_minimum_required(VERSION 3.6.0)

project(Timer)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_TIMER_EXPORT")
endif()

# Include directories
include_directories(
	"Public"
	"Private"
)

# Platform agnostic dependencies
set(EXTERNAL_DEPENDENCIES
	
)

set(DEPENDENCIES
    Logging
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/TimerServiceImpl.h
	Private/TimerServiceImpl.cpp
)

set(PUBLIC_SOURCES
    Public/Timer/TimerService.h
)

set(PRECOMPILED_SOURCE Private/pch.cpp)
set(PRECOMPILED_HEADER Private/pch.h)

configure_platform_file(${PROJECT_NAME} Public/Timer  ${PUBLIC_SOURCES})

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
	${PRECOMPILED_SOURCE}
	${PRECOMPILED_HEADER}
)

target_set_precompiled_header(${PROJECT_NAME} ${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})
