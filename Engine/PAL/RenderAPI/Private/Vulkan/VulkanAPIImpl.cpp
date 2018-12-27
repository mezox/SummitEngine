#include <Vulkan/VulkanAPIImpl.h>
#include <Vulkan/VulkanLoaderHelper.h>
#include <Vulkan/VulkanDebug.inl>
#include <Logging/LoggingService.h>
#include <PAL/FileSystem/File.h>
#include <nlohmann/json.hpp>

#include <dlfcn.h>
#include <cstring>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('V','L','K',' ')

#ifndef _WIN32
#define GetProcAddress dlsym
#endif

using json = nlohmann::json;

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
    
    std::shared_ptr<IRenderAPI> CreateRenderAPI()
    {
        return std::make_shared<VulkanRenderAPI>();
    }
    
    class VulkanDevice: public IDevice
    {
    public:
        
    public:
    };
    
    VulkanRenderAPI::~VulkanRenderAPI()
    {}
    
    VulkanRenderAPI::VulkanRenderAPI()
    {}
    
    void VulkanRenderAPI::Initialize()
    {
        using namespace PAL::FileSystem;
        
        mVulkanLibrary = dlopen("libvulkan.dylib", RTLD_NOW | RTLD_LOCAL);
        if (!mVulkanLibrary)
        {
            LOG(Warning) << "Could not load Vulkan library!";
            return;
        }
        
        LoadGlobalFunctions();
        
        mAvailableInstanceExtensions = QueryInstanceExtensions();
        mAvailableInstanceLayers = QueryInstanceLayers();
        
        File configFile("/Users/tomaskubovcik/Dev/SummitEngine/vk_config.json");
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
        
        if(IsExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
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
        
        mPhysicalDevices = QueryPhysicalDevices();
        
        LOG(Information) << "Available " << mPhysicalDevices.size() << " GPU devices:";
        
        for (const auto& device : mPhysicalDevices)
        {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            
            LOG(Information) << deviceProperties.deviceName
            << ", supported vulkan API version: "
            << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
            << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
            << VK_VERSION_PATCH(deviceProperties.apiVersion);
        }
    }
    
    void VulkanRenderAPI::DeInitialize()
    {
        mAvailableInstanceExtensions.clear();
        mAvailableInstanceLayers.clear();
        mEnabledInstanceExtensions.clear();
        mEnabledInstanceValidationLayers.clear();
        
        vkDestroyInstance(mInstance, nullptr);
        dlclose(mVulkanLibrary);
    }

    bool VulkanRenderAPI::IsExtensionEnabled(const char* extensionName) const
    {
        return std::find(mEnabledInstanceExtensions.begin(), mEnabledInstanceExtensions.end(), extensionName) != mEnabledInstanceExtensions.end();
    }
    
    void VulkanRenderAPI::OnDeviceConnected()
    {
        mPhysicalDevices = QueryPhysicalDevices();
    }
    
    void VulkanRenderAPI::LoadGlobalFunctions()
    {
        vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(mVulkanLibrary, "vkGetInstanceProcAddr");
        if(!vkGetInstanceProcAddr)
        {
            throw no_entry_point_func("vkGetInstanceProcAddr not found!");
        }
        
        LOAD_VK_GLOBAL_LEVEL_FUNCTION(vkCreateInstance);
        LOAD_VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceExtensionProperties);
        LOAD_VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceLayerProperties);
    }
    
    void VulkanRenderAPI::LoadInstanceFunctions()
    {
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkDestroyInstance);
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkEnumeratePhysicalDevices);
        LOAD_VK_INSTANCE_LEVEL_FUNCTION(mInstance, vkGetPhysicalDeviceProperties);
    }
    
    void VulkanRenderAPI::LoadInstanceExtensions()
    {
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
    }
    
    std::vector<VkExtensionProperties> VulkanRenderAPI::QueryInstanceExtensions() const
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        
        LOG(Information) << extensions.size() << " instance extensions available:";
        for (const auto& extension : extensions)
        {
            LOG(Information) << extension.extensionName;
        }
        
        return extensions;
    }
    
    std::vector<VkLayerProperties> VulkanRenderAPI::QueryInstanceLayers() const
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        LOG(Information) << availableLayers.size() << " instance layers available:";
        for (const auto& layer : availableLayers)
        {
            LOG(Information) << layer.layerName;
        }
        
        return availableLayers;
    }
    
    std::vector<VkPhysicalDevice> VulkanRenderAPI::QueryPhysicalDevices() const
    {
        uint32_t deviceCount{ 0 };
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        VK_CHECK_RESULT(vkEnumeratePhysicalDevices(mInstance, &deviceCount, physicalDevices.data()));
        
        return physicalDevices;
    }
    
    std::shared_ptr<IDevice> VulkanRenderAPI::CreateDevice(DeviceType type)
    {
        return nullptr;
    }
}
