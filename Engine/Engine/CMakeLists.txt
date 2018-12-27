cmake_minimum_required(VERSION 3.6.0)

project(Engine)

if (${BUILD_SHARED_LIBS})
	add_definitions("-D_ENGINE_EXPORT")
endif()

include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/EngineImpl.h
	Private/EngineImpl.cpp
	Private/Window.h
)

set(PUBLIC_SOURCES
	Public/Engine/EngineBase.h
	Public/Engine/Engine.h
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

    set(PLATFORM_INTERFACES
    )
endif()

set(DEPENDENCIES
	nlohmann_json

	FileSystem
	Logging
	RenderAPI
)


add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_link_libraries(${PROJECT_NAME} "-framework AppKit" ${DEPENDENCIES})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
