#pragma once

#include <PAL/RenderAPI/RenderAPIService.h>
#include <vector>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>

namespace PAL::RenderAPI
{
    struct vk_error : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_extension : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_instance_func : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_entry_point_func : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_instance_func_ext : std::runtime_error { using std::runtime_error::runtime_error; };
    
    class VulkanRenderAPI : public IRenderAPI
    {
    public:
        VulkanRenderAPI();
        ~VulkanRenderAPI();
        
        void Initialize() override;
        std::shared_ptr<IDevice> CreateDevice(DeviceType type) override;
        void DeInitialize() override;
        
        bool IsExtensionEnabled(const char* extensionName) const;
        
        // slots
    private:
        void OnDeviceConnected();
        
    private:
        void LoadGlobalFunctions();
        void LoadInstanceFunctions();
        void LoadInstanceExtensions();
        
        std::vector<VkExtensionProperties> QueryInstanceExtensions() const;
        std::vector<VkLayerProperties> QueryInstanceLayers() const;
        std::vector<VkPhysicalDevice> QueryPhysicalDevices() const;
        
    private:
        void* mVulkanLibrary{ nullptr };
        VkInstance mInstance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT mCallback{ VK_NULL_HANDLE };
        
        std::vector<VkExtensionProperties> mAvailableInstanceExtensions;
        std::vector<VkLayerProperties> mAvailableInstanceLayers;
        std::vector<VkPhysicalDevice> mPhysicalDevices;
        
        std::vector<const char*> mEnabledInstanceExtensions;
        std::vector<const char*> mEnabledInstanceValidationLayers;
        
#if defined(VK_VERSION_1_0)
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{ nullptr };
        PFN_vkCreateInstance vkCreateInstance{ nullptr };
        PFN_vkDestroyInstance vkDestroyInstance{ nullptr };
        PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties{ nullptr };
        PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties{ nullptr };
        PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices{ nullptr };
        PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties{ nullptr };
#endif
        
#if defined(VK_EXT_debug_utils)
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
#endif
    };
}
