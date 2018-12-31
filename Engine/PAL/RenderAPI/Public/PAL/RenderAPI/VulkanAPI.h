#pragma once

#include <PAL/RenderAPI/RenderAPIBase.h>
#include <Core/Service.h>

#ifndef VK_NO_PROTOTYPES
#   define VK_NO_PROTOTYPES
#endif
#include <vulkan/vulkan.h>

#include <vector>

namespace PAL::RenderAPI
{
	RENDERAPI_API std::string ToString(VkResult result);

    struct vk_error : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_extension : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_instance_func : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_entry_point_func : std::runtime_error { using std::runtime_error::runtime_error; };
    struct no_instance_func_ext : std::runtime_error { using std::runtime_error::runtime_error; };
    
    class RENDERAPI_API VulkanRenderAPI
    {
		friend class VulkanRenderer;

    public:
        VulkanRenderAPI();
        virtual ~VulkanRenderAPI();
        
        void Initialize();
        void DeInitialize();
        
        bool IsExtensionEnabled(const char* extensionName) const;

		std::vector<VkExtensionProperties> EnumerateInstanceExtensionProperties() const;
		std::vector<VkLayerProperties> EnumerateInstanceLayerProperties() const;
		std::vector<VkPhysicalDevice> EnumeratePhysicalDevices() const;
		std::vector<VkExtensionProperties> EnumerateDeviceExtensionProperties(const VkPhysicalDevice& device) const;

		// this has to be called before creating queues
		std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice& device) const;
		VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures(const VkPhysicalDevice& physicalDevice) const;
		VkPhysicalDeviceProperties GetPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice) const;

		VkDevice CreateDevice(const VkPhysicalDevice& physicalDevice, const VkDeviceCreateInfo& createInfo) const;

		VkSurfaceCapabilitiesKHR GetPhysicalDeviceSurfaceCapabilitiesKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;
		std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormatsKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;
		std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModesKHR(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) const;
		VkBool32 GetPhysicalDeviceSurfaceSupportKHR(const VkPhysicalDevice& device, uint32_t queueFamilyIndex, const VkSurfaceKHR& surface) const;


		void DestroySurface(const VkSurfaceKHR& surface) const;

		// Platform specific
		virtual VkSurfaceKHR CreateWindowSurface(void* nativeHandle) const = 0;

        
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
        VkInstance mInstance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT mCallback{ VK_NULL_HANDLE };
		bool mIsInitialized{ false };
        
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
		PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties{ nullptr };
		PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures{ nullptr };

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

    class RENDERAPI_API VulkanAPIServiceLocator
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
}
