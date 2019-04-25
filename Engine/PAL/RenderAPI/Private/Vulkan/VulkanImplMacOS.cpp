#include "Vulkan/VulkanImplMacOS.h"
#include "Vulkan/VulkanLoaderHelper.h"
#include <Logging/LoggingService.h>

#include <vulkan/vulkan_macos.h>

#include <dlfcn.h>

using namespace PAL::RenderAPI;

void VulkanImplMacOS::PlatformInitialize()
{
    mVulkanLibrary = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
    //mVulkanLibrary = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
    if (!mVulkanLibrary)
    {
        throw vk_error("Could not load Vulkan library!");
    }
    
	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(mVulkanLibrary, "vkGetInstanceProcAddr");
	if (!vkGetInstanceProcAddr)
	{
		throw no_entry_point_func("vkGetInstanceProcAddr not found!");
	}
}

void VulkanImplMacOS::PlatformLoadInstanceExtensions()
{
	if (IsExtensionEnabled(VK_MVK_MACOS_SURFACE_EXTENSION_NAME))
	{
		LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance.Get(), vkCreateMacOSSurfaceMVK);
	}
	else
	{
		LOG(Warning) << "Extension: " << VK_MVK_MACOS_SURFACE_EXTENSION_NAME << " not available.";
	}
}

VkSurfaceKHR VulkanImplMacOS::CreateWindowSurface(void* nativeHandle) const
{
    VkMacOSSurfaceCreateInfoMVK surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.pView = nativeHandle;
    surfaceCreateInfo.flags = 0;
    
    VkSurfaceKHR vulkanSurface{ VK_NULL_HANDLE };
	VK_CHECK_RESULT(vkCreateMacOSSurfaceMVK(mInstance.Get(), &surfaceCreateInfo, nullptr, &vulkanSurface));

	return vulkanSurface;
}

void VulkanImplMacOS::PlatformDeinitialize()
{
	if (mVulkanLibrary)
	{
		dlclose(mVulkanLibrary);
	}
}

std::unique_ptr<VulkanRenderAPI> PAL::RenderAPI::CreateVulkanRenderAPI()
{
	return std::make_unique<VulkanImplMacOS>();
}
