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
    Core
)

# platform agnostic source files
set(PUBLIC_SOURCES
	Public/Renderer/SwapChain.h
    Public/Renderer/Renderer.h
    Public/Renderer/VertexBuffer.h
    Public/Renderer/Image.h
    Public/Renderer/SharedDeviceTypes.h
    Public/Renderer/DeviceObject.h
    Public/Renderer/View.h
    Public/Renderer/Layer.h
    Public/Renderer/Effect.h
    Public/Renderer/RenderPass.h

    # resources
    Public/Renderer/Resources/DeviceResource.h
    Public/Renderer/Resources/Framebuffer.h
    Public/Renderer/Resources/Buffer.h
    Public/Renderer/Resources/Synchronization.h
)

set(PRIVATE_SOURCES
    Private/DeviceObjectImpl.h
    Private/DeviceObjectImpl.cpp
    Private/VertexBuffer.cpp
    Private/Image.cpp
    Private/Vulkan/VulkanCommandBuffer.h
    Private/Vulkan/VulkanCommandBuffer.cpp
    Private/Vulkan/VulkanGuiRenderer.h
	Private/Vulkan/VulkanGuiRenderer.cpp
    Private/Vulkan/VulkanRendererImpl.h
	Private/Vulkan/VulkanRendererImpl.cpp
	Private/Vulkan/VulkanSwapChainImpl.h
	Private/Vulkan/VulkanSwapChainImpl.cpp
	Private/Vulkan/VulkanDeviceObjects.h
	Private/View.cpp
    Private/Effect.cpp
    Private/Framebuffer.cpp
	Private/SwapChainBase.cpp
    Private/RenderPass.cpp

    Private/Vulkan/VulkanTypes.h
    Private/Vulkan/VulkanTypes.cpp


    Private/stb_image.h
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
