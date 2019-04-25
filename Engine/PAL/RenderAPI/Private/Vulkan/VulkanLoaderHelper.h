#pragma once

#define VK_CHECK_RESULT(f) \
{ \
    VkResult res = (f); \
    if (res != VK_SUCCESS) \
    { \
        LOG(Warning) << "Fatal: " << PAL::RenderAPI::ToString(res); \
    } \
}

#define LOAD_VK_GLOBAL_LEVEL_FUNCTION(fun) \
if(!(fun = (PFN_##fun)vkGetInstanceProcAddr(nullptr, #fun))) \
{ \
    LOG(Warning) << "Could not load vulkan global level function: " << #fun << "!";  \
    fun = nullptr; \
}

#define LOAD_VK_INSTANCE_LEVEL_FUNCTION(instance, fun) \
if(!(fun = (PFN_##fun)vkGetInstanceProcAddr(instance, #fun ))) \
{ \
    fun = nullptr; \
    throw no_instance_func(#fun); \
}

#define LOAD_VK_INSTANCE_LEVEL_FUNCTION_EXT(instance, fun) \
if(!(fun = (PFN_##fun)vkGetInstanceProcAddr(instance, #fun ))) \
{ \
    fun = nullptr; \
    throw no_instance_func_ext(#fun); \
}

#define LOAD_VK_DEVICE_LEVEL_FUNCTION(device, loadFunc, fun)										\
if(!(fun = (PFN_##fun)loadFunc( device, #fun )))								\
{																						\
	fun = nullptr;																		\
}

#define LOAD_VK_DEVICE_LEVEL_FUNCTION_EXT(device, loadFunc, fun)										\
if(!(fun = (PFN_##fun)loadFunc( device, #fun )))								\
{																						\
	fun = nullptr;																		\
}
