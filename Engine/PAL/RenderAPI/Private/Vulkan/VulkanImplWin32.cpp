#include "Vulkan/VulkanImplWin32.h"
#include "Vulkan/VulkanLoaderHelper.h"

#include <Logging/LoggingService.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vulkan/vulkan_win32.h>

using namespace PAL::RenderAPI;

void VulkanImplWin32::PlatformInitialize()
{
	mVulkanLibrary = LoadLibrary("vulkan-1.dll");
	if (!mVulkanLibrary)
	{
		throw vk_error("Could not load Vulkan library!");
	}

	vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(static_cast<HMODULE>(mVulkanLibrary), "vkGetInstanceProcAddr");
	if (!vkGetInstanceProcAddr)
	{
		throw no_entry_point_func("vkGetInstanceProcAddr not found!");
	}
}

void VulkanImplWin32::PlatformLoadInstanceExtensions()
{
	if (IsExtensionEnabled(VK_KHR_WIN32_SURFACE_EXTENSION_NAME))
	{
		LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkCreateWin32SurfaceKHR);
	}
	else
	{
		LOG(Warn) << "Extension: " << VK_KHR_WIN32_SURFACE_EXTENSION_NAME << " not available.";
	}
}

VkSurfaceKHR VulkanImplWin32::CreateWindowSurface(void* nativeHandle) const
{
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = static_cast<HWND>(nativeHandle);
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
	surfaceCreateInfo.pNext = nullptr;
	surfaceCreateInfo.flags = 0;

	VkSurfaceKHR vulkanSurface{ VK_NULL_HANDLE };
	VK_CHECK_RESULT(vkCreateWin32SurfaceKHR(mInstance, &surfaceCreateInfo, nullptr, &vulkanSurface));

	return vulkanSurface;
}

void VulkanImplWin32::PlatformDeinitialize()
{
	if (mVulkanLibrary)
	{
		FreeLibrary(static_cast<HMODULE>(mVulkanLibrary));
	}
}

std::shared_ptr<VulkanRenderAPI> PAL::RenderAPI::CreateVulkanRenderAPI()
{
	return std::make_shared<VulkanImplWin32>();
}