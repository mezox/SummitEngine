cmake_minimum_required(VERSION 3.6.0)

project(Core)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_CORE_EXPORT")
endif()

# Platform agnostic dependencies
set(EXTERNAL_DEPENDENCIES
	Microprofile
)

set(DEPENDENCIES
    Logging
)

# Include directories
include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/SummitDispatcher.cpp
)

set(PUBLIC_SOURCES
    Public/Core/Service.h
    Public/Core/TypeConvert.h
    Public/Core/Templates.h
    Public/Core/Assert.h
    Public/Dispatcher/SummitDispatcher.h
    Public/Event/Signal.h
    Public/Event/Event.h
)

# set(PRECOMPILED_SOURCE Private/Logging/pch.cpp)
# set(PRECOMPILED_HEADER Private/Logging/pch.h)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

configure_platform_file(${PROJECT_NAME} Public ${PUBLIC_SOURCES})

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
#	${PRECOMPILED_SOURCE}
#   ${PRECOMPILED_HEADER}
)

target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})

ide_source_files_group( ${PUBLIC_SOURCES}
                        ${PRIVATE_SOURCES}
)
