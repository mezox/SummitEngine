cmake_minimum_required(VERSION 3.6.0)

project(Microprofile)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_MICROPROFILE_EXPORT")
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
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/microprofile.cpp
)

set(PUBLIC_SOURCES
    Public/microprofile/microprofile.h
)

configure_platform_file(${PROJECT_NAME} Public/microprofile  ${PUBLIC_SOURCES})

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
)

target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "${IDE_PROJECT_FOLDER}")
