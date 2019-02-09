cmake_minimum_required(VERSION 3.12.3)
project(SummitEngineApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(ENABLE_CLANG_TIDY OFF)

# Enable project groupping in IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

macro(ide_source_files_group)
	foreach(source_file ${ARGN})
		string(REGEX REPLACE "/?[^/]+$" "" folder ${source_file})
        set(folder "${_target_folder}/${folder}")
        string(REPLACE "/" "\\" folder ${folder})

		source_group(${folder} FILES ${source_file})
	endforeach()
endmacro()

if (CMAKE_GENERATOR STREQUAL Xcode)
	macro(target_set_precompiled_header target pch pcs)
		set_property(TARGET ${target} PROPERTY XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES)
		set_property(TARGET ${target} PROPERTY XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${pch}")
		target_compile_definitions(${target} PRIVATE -DUSE_PCH)
	endmacro(target_set_precompiled_header)
else()
	macro(target_set_precompiled_header target pch pcs)
		target_compile_definitions(${target} PRIVATE -DUSE_PCH)
	endmacro(target_set_precompiled_header)
endif()

macro(append_compile_flags _flags)
foreach(source_file ${ARGN})
set_property(SOURCE ${source_file} APPEND_STRING PROPERTY COMPILE_FLAGS " ${_flags}")
endforeach()
endmacro(append_compile_flags)

if (ENABLE_CLANG_TIDY)
	find_program (CLANG_TIDY_EXE NAMES "clang-tidy" PATHS /usr/local/opt/llvm/bin )
	if (CLANG_TIDY_EXE)
	  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
	  set(CLANG_TIDY_CHECKS "-*,modernize-*")
	  set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};checks=-*,readability-*"
	    CACHE STRING "" FORCE)
	else()
	  message(AUTHOR_WARNING "clang-tidy not found!")
	  set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) # delete it
	endif()
endif()

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

	macro(set_objective_c_compilation)
		foreach(source ${ARGN})
			append_compile_flags("-x objective-c++" ${source})
			message(STATUS "Set Objective-C compilation for ${source}")
		endforeach()
	endmacro()

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(PLATFORM_WINDOWS ON)
	set(BUILD_SHARED_LIBS ON)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
else()
	message(FATAL_ERROR "Unsupported platform! SummitEngine supports only macOS")
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Library output path")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Executable output path")

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

	set(BUILD_SHARED_LIBS ON)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-abi-version=2 -fobjc-arc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -stdlib=libc++ -fobjc-abi-version=2 -fobjc-arc")

    if (${BUILD_SHARED_LIBS})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
	endif()
elseif(PLATFORM_APPLE_IOS)
	set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
	set(CMAKE_SHARED_LIBRARY_SUFFIX ".dylib")
	set(CMAKE_SHARED_MODULE_PREFIX "lib")
	set(CMAKE_SHARED_MODULE_SUFFIX ".so")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fobjc-abi-version=2 -fobjc-arc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -stdlib=libc++ -fobjc-abi-version=2 -fobjc-arc")

    if (${BUILD_SHARED_LIBS})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
	endif()
endif()

set(SummitEngineDir ${CMAKE_CURRENT_SOURCE_DIR})

macro(configure_platform_file project_name local_path public_sources)
	string(TOUPPER ${project_name} MODULE_NAME)
	configure_file(${SummitEngineDir}/ModuleBase.h.in ${CMAKE_CURRENT_SOURCE_DIR}/${local_path}/${PROJECT_NAME}Base.h @ONLY)
	list(APPEND ${public_sources} ${CMAKE_CURRENT_SOURCE_DIR}/${local_path}/${PROJECT_NAME}Base.h)
endmacro(configure_platform_file)

add_subdirectory(3rdParty)
add_subdirectory(Engine)
add_subdirectory(Application)

#add_executable(TestApp main.cpp)
#target_link_libraries(TestApp doctest Math)
