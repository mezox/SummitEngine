cmake_minimum_required(VERSION 3.8)

project(FileSystem)

if (${BUILD_SHARED_LIBS})
add_definitions("-D_FILESYSTEM_EXPORT")
endif()


include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/File.cpp
	Private/FileSystemServiceImpl.h

)

set(PUBLIC_SOURCES
	Public/PAL/FileSystem/FileSystemBase.h
	Public/PAL/FileSystem/FileSystemService.h
	Public/PAL/FileSystem/File.h
	Public/PAL/FileSystem/FileTypes.h
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)
	set(OBJECTIVE_C_SOURCES
		Private/FileSystemServiceImpl.cpp
	)

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
	)

	set_objective_c_compilation(${OBJECTIVE_C_SOURCES})

    set(PLATFORM_INTERFACES

    )
endif()

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Public/ ${PLATFORM_INTERFACES})
