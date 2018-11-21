cmake_minimum_required(VERSION 3.0)
project(SummitEngineApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(PLATFORM_APPLE ON)

	if(NOT DEFINED IOS_PLATFORM)
		set(PLATFORM_APPLE_MACOS ON)
		message(STATUS "Platform: macOS")
	else()
		set(PLATFORM_APPLE_IOS ON)
		message(STATUS "Platform: iOS")
	endif()

else()
	message(FATAL_ERROR "Unsupported platform! SummitEngine supports only macOS")
endif()

# Architecture detection
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ARCHITECTURE_64 TRUE)
	message(STATUS "Architecture: 64 bit")
else()
	set(ARCHITECTURE_32 TRUE)
	message(STATUS "Architecture: 32 bit")
endif()

if(PLATFORM_APPLE_MACOS)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
	set(CMAKE_SHARED_MODULE_PREFIX "lib")
	set(CMAKE_SHARED_MODULE_SUFFIX ".so")
endif()

add_subdirectory(3rdParty)
add_subdirectory(Engine)
add_subdirectory(Application)