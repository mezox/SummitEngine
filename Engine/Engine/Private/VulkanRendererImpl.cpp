#include "VulkanRendererImpl.h"

#include <PAL/RenderAPI/VulkanAPI.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include <Logging/LoggingService.h>

using namespace Renderer;

namespace Renderer
{
	std::unique_ptr<IRenderer> CreateRenderer()
	{
		return std::make_unique<VulkanRenderer>();
	}

	class VulkanWindowSurface : public RendererResource
	{
	public:
		explicit VulkanWindowSurface(const VkSurfaceKHR& surface)
			: mSurface(surface)
		{}

		~VulkanWindowSurface()
		{
			PAL::RenderAPI::VulkanAPIServiceLocator::Service().DestroySurface(mSurface);
		}

		const VkSurfaceKHR& GetVulkanSurface() const { return mSurface; }

	private:
		VkSurfaceKHR mSurface;
	};
    
    std::unique_ptr<IRenderer> RendererServiceLocator::mService = nullptr;
}

void VulkanRenderer::Initialize()
{
	const auto& vulkanAPI = PAL::RenderAPI::VulkanAPIServiceLocator::Service();

	const auto instanceExt = vulkanAPI.EnumerateInstanceExtensionProperties();

	LOG(Information) << instanceExt.size() << " instance extensions available:";
	for (const auto& extension : instanceExt)
	{
		LOG(Information) << extension.extensionName;
	}

	const auto instanceLayers = vulkanAPI.EnumerateInstanceLayerProperties();

	LOG(Information) << instanceLayers.size() << " instance layers available:";
	for (const auto& layer : instanceLayers)
	{
		LOG(Information) << layer.layerName;
	}

	const auto physicalDevices = vulkanAPI.EnumeratePhysicalDevices();
	const auto deviceExt = vulkanAPI.EnumerateDeviceExtensionProperties(physicalDevices.front());

	LOG(Information) << "---------------Device extensions:--------------";
	for (const auto& ext : deviceExt)
	{
		LOG(Information) << ext.extensionName << ", v: " << VK_VERSION_MAJOR(ext.specVersion) << "."
			<< VK_VERSION_MAJOR(ext.specVersion) << "."
			<< VK_VERSION_PATCH(ext.specVersion);
	}
}

void Renderer::VulkanRenderer::Deinitialize()
{
}

std::shared_ptr<IDevice> VulkanRenderer::CreateDevice(DeviceType type)
{
	const auto& vulkanAPI = PAL::RenderAPI::VulkanAPIServiceLocator::Service();

	const auto physicalDevices = vulkanAPI.EnumeratePhysicalDevices();
	const auto& physicalDevice = physicalDevices.front();
	const auto queueProps = vulkanAPI.GetPhysicalDeviceQueueFamilyProperties(physicalDevice);

	std::vector<float> queuePriorities{ 1.0f };

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueCount = static_cast<uint32_t>(queuePriorities.size());
	queueCreateInfo.queueFamilyIndex = 0;
	queueCreateInfo.pQueuePriorities = queuePriorities.data();

	PAL::RenderAPI::DeviceData deviceData;
	deviceData.device = physicalDevice;
	deviceData.deviceExtensions = vulkanAPI.EnumerateDeviceExtensionProperties(physicalDevice);
	deviceData.deviceProcAddrFunc = vulkanAPI.vkGetDeviceProcAddr;
	deviceData.deviceFeatures = vulkanAPI.GetPhysicalDeviceFeatures(physicalDevice);
	deviceData.deviceProperties = vulkanAPI.GetPhysicalDeviceProperties(physicalDevice);

	std::vector<const char*> mEnabledDeviceExtensions;
	std::vector<const char*> mEnabledDeviceValidationLayers;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.pEnabledFeatures = &deviceData.deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mEnabledDeviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = mEnabledDeviceExtensions.data();
	deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(mEnabledDeviceValidationLayers.size());
	deviceCreateInfo.ppEnabledLayerNames = mEnabledDeviceValidationLayers.data();

	deviceData.logicalDevice = vulkanAPI.CreateDevice(physicalDevice, deviceCreateInfo);

	auto data = std::make_shared<PAL::RenderAPI::VulkanDevice>(deviceData);
	return nullptr;
}

void VulkanRenderer::CreateWindowSurface(std::unique_ptr<RendererResource>& surface, void * nativeHandle) const
{
	const auto& vulkanAPI = PAL::RenderAPI::VulkanAPIServiceLocator::Service();
	const auto vulkanSurface = vulkanAPI.CreateWindowSurface(nativeHandle);
	surface = std::make_unique<VulkanWindowSurface>(vulkanSurface);
}

//void VulkanRenderer::CreateSwapChain(std::unique_ptr<RendererResource>& swapChain, std::unique_ptr<RendererResource>& surface, uint32_t width, uint32_t height) const
//{
//	const auto& physicalDevice = mPhysicalDevices.back(); //TODO: pull device info from somewhere not just last
//	const auto& vulkanSurface = static_cast<VulkanWindowSurface*>(surface.get())->GetVulkanSurface();
//
//
//
//
//}
