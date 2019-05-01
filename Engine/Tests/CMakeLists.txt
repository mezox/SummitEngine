cmake_minimum_required(VERSION 3.9.12)

project(UnitTests)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_UNITTEST_EXPORT")
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
	Renderer
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/main.cpp
)

set(PUBLIC_SOURCES
)

# configure_platform_file(${PROJECT_NAME} Public/UnitTests  ${PUBLIC_SOURCES})

add_executable(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
)

target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})

ide_source_files_group( ${PUBLIC_SOURCES}
                        ${PRIVATE_SOURCES}
)

#doctest_add_test(NAME ${PROJECT_NAME} COMMAND $<TARGET_FILE:${PROJECT_NAME}> --no-version)
