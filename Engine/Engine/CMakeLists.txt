cmake_minimum_required(VERSION 3.6.0)

project(Engine)

if (${BUILD_SHARED_LIBS})
	add_definitions("-D_ENGINE_EXPORT")
endif()

include_directories(
	"Public"
	"Private"
)

set(EXTERNAL_DEPENDENCIES
    Microprofile
)

set(DEPENDENCIES
	FileSystem
	Logging
	RenderAPI
    Event
    Timer
    Math
    Renderer
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/EngineImpl.cpp
)

set(PUBLIC_SOURCES
	Public/Engine/Engine.h
    Public/Engine/Window.h
    Public/Engine/WindowEvent.h
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

	set(OBJECTIVE_C_SOURCES
		Private/macOS/SummitView.h
		Private/macOS/SummitWindow.h
		Private/macOS/ViewController.h
		Private/macOS/ViewController.m
		Private/macOS/SummitView.mm
		Private/macOS/SummitWindow.mm
		Private/macOS/Window.mm
	)

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
	)

	set_objective_c_compilation(${OBJECTIVE_C_SOURCES})

    list(APPEND DEPENDENCIES
    	"-framework AppKit"
        "-framework QuartzCore"
    )
elseif(PLATFORM_APPLE_IOS)
	set(OBJECTIVE_C_SOURCES
		Private/iOS/ViewController.h
		Private/iOS/ViewController.mm
		Private/iOS/SummitView.h
		Private/iOS/SummitView.mm
		Private/iOS/Window.mm
	)

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
	)

	set_objective_c_compilation(${OBJECTIVE_C_SOURCES})

    list(APPEND DEPENDENCIES
    	"-framework UIKit"
        "-framework QuartzCore"
    )
elseif(PLATFORM_WINDOWS)
	list(APPEND PRIVATE_SOURCES
		Private/Windows/Window.cpp
	)
endif()

configure_platform_file(${PROJECT_NAME} Public/Engine "${PUBLIC_SOURCES}")


add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")

ide_source_files_group( ${PUBLIC_SOURCES}
                        ${PRIVATE_SOURCES}
                        ${PRECOMPILED_SOURCE}
                        ${PRECOMPILED_HEADER}
)
