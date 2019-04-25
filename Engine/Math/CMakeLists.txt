cmake_minimum_required(VERSION 3.6.0)

project(Math)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_MATH_EXPORT")
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
    Core
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/Matrix4.cpp
)

set(PUBLIC_SOURCES
	Public/Math/Math.h
    Public/Math/Vector2.h
    Public/Math/Vector3.h
    Public/Math/Vector4.h
    Public/Math/Matrix4.h
)

#set(PRECOMPILED_SOURCE Private/pch.cpp)
#set(PRECOMPILED_HEADER Private/pch.h)

configure_platform_file(${PROJECT_NAME} Public/Math  ${PUBLIC_SOURCES})

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
#	${PRECOMPILED_SOURCE}
#	${PRECOMPILED_HEADER}
)

#target_set_precompiled_header(${PROJECT_NAME} ${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})

ide_source_files_group( ${PUBLIC_SOURCES}
                        ${PRIVATE_SOURCES}
                        #${PRECOMPILED_SOURCE}
                        #${PRECOMPILED_HEADER}
)
