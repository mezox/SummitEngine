#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

#include <exception>

namespace PAL::RenderAPI
{
	struct no_device_func : std::runtime_error { using std::runtime_error::runtime_error; };

	struct RENDERAPI_API DeviceData
	{
		VkPhysicalDevice device{ VK_NULL_HANDLE };
		VkDevice logicalDevice{ VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures deviceFeatures{ VK_NULL_HANDLE };
		VkPhysicalDeviceProperties deviceProperties{ VK_NULL_HANDLE };
		std::vector<VkExtensionProperties> deviceExtensions;
		PFN_vkGetDeviceProcAddr deviceProcAddrFunc{ nullptr };
	};

	class RENDERAPI_API VulkanDevice
	{
	public:
		explicit VulkanDevice(DeviceData& deviceData);
		~VulkanDevice();

		void CreateSwapchainKHR(const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain) const;
		void DestroySwapchainKHR(VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator) const;

	private:
		void LoadFunctions(PFN_vkGetDeviceProcAddr loadFunc);
		bool IsExtensionSupported(const char* extensionName) const;

	private:
		VkPhysicalDevice mPhysicalDevice{ VK_NULL_HANDLE };
		VkDevice mLogicalDevice{ VK_NULL_HANDLE };
		VkPhysicalDeviceProperties mDeviceProperties{ VK_NULL_HANDLE };
		VkPhysicalDeviceFeatures mDeviceFeatures{ VK_NULL_HANDLE };
		std::vector<VkExtensionProperties> mDeviceExtensions;

		PFN_vkDestroyDevice vkDestroyDevice{ nullptr };

		// VK_KHR_swapchain
		PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR{ nullptr };
		PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR{ nullptr };
	};
}