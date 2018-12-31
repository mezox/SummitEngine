#pragma once

#include "Renderer.h"

namespace Renderer
{
	class VulkanRenderer : public IRenderer
	{
	public:
		void Initialize() override;
		void Deinitialize() override;

		std::shared_ptr<IDevice> CreateDevice(DeviceType type) override;
		void CreateSwapChain(std::unique_ptr<RendererResource>& swapChain, std::unique_ptr<RendererResource>& surface, uint32_t width, uint32_t height) const {}

		void CreateWindowSurface(std::unique_ptr<RendererResource>& surface, void* nativeHandle) const override;

	private:
	};
}