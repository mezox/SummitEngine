cmake_minimum_required(VERSION 3.12)

project(RenderAPI)

if (${BUILD_SHARED_LIBS})
	add_definitions("-D_RENDERAPI_EXPORT")
endif()

set(VULKAN_SDK "/Users/tomaskubovcik/Dev/sdks/vulkansdk/macOS")
find_package(vulkan REQUIRED)

include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/Metal/MetalAPIImpl.h
	Private/Vulkan/VulkanAPIImpl.h
	Private/Vulkan/VulkanLoaderHelper.h
	Private/Vulkan/VulkanDebug.inl

	Private/RenderAPIFactory.cpp
)

set(PUBLIC_SOURCES
	Public/PAL/RenderAPI/RenderAPIBase.h
	Public/PAL/RenderAPI/RenderAPIService.h
)

# Platform agnostic dependencies
set(DEPENDENCIES
    # External
    nlohmann_json

    # Internal
    FileSystem
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)
	add_definitions("-DVK_USE_PLATFORM_MACOS_MVK")

	set(OBJECTIVE_C_SOURCES
		Private/Metal/MetalAPIImpl.cpp
		Private/Vulkan/VulkanAPIImpl.cpp
	)

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
	)

	set_objective_c_compilation(${OBJECTIVE_C_SOURCES})

    set(PLATFORM_INTERFACES

    )
elseif (PLATFORM_APPLE_IOS)
	add_definitions("-DVK_USE_PLATFORM_IOS_MVK")

	set(OBJECTIVE_C_SOURCES
		Private/Metal/MetalAPIImpl.cpp
		Private/Vulkan/VulkanAPIImpl.cpp
	)

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
	)

	set_objective_c_compilation(${OBJECTIVE_C_SOURCES})

    set(PLATFORM_INTERFACES

    )
endif()

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_link_libraries(${PROJECT_NAME} Core Logging "-framework Metal" Vulkan::Vulkan ${DEPENDENCIES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Public/ ${PLATFORM_INTERFACES})
