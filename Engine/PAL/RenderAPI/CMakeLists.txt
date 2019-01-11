cmake_minimum_required(VERSION 3.12)

project(RenderAPI)

if (${BUILD_SHARED_LIBS})
	add_definitions("-D_RENDERAPI_EXPORT")
endif()

set(VULKAN_SDK "/Users/tomaskubovcik/Dev/sdks/vulkansdk/macOS")
#set(Vulkan_LIBRARY ${VULKAN_SDK}/lib)
#set(Vulkan_INCLUDE_DIR ${VULKAN_SDK}/include)

find_package(vulkan REQUIRED)

include_directories(
	"Public"
	"Private"
)

# platform agnostic source files
set(PRIVATE_SOURCES
	Private/Vulkan/VulkanLoaderHelper.h
	Private/Vulkan/VulkanDevice.cpp

	Private/Vulkan/VulkanSwapChainImpl.h
	Private/Vulkan/VulkanSwapChainImpl.cpp

	Private/Vulkan/VulkanImplAny.cpp
)

set(PUBLIC_SOURCES
	Public/PAL/RenderAPI/SwapChain.h
	Public/PAL/RenderAPI/VulkanAPI.h
	Public/PAL/RenderAPI/VulkanDevice.h
)

# Platform agnostic dependencies
set(DEPENDENCIES
    # External
    nlohmann_json
    Vulkan::Vulkan

    # Internal
    FileSystem
    Logging
)

# platform dependant settings
if (PLATFORM_APPLE_MACOS)
	add_definitions("-DVK_USE_PLATFORM_MACOS_MVK")

	set(OBJECTIVE_C_SOURCES
		#Private/Metal/MetalAPIImpl.h
		#Private/Metal/MetalAPIImpl.cpp
	)

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
		Private/Vulkan/VulkanImplMacOS.h
		Private/Vulkan/VulkanImplMacOS.cpp
	)

	set_objective_c_compilation(${OBJECTIVE_C_SOURCES})

    set(PLATFORM_INTERFACES

    )
elseif (PLATFORM_APPLE_IOS)
	add_definitions("-DVK_USE_PLATFORM_IOS_MVK")

	list(APPEND PRIVATE_SOURCES
		${OBJECTIVE_C_SOURCES}
		Private/Vulkan/VulkanImplIOS.h
		Private/Vulkan/VulkanImplIOS.cpp
	)

elseif(PLATFORM_WINDOWS)
	add_definitions("-DVK_USE_PLATFORM_WIN32_KHR")

	list(APPEND PRIVATE_SOURCES
		Private/Vulkan/VulkanImplWin32.cpp
		Private/Vulkan/VulkanImplWin32.h
	)
endif()

configure_platform_file(${PROJECT_NAME} Public/PAL/RenderAPI "${PUBLIC_SOURCES}")

add_library(${PROJECT_NAME} ${PUBLIC_SOURCES} ${PRIVATE_SOURCES})
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Public/ ${PLATFORM_INTERFACES})
