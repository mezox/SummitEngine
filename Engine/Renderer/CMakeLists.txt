cmake_minimum_required(VERSION 3.6.0)

project(Renderer)

# Module export definition
if (${BUILD_SHARED_LIBS})
	add_definitions("-D_RENDERER_EXPORT")
endif()

# Include directories
include_directories(
	"Public"
	"Private"
)

# Platform agnostic dependencies
set(EXTERNAL_DEPENDENCIES
	Microprofile
)

set(DEPENDENCIES
    Logging
    Math
    RenderAPI
    Event
)

# platform agnostic source files
set(PUBLIC_SOURCES
	Public/Renderer/SwapChain.h
    Public/Renderer/Renderer.h
    Public/Renderer/VertexBuffer.h
    Public/Renderer/DeviceObject.h
)

set(PRIVATE_SOURCES
    Private/VertexBuffer.cpp
    Private/Vulkan/VulkanRendererImpl.h
	Private/Vulkan/VulkanRendererImpl.cpp
	Private/Vulkan/VulkanSwapChainImpl.h
	Private/Vulkan/VulkanSwapChainImpl.cpp
	Private/Vulkan/VulkanDeviceObjects.h
)

#set(PRECOMPILED_SOURCE Private/pch.cpp)
#set(PRECOMPILED_HEADER Private/pch.h)

configure_platform_file(${PROJECT_NAME} Public/Renderer  ${PUBLIC_SOURCES})

add_library(${PROJECT_NAME}
	${PUBLIC_SOURCES}
	${PRIVATE_SOURCES}
	#${PRECOMPILED_SOURCE}
	#${PRECOMPILED_HEADER}
)

#target_set_precompiled_header(${PROJECT_NAME} ${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE})
target_include_directories(${PROJECT_NAME} INTERFACE "Public")
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${EXTERNAL_DEPENDENCIES})

ide_source_files_group( ${PUBLIC_SOURCES}
                        ${PRIVATE_SOURCES}
                        ${PRECOMPILED_SOURCE}
                        ${PRECOMPILED_HEADER}
)
