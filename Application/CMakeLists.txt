cmake_minimum_required(VERSION 3.7)

SET(TARGET_PROJECT_FOLDER "${TARGET_PROJECT_FOLDER}/Application")

if(PLATFORM_APPLE_MACOS)
	add_subdirectory(macOS)
elseif(PLATFORM_APPLE_IOS)
	add_subdirectory(iOS)
elseif(PLATFORM_WINDOWS)
	add_subdirectory(Windows)
endif()