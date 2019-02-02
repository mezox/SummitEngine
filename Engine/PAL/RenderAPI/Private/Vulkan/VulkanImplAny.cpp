#include <PAL/RenderAPI/VulkanAPI.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include <Vulkan/VulkanLoaderHelper.h>
#include <Logging/LoggingService.h>
#include <PAL/FileSystem/File.h>
#include <nlohmann/json.hpp>

#include <cstring>
#include <memory>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('V','L','K',' ')

using json = nlohmann::json;

using namespace PAL::FileSystem;

namespace
{
    void FilterLayers(const std::vector<VkLayerProperties>& available, const std::vector<std::string>& requested, std::vector<const char*>& enabled)
    {
        for(const auto layer : requested)
        {
            const auto result = std::find_if(available.begin(), available.end(), [layer](const VkLayerProperties& layerProp){
                return strcmp(layer.c_str(), layerProp.layerName) == 0;
            });
            
            if(result == available.end())
            {
                LOG(Warning) << "Validation layer: " << layer << " is not available!";
            }
            else
            {
                enabled.push_back(result->layerName);
            }
        }
    }
    
    void FilterExtensions(const std::vector<VkExtensionProperties>& available, const std::vector<std::string>& requested, std::vector<const char*>& enabled)
    {
        for(const auto extension : requested)
        {
            const auto result = std::find_if(available.begin(), available.end(), [extension](const VkExtensionProperties& extProp){
                return strcmp(extension.c_str(), extProp.extensionName) == 0;
            });
            
            if(result == available.end())
            {
                LOG(Warning) << "Extension: " << extension << " is not available!";
            }
            else
            {
                enabled.push_back(result->extensionName);
            }
        }
    }
}

namespace PAL::RenderAPI
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        std::string msgOut = "[VK ";
        
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                msgOut += "Verbose ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                msgOut += "Info ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                msgOut += "Warning ";
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                msgOut += "Error ";
                break;
            default:
                break;
        }
        
        switch (messageType)
        {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                msgOut += "General";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                msgOut += "Performance";
                break;
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                msgOut += "Validation";
                break;
            default:
                break;
        }
        
        LOG(Debug) << msgOut << "] " << pCallbackData->pMessage;
        
        return VK_FALSE;
    }

    
    struct VulkanSettingsImpl
    {
        std::vector<std::string> layers;
        std::vector<std::string> extensions;
    };
    
    void from_json(const json& jsonObject, VulkanSettingsImpl& settings)
    {
        jsonObject["validation_layers"].get_to(settings.layers);
        jsonObject["extensions"].get_to(settings.extensions);
    }
    
    std::string ToString(VkResult result)
    {
        switch (result)
        {
            case VK_SUCCESS: return "Success";
            case VK_ERROR_INCOMPATIBLE_DRIVER: return "Incompatible driver";
            case VK_ERROR_LAYER_NOT_PRESENT: throw vk_error("Requested validation layer is not available");
            default:
                break;
        }
        
        return "Unknow error";
    }
    
    std::unique_ptr<VulkanRenderAPI> VulkanAPIServiceLocator::mService = nullptr;
    
    VulkanRenderAPI::~VulkanRenderAPI()
    {
		DeInitialize();
	}
    
    VulkanRenderAPI::VulkanRenderAPI()
    {
	}
    
    void VulkanRenderAPI::Initialize()
    {
		if (mIsInitialized)
			return;

		PlatformInitialize();
        LoadGlobalFunctions();
        
        mAvailableInstanceExtensions = EnumerateInstanceExtensionProperties();
        mAvailableInstanceLayers = EnumerateInstanceLayerProperties();
        
#ifndef _WIN32
        File configFile("/Users/tomaskubovcik/Dev/SummitEngine/vk_config.json");
#else
		File configFile("C:/Users/Tomas/Dev/SummitEngine/vk_config.json");
#endif
        
        configFile.Open(EFileAccessMode::Read);
        if(configFile.IsOpened())
        {
            const auto data = configFile.Read();
            const VulkanSettingsImpl settings = json::parse(data);
            
            FilterLayers(mAvailableInstanceLayers, settings.layers, mEnabledInstanceValidationLayers);
            FilterExtensions(mAvailableInstanceExtensions, settings.extensions, mEnabledInstanceExtensions);
        }
        
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Summit Engine Demo";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "SummitEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        appInfo.pNext = nullptr;
        
        VkInstanceCreateInfo instanceInfo;
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(mEnabledInstanceExtensions.size());
        instanceInfo.ppEnabledExtensionNames = mEnabledInstanceExtensions.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(mEnabledInstanceValidationLayers.size());
        instanceInfo.ppEnabledLayerNames = mEnabledInstanceValidationLayers.data();
        instanceInfo.flags = 0;
        instanceInfo.pNext = nullptr;
        
        VK_CHECK_RESULT(vkCreateInstance(&instanceInfo, nullptr, &mInstance));
        
        LoadInstanceFunctions();
        LoadInstanceExtensions();
        
        if(IsExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) && !mEnabledInstanceValidationLayers.empty())
        {
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsInfo;
            debugUtilsInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugUtilsInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
            debugUtilsInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
            debugUtilsInfo.pfnUserCallback = debugCallback;
            debugUtilsInfo.pUserData = nullptr; // Optional
            debugUtilsInfo.flags = 0;
            
            VK_CHECK_RESULT(vkCreateDebugUtilsMessengerEXT(mInstance, &debugUtilsInfo, nullptr, &mCallback));
        }

		mIsInitialized = true;
    }
    
    void VulkanRenderAPI::DeInitialize()
    {
        if(mIsInitialized)
        {
            mAvailableInstanceExtensions.clear();
            mAvailableInstanceLayers.clear();
            mEnabledInstanceExtensions.clear();
            mEnabledInstanceValidationLayers.clear();
            
            vkDestroyInstance(mInstance, nullptr);
            PlatformDeinitialize();

            mIsInitialized = false;
        }
    }

    bool VulkanRenderAPI::IsExtensionEnabled(const char* extensionName) const
    {
		return std::find_if(mEnabledInstanceExtensions.begin(), mEnabledInstanceExtensions.end(), [extensionName](const char* ext) {
			return strcmp(extensionName, ext) == 0;
		}) != mEnabledInstanceExtensions.end();
    }

	std::vector<VkQueueFamilyProperties> VulkanRenderAPI::GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice& device) const
	{
		uint32_t queuePropertyCount{ 0 };
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queuePropertyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueProperties(queuePropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queuePropertyCount, queueProperties.data());

		return queueProperties;
	}

	VkPhysicalDeviceFeatures VulkanRenderAPI::GetPhysicalDeviceFeatures(const VkPhysicalDevice& physicalDevice) const
	{
		VkPhysicalDeviceFeatures features{ VK_NULL_HANDLE };
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		return features;
	}

	VkPhysicalDeviceProperties VulkanRenderAPI::GetPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice) const
	{
		VkPhysicalDeviceProperties properties{ VK_NULL_HANDLE };
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		return properties;
	}

	VkDevice VulkanRenderAPI::CreateDevice(const VkPhysicalDevice & physicalDevice, const VkDeviceCreateInfo & createInfo) const
	{
		VkDevice device{ VK_NULL_HANDLE };
		VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));
		return device;
	}

	void VulkanRenderAPI::DestroySurface(const VkSurfaceKHR& surface) const
	{
		vkDestroySurfaceKHR(mInstance, surface, nullptr);
	}
    
    void VulkanRenderAPI::OnDeviceConnected()
    {
        //mPhysicalDevices = QueryPhysicalDevices();
    }
    
    void VulkanRenderAPI::LoadGlobalFunctions()
    {
        LOAD_VK_GLOBAL_LEVEL_FUNCTION(vkCreateInstance);
        LOAD_VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceExtensionProperties);
        LOAD_VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceLayerProperties);
    }
    
    void VulkanRenderAPI::LoadInstanceFunctions()
    {
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkDestroyInstance);
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkEnumeratePhysicalDevices);
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkGetPhysicalDeviceProperties);
		LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkEnumerateDeviceExtensionProperties);
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkEnumerateDeviceLayerProperties);
		LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkGetPhysicalDeviceQueueFamilyProperties);
		LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkGetPhysicalDeviceFeatures);
		LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkCreateDevice);
		LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkGetDeviceProcAddr);
    }
    
    void VulkanRenderAPI::LoadInstanceExtensions()
    {
		PlatformLoadInstanceExtensions();

        // VK_EXT_debug_utils
        if(IsExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkCreateDebugUtilsMessengerEXT);
            LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkDestroyDebugUtilsMessengerEXT);
        }
        else
        {
            LOG(Warn) << "Extension: " << VK_EXT_DEBUG_UTILS_EXTENSION_NAME << " not available.";
        }

		if (IsExtensionEnabled(VK_KHR_SURFACE_EXTENSION_NAME))
		{
			LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkDestroySurfaceKHR);
			LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
			LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkGetPhysicalDeviceSurfaceFormatsKHR);
			LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkGetPhysicalDeviceSurfacePresentModesKHR);
			LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(mInstance, vkGetPhysicalDeviceSurfaceSupportKHR);
		}
		else
		{
			LOG(Warn) << "Extension: " << VK_KHR_SURFACE_EXTENSION_NAME << " not available.";
		}
    }
    
    std::vector<VkExtensionProperties> VulkanRenderAPI::EnumerateInstanceExtensionProperties() const
    {
		uint32_t extensionCount{ 0 };
		VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
        
        std::vector<VkExtensionProperties> extensions(extensionCount);
		VK_CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()));
        
        return extensions;
    }
    
    std::vector<VkLayerProperties> VulkanRenderAPI::EnumerateInstanceLayerProperties() const
    {
        uint32_t layerCount;
		VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
        
        std::vector<VkLayerProperties> availableLayers(layerCount);
		VK_CHECK_RESULT(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));
        
        return availableLayers;
    }
    
    std::vector<VkPhysicalDevice> VulkanRenderAPI::EnumeratePhysicalDevices() const
    {
        uint32_t deviceCount{ 0 };
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mInstance, &deviceCount, physicalDevices.data()));
        
        return physicalDevices;
    }

	std::vector<VkExtensionProperties> VulkanRenderAPI::EnumerateDeviceExtensionProperties(const VkPhysicalDevice& device) const
	{
		uint32_t propertyCount{ 0 };
		VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, nullptr));

		std::vector<VkExtensionProperties> deviceExtensions(propertyCount);
		VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, deviceExtensions.data()));

		return deviceExtensions;
	}
    
    std::vector<VkLayerProperties> VulkanRenderAPI::EnumerateDeviceLayerProperties(const VkPhysicalDevice& device) const
    {
        uint32_t layerCount{ 0 };
        VK_CHECK_RESULT(vkEnumerateDeviceLayerProperties(device, &layerCount, nullptr));
        
        std::vector<VkLayerProperties> deviceLayers(layerCount);
        VK_CHECK_RESULT(vkEnumerateDeviceLayerProperties(device, &layerCount, deviceLayers.data()));
        
        return deviceLayers;
    }

	std::vector<VkSurfaceFormatKHR> VulkanRenderAPI::GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const
	{
		uint32_t surfaceFormatCount{ 0 };
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, nullptr));

		std::vector<VkSurfaceFormatKHR> formats(surfaceFormatCount);
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &surfaceFormatCount, formats.data()));

		return formats;
	}

	std::vector<VkPresentModeKHR> VulkanRenderAPI::GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const
	{
		uint32_t presentationModesCount{ 0 };
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModesCount, nullptr));
		
		std::vector<VkPresentModeKHR> presentModes(presentationModesCount);
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationModesCount, presentModes.data()));

		return presentModes;
	}

	VkBool32 VulkanRenderAPI::GetPhysicalDeviceSurfaceSupportKHR(const VkPhysicalDevice& device, uint32_t queueFamilyIndex, const VkSurfaceKHR& surface) const
	{
		VkBool32 supported{ VK_FALSE };
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex, surface, &supported));
		return supported;
	}

	VkSurfaceCapabilitiesKHR VulkanRenderAPI::GetPhysicalDeviceSurfaceCapabilitiesKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const
	{
		VkSurfaceCapabilitiesKHR caps{ VK_NULL_HANDLE };
		VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &caps));
		return caps;
	}
}
