//#include "VulkanSwapChainImpl.h"
//#include "VulkanImpl.h"
//
//using namespace Renderer;
//using namespace PAL::RenderAPI;
//
//SwapChainVK::SwapChainVK(std::shared_ptr<const PAL::RenderAPI::IRenderAPI> vulkanWrapper)
//	: RendererResource(std::move(vulkanWrapper))
//{}
//
//SwapChainVK::~SwapChainVK()
//{}
//
//void SwapChainVK::Initialize(const uint32_t width, const uint32_t height)
//{
//	//const auto surface = LowVK::GetSurface();
//
//	const auto& vulkanAPI = *std::static_pointer_cast<PAL::RenderAPI::VulkanRenderAPI>(RenderAPIServiceLocator::Service());
//
//	const auto caps = vulkanAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vulkanSurface);
//	const auto formats = vulkanAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vulkanSurface);
//	const auto presentModes = vulkanAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vulkanSurface);
//	const auto supported = vulkanAPI.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 0, vulkanSurface);
//
//	if (supported)
//	{
//		const auto& format = mFormats.front();      // Get B8G8R8A8_unorm
//		const auto& presentMode = mPresentationModes.front();   // Immeadiate
//
//		VkExtent2D screenExtent;
//		screenExtent.width = width;
//		screenExtent.height = height;
//
//		auto imagesCount = mCapabilities.minImageCount + 1;
//
//		if (1/*mEnableTrippleBuffering*/)
//		{
//			if (mCapabilities.maxImageCount > 0 && imagesCount > mCapabilities.maxImageCount)
//			{
//				imagesCount = mCapabilities.maxImageCount;
//			}
//		}
//
//		VkSwapchainCreateInfoKHR createInfo{};
//		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//		createInfo.surface = surface;
//		createInfo.minImageCount = imagesCount;
//		createInfo.imageColorSpace = format.colorSpace;
//		createInfo.imageFormat = format.format;
//		createInfo.imageExtent = screenExtent;
//		createInfo.imageArrayLayers = 1;
//		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //Use transfer if I want to render to other buffer and apply post process and then copy;
//		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		createInfo.queueFamilyIndexCount = 0; // Optional
//		createInfo.pQueueFamilyIndices = nullptr; // Optional
//		createInfo.preTransform = mCapabilities.currentTransform;
//		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//		createInfo.presentMode = presentMode;
//		createInfo.clipped = VK_TRUE;
//		createInfo.oldSwapchain = VkSwapchainKHR{};
//
//		LowVK::CreateSwapchainKHR(&createInfo, nullptr, &mHandle);
//		LowVK::GetSwapchainImagesKHR(mHandle, mImages);
//
//		mImageViews.resize(mImages.size());
//
//		for (size_t idx{ 0 }; idx < mImageViews.size(); ++idx)
//		{
//			VkImageViewCreateInfo imageViewCreateInfo{};
//			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//			imageViewCreateInfo.image = mImages[idx];
//			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//			imageViewCreateInfo.format = format.format;
//			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
//			imageViewCreateInfo.subresourceRange.levelCount = 1;
//			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
//			imageViewCreateInfo.subresourceRange.layerCount = 1;
//
//			LowVK::CreateImageView(&imageViewCreateInfo, nullptr, &mImageViews[idx]);
//		}
//	}
//}
//
//void SwapChainVK::Destroy()
//{
//	const auto& vulkanAPI = *std::static_pointer_cast<PAL::RenderAPI::VulkanRenderAPI>(RenderAPIServiceLocator::Service());
//
//
//
//	for (auto& imageView : mImageViews)
//	{
//		LowVK::DestroyImageView(imageView, nullptr);
//	}
//
//	for (auto& image : mImages)
//	{
//		LowVK::DestroyImage(image, nullptr);
//	}
//
//	LowVK::DestroySwapchainKHR(mHandle, nullptr);
//}
//
//void SwapChainVK::SetSemaphore(const uint32_t width, const uint32_t height, VkQueue present, VkSemaphore renderFinished, VkSemaphore imgAvailable, VkRenderPass pass)
//{
//	mSemaphore = renderFinished;
//	mPresentQueue = present;
//	mImgAvailable = imgAvailable;
//	mRenderPass = pass;
//
//	mFramebuffers.resize(mImages.size());
//
//	mDepthTexture = std::make_unique<Texture>(ImageFormat::DEPTH, ImageUsage::DepthAttachment, width, height);
//
//	for (size_t i = 0; i < mImages.size(); i++)
//	{
//		auto depthTex = (RendererTextureVK*)mDepthTexture->GetRendererTexture();
//
//		std::vector<VkImageView> attachments{ mImageViews[i], depthTex->imageView };
//
//		VkFramebufferCreateInfo framebufferInfo{};
//		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//		framebufferInfo.renderPass = mRenderPass;
//		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//		framebufferInfo.pAttachments = attachments.data();
//		framebufferInfo.width = width;
//		framebufferInfo.height = height;
//		framebufferInfo.layers = 1;
//
//		LowVK::CreateFramebuffer(&framebufferInfo, nullptr, &mFramebuffers[i]);
//	}
//}
//
//uint32_t SwapChainVK::SwapBuffers()
//{
//	LowVK::AcquireNextImageKHR(mHandle, std::numeric_limits<uint64_t>::max(), mImgAvailable, {}, &mImageIndex);
//	return mImageIndex;
//}
//
//void SwapChainVK::Present()
//{
//	VkPresentInfoKHR presentInfo{};
//	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//	presentInfo.waitSemaphoreCount = 1;
//	presentInfo.pWaitSemaphores = &mSemaphore;
//
//	presentInfo.swapchainCount = 1;
//	presentInfo.pSwapchains = &mHandle;
//	presentInfo.pImageIndices = &mImageIndex;
//	presentInfo.pResults = nullptr; // Optional
//
//	LowVK::QueuePresentKHR(mPresentQueue, &presentInfo);
//}
//
//const uint8_t SwapChainVK::GetImageCount() const
//{
//	return static_cast<uint8_t>(mImages.size());
//}