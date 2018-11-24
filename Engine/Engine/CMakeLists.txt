cmake_minimum_required(VERSION 3.5.0)

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
)

set(PUBLIC_SOURCES
	Public/Engine/EngineBase.h
	Public/Engine/Engine.h
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)

endif()

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_link_libraries(${PROJECT_NAME} Logging)
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
