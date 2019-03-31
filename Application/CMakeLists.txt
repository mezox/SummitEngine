cmake_minimum_required(VERSION 3.7)

SET(TARGET_PROJECT_FOLDER "${TARGET_PROJECT_FOLDER}/Application")

include_directories(".")

# platform agnostic source files
set(SHARED_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/SummitDemo.h
	${CMAKE_CURRENT_SOURCE_DIR}/SummitDemo.cpp
)

if(PLATFORM_APPLE_MACOS)
	add_subdirectory(macOS)
elseif(PLATFORM_APPLE_IOS)
	add_subdirectory(iOS)
elseif(PLATFORM_WINDOWS)
	add_subdirectory(Windows)
endif()