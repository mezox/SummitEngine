#pragma once

#include <PAL/RenderAPI/Vulkan/VulkanAPI.h>

namespace PAL::RenderAPI
{
	class VulkanImplMacOS final : public VulkanRenderAPI
	{
	public:
		VkSurfaceKHR CreateWindowSurface(void* nativeHandle) const override;

	private:
		void PlatformInitialize() override;
		void PlatformDeinitialize() override;
		void PlatformLoadInstanceExtensions() override;

	private:
		// VK_MVK_macos_surface extension
		PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK{ nullptr };
	};
}
