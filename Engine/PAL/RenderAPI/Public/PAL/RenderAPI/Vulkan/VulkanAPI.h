#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

#include <Core/Handle.h>
#include <Core/Platform.h>

#include <vector>

namespace PAL::RenderAPI
{
    /*!
     @brief vk_error exception thrown in case of runtime vulkan error.
     */
    struct RENDERAPI_API vk_error : std::runtime_error { using std::runtime_error::runtime_error; };
    
    /*!
     @brief no_extension exception thrown if requested extension is not present.
     */
    struct RENDERAPI_API no_extension : std::runtime_error { using std::runtime_error::runtime_error; };
    
    struct RENDERAPI_API no_instance_func : std::runtime_error { using std::runtime_error::runtime_error; };
    struct RENDERAPI_API no_entry_point_func : std::runtime_error { using std::runtime_error::runtime_error; };
    struct RENDERAPI_API no_instance_func_ext : std::runtime_error { using std::runtime_error::runtime_error; };
    
    class RENDERAPI_API VulkanRenderAPI
    {
    public:
        VulkanRenderAPI() = default;
        virtual ~VulkanRenderAPI();
        
        void Initialize();
        void DeInitialize();
        
        /*!
         @brief Check whether requested extension is enabled.
         @param extensionName Extension name.
         @return True if extension is enabled, false otherwise.
         */
        NO_DISCARD const bool IsExtensionEnabled(const char* extensionName) const;

        /*!
         @brief Query instance extensions.
         @return Array of instance extension properties.
         */
		NO_DISCARD std::vector<VkExtensionProperties> EnumerateInstanceExtensionProperties() const;
        
        /*!
         @brief Query instance validation layers.
         @param Array of instance validation layer properties.
         */
		NO_DISCARD std::vector<VkLayerProperties> EnumerateInstanceLayerProperties() const;
        
        /*!
         @brief Query physical devices supporting vulkan.
         @param Array of physical devices and their properties.
         */
		NO_DISCARD std::vector<VkPhysicalDevice> EnumeratePhysicalDevices() const;
        
        /*!
         @brief Query physical device's supported extensions.
         @param device Physical device.
         @return Array of supported extension properties.
         */
		NO_DISCARD std::vector<VkExtensionProperties> EnumerateDeviceExtensionProperties(const VkPhysicalDevice& device) const;
        
        /*!
         @brief Query physical device's supported validation layers.
         @param device Physical device.
         @return Array of supported validation layers.
         */
        NO_DISCARD std::vector<VkLayerProperties> EnumerateDeviceLayerProperties(const VkPhysicalDevice& device) const;

        /*!
         @brief Query queue family properties for given device. Has to be called before requesting queue handles.
         @param deice Physical device.
         @return Array of queue family properties.
         */
		NO_DISCARD std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice& device) const;
        
        /*!
         @brief Query physical device features.
         @param device Physical device.
         @return Physical device features.
         */
		NO_DISCARD VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& physicalDevice) const;
        
        /*!
         @brief Query physical device properties.
         @param device Physical device.
         @return Physical device properties.
         */
		NO_DISCARD VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice) const;

        /*!
         @brief Creates virtual representation of device (logical device).
         @param device Physical device.
         @param createInfo Properties & features of physical device to enable.
         @return Logical device handle.
         */
		NO_DISCARD VkDevice CreateDevice(const VkPhysicalDevice& physicalDevice, const VkDeviceCreateInfo& createInfo) const;

		NO_DISCARD VkSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;
		NO_DISCARD std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;
		NO_DISCARD std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;
		NO_DISCARD VkBool32 GetPhysicalDeviceSurfaceSupportKHR(const VkPhysicalDevice& device, uint32_t queueFamilyIndex, const VkSurfaceKHR& surface) const;
        void GetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) const;

		void DestroySurface(const VkSurfaceKHR& surface) const;

		// Platform specific
		NO_DISCARD virtual VkSurfaceKHR CreateWindowSurface(void* nativeHandle) const = 0;

        // slots
    private:
        void OnDeviceConnected();

        void LoadGlobalFunctions();
        void LoadInstanceFunctions();
        void LoadInstanceExtensions();

    protected:
		virtual void PlatformInitialize() = 0;
		virtual void PlatformDeinitialize() = 0;
		virtual void PlatformLoadInstanceExtensions() = 0;

    public:
		PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr{ nullptr };
        
    protected:
        void* mVulkanLibrary{ nullptr };
        MovableHandle<VkInstance> mInstance;
        MovableHandle<VkDebugUtilsMessengerEXT> mCallback;
        
        std::vector<VkExtensionProperties> mAvailableInstanceExtensions;
        std::vector<VkLayerProperties> mAvailableInstanceLayers;
        std::vector<VkPhysicalDevice> mPhysicalDevices;
        
        std::vector<const char*> mEnabledInstanceExtensions;
        std::vector<const char*> mEnabledInstanceValidationLayers;

		// Vulkan 1.0
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{ nullptr };
		PFN_vkCreateInstance vkCreateInstance{ nullptr };
		PFN_vkDestroyInstance vkDestroyInstance{ nullptr };
		PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties{ nullptr };
		PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties{ nullptr };
		PFN_vkCreateDevice vkCreateDevice{ nullptr };

		// Physical devices
		PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices{ nullptr };
		PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties{ nullptr };
		PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties{ nullptr };
        PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties{ nullptr };
		PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties{ nullptr };
		PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures{ nullptr };
        PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties{ nullptr };

		// VK_EXT_debug_utils
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ nullptr };
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ nullptr };

		// VK_KHR_surface
		PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR{ nullptr };
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR{ nullptr };
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR{ nullptr };
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR{ nullptr };
		PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR{ nullptr };
    };

    class RENDERAPI_API VulkanAPI
    {
    public:
        static void Provide(std::unique_ptr<VulkanRenderAPI> service)
        {
            mService = std::move(service);
        }
        
        static VulkanRenderAPI& Service()
        {
            if(!mService)
            {
                throw std::runtime_error("VulkanAPI service unitialized");
            }
            
            return *mService;
        }
        
        static bool Available()
        {
            return mService != nullptr;
        }
        
    private:
        static std::unique_ptr<VulkanRenderAPI> mService;
    };

	RENDERAPI_API std::unique_ptr<VulkanRenderAPI> CreateVulkanRenderAPI();
    RENDERAPI_API std::string ToString(VkResult result);
}
