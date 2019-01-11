#pragma once

#include <PAL/RenderAPI/VulkanAPI.h>

namespace PAL::RenderAPI
{
	class VulkanImplIOS : public VulkanRenderAPI
	{
	public:
		VkSurfaceKHR CreateWindowSurface(void* nativeHandle) const override;

	private:
		void PlatformInitialize() override;
		void PlatformDeinitialize() override;
		void PlatformLoadInstanceExtensions() override;

	private:
		// VK_MVK_icos_surface extension
		PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK{ nullptr };
	};
}
