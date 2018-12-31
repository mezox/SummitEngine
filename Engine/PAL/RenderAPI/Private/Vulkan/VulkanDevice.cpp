#include <PAL/RenderAPI/VulkanAPI.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include "VulkanLoaderHelper.h"
#include <Logging/LoggingService.h>

#include <algorithm>

namespace PAL::RenderAPI
{
	VulkanDevice::VulkanDevice(DeviceData& deviceData)
		: mPhysicalDevice(deviceData.device)
		, mLogicalDevice(deviceData.logicalDevice)
		, mDeviceProperties(deviceData.deviceProperties)
		, mDeviceFeatures(deviceData.deviceFeatures)
		, mDeviceExtensions(std::move(deviceData.deviceExtensions))
	{
		LoadFunctions(deviceData.deviceProcAddrFunc);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(mLogicalDevice, nullptr);
	}

	void VulkanDevice::CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) const
	{
		VK_CHECK_RESULT(vkCreateSwapchainKHR(mLogicalDevice, pCreateInfo, pAllocator, pSwapchain));
	}

	void VulkanDevice::DestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) const
	{
		vkDestroySwapchainKHR(mLogicalDevice, swapchain, pAllocator);
	}

	void VulkanDevice::LoadFunctions(PFN_vkGetDeviceProcAddr loadFunc)
	{
		LOAD_VK_DEVICE_LEVEL_FUNCTION(mLogicalDevice, loadFunc, vkDestroyDevice);

		if (IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
		{
			LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkCreateSwapchainKHR);
			LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(mLogicalDevice, loadFunc, vkDestroySwapchainKHR);
		}
		else
		{
			LOG(Warn) << "Device Extension: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME << " not available.";
		}
	}

	bool VulkanDevice::IsExtensionSupported(const char * extensionName) const
	{
		return std::find_if(mDeviceExtensions.begin(), mDeviceExtensions.end(), [extensionName](const VkExtensionProperties& props) {
			return strcmp(extensionName, props.extensionName) == 0;
		}) != mDeviceExtensions.end();
	}
}
