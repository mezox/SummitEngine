#include "Vulkan/VulkanImplIOS.h"
#include "Vulkan/VulkanLoaderHelper.h"
#include <Logging/LoggingService.h>

#include <vulkan/vulkan_ios.h>

#include <dlfcn.h>

using namespace PAL::RenderAPI;

void VulkanImplIOS::PlatformInitialize()
{
    mVulkanLibrary = dlopen("libMoltenVK.dylib", RTLD_NOW | RTLD_LOCAL);
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

void VulkanImplIOS::PlatformLoadInstanceExtensions()
{
	if (IsExtensionEnabled(VK_MVK_IOS_SURFACE_EXTENSION_NAME))
	{
		LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkCreateIOSSurfaceMVK);
	}
	else
	{
		LOG(Warn) << "Extension: " << VK_MVK_IOS_SURFACE_EXTENSION_NAME << " not available.";
	}
}

VkSurfaceKHR VulkanImplIOS::CreateWindowSurface(void* nativeHandle) const
{
    VkIOSSurfaceCreateInfoMVK surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.pView = nativeHandle;
    surfaceCreateInfo.flags = 0;
    
    VkSurfaceKHR vulkanSurface{ VK_NULL_HANDLE };
	VK_CHECK_RESULT(vkCreateIOSSurfaceMVK(mInstance, &surfaceCreateInfo, nullptr, &vulkanSurface));

	return vulkanSurface;
}

void VulkanImplIOS::PlatformDeinitialize()
{
	if (mVulkanLibrary)
	{
		dlclose(mVulkanLibrary);
	}
}

std::unique_ptr<VulkanRenderAPI> PAL::RenderAPI::CreateVulkanRenderAPI()
{
	return std::make_unique<VulkanImplIOS>();
}
