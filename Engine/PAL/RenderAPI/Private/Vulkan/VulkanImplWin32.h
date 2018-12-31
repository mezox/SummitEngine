#pragma once

#include <PAL/RenderAPI/VulkanAPI.h>

namespace PAL::RenderAPI
{
	class VulkanImplWin32 : public VulkanRenderAPI
	{
	public:
		VkSurfaceKHR CreateWindowSurface(void* nativeHandle) const override;

	private:
		void PlatformInitialize() override;
		void PlatformDeinitialize() override;
		void PlatformLoadInstanceExtensions() override;

	private:
		// VK_KHR_win32_surface extension
		PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR{ nullptr };
	};
}