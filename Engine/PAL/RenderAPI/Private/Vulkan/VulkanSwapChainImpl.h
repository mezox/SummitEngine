//#pragma once
//
//#include <PAL/RenderAPI/SwapChain.h>
//#include <PAL/RenderAPI/RenderAPIService.h>
//
//#include <vulkan/vulkan.h>
//#include <vector>
//#include <memory>
//
//namespace Renderer
//{
//	class SwapChainVK : public ISwapChain
//	{
//	public:
//		SwapChainVK(const std::shared_ptr<PAL::RenderAPI::IDevice>& device);
//		~SwapChainVK();
//
//		void Initialize(const uint32_t width, const uint32_t height) override;
//		void Destroy() override;
//
//		void SetSemaphore(const uint32_t width, const uint32_t height, VkQueue present, VkSemaphore renderFinished, VkSemaphore imgAvailable, VkRenderPass pass);
//
//		uint32_t SwapBuffers() override;
//		void Present()override;
//
//		const uint8_t GetImageCount() const override;
//
//		const VkSwapchainKHR& GetSwapChain() const { return mHandle; }
//		const std::vector<VkSurfaceFormatKHR>& GetFormats() const { return mFormats; }
//		const std::vector<VkImageView>& GetSwapImageViews() const { return mImageViews; }
//		const std::vector<VkFramebuffer>& GetFramebuffers() const { return mFramebuffers; }
//
//	private:
//		VkSwapchainKHR mHandle;
//		std::vector<VkImage> mImages;
//		std::vector<VkImageView> mImageViews;
//		std::vector<VkFramebuffer> mFramebuffers;
//
//		// Swap chain properties
//		VkSurfaceCapabilitiesKHR mCapabilities;
//		std::vector<VkSurfaceFormatKHR> mFormats;
//		std::vector<VkPresentModeKHR> mPresentationModes;
//
//		VkQueue mPresentQueue;
//		VkSemaphore mSemaphore;
//		VkSemaphore mImgAvailable;
//
//		VkRenderPass mRenderPass;
//
//		uint32_t mImageIndex{ 0 };
//
//		std::shared_ptr<PAL::RenderAPI::IDevice> mDevice;
//	};
//}