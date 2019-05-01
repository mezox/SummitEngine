#include "VulkanRendererImpl.h"

#include <Renderer/VertexBuffer.h>
#include <Renderer/Resources/Buffer.h>
#include <Renderer/Image.h>
#include <Renderer/View.h>
#include <Renderer/Object3D.h>
#include <Renderer/Resources/Synchronization.h>
#include <Renderer/Resources/Texture.h>

#include <PAL/RenderAPI/Vulkan/VulkanAPI.h>
#include <PAL/RenderAPI/Vulkan/VulkanDevice.h>
#include <PAL/FileSystem/File.h>

#include <Logging/LoggingService.h>

#include "VulkanTypes.h"
#include "VulkanSwapChainImpl.h"
#include "VulkanDeviceObjects.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommands.h"

#include <Math/Matrix4.h>
#include <Math/Math.h>

#include <imgui/imgui.h>

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('V','K','R','N')

using namespace Renderer;
using namespace Renderer::Vulkan;
using namespace PAL::FileSystem;
using namespace PAL::RenderAPI;

namespace
{
    constexpr uint8_t SWAP_CHAIN_IMAGE_COUNT = 2;
}

std::unique_ptr<IRenderer> RendererLocator::mService;

std::unique_ptr<IRenderer> Renderer::CreateRenderer()
{
    return std::make_unique<VulkanRenderer>();
}

void VulkanRenderer::Initialize()
{
	const auto& vulkanAPI = VulkanAPI::Service();

	const auto instanceExt = vulkanAPI.EnumerateInstanceExtensionProperties();

	LOG(Information) << "---------------Instance extensions--------------";
	for (const auto& extension : instanceExt)
	{
		LOG(Information) << extension.extensionName;
	}

	const auto instanceLayers = vulkanAPI.EnumerateInstanceLayerProperties();

    LOG(Information) << "---------------Instance layers--------------";
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
    
    const auto deviceLayers = vulkanAPI.EnumerateDeviceLayerProperties(physicalDevices.front());
    
    LOG(Information) << "---------------Device layers:--------------";
    for (const auto& layer : deviceLayers)
    {
        LOG(Information) << layer.layerName;
    }
    
    CreateDevice(DeviceType::Integrated);
    
    mDevice->GetDeviceQueue(0, 0, &mGraphicsQueue);
    
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0;
    poolInfo.flags = 0; // Optional
    
    mDevice->CreateCommandPool(&poolInfo, nullptr, &mCommandPool);
    
    mCommandBufferFactory = std::make_shared<CommandBufferFactory>(mDevice, mCommandPool, mGraphicsQueue);
    
    // Setup descriptor pool  // Pre-allocate this for whole renderer/ per render thread?
    
    VkDescriptorPoolSize samplersPool{};
    samplersPool.descriptorCount = 20;
    samplersPool.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    
    VkDescriptorPoolSize ubosPool{};
    ubosPool.descriptorCount = 20;
    ubosPool.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    
    std::vector<VkDescriptorPoolSize> poolSizes{ samplersPool, ubosPool };
    
    VkDescriptorPoolCreateInfo descPoolInfo{};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descPoolInfo.pPoolSizes = poolSizes.data();
    descPoolInfo.maxSets = 10;        // Depends on swap chain images cnt
    
    mDevice->CreateDescriptorPool(&descPoolInfo, nullptr, &mDescriptorPool);
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    mDevice->AllocateCommandBuffers(&allocInfo, &mCmdBuff);
}

DeviceObject VulkanRenderer::CreateSurface(void* nativeViewHandle) const
{
    VkSurfaceKHR vulkanSurface = VulkanAPI::Service().CreateWindowSurface(nativeViewHandle);
    return Basify(SurfaceDeviceObject{ vulkanSurface });
}

FramebufferDeviceObject VulkanRenderer::CreateFramebufferImpl(const uint32_t width,
                                                              const uint32_t height,
                                                              const std::vector<VkImageView>& attachments,
                                                              const VkRenderPass& renderPass) const
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    
    VkFramebuffer framebuffer{ VK_NULL_HANDLE };
    mDevice->CreateFramebuffer(&framebufferInfo, nullptr, &framebuffer);

    return FramebufferDeviceObject{ framebuffer };
}

VkSampler VulkanRenderer::CreateSamplerImpl(const SamplerDesc &desc) const
{
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = ConvertType(desc.magFilter);       // Oversampling (more fragments than texels)
    samplerInfo.minFilter = ConvertType(desc.minFilter);       // Undersampling (more texels than fragments)
    samplerInfo.addressModeU = ConvertType(desc.uAddressMode);
    samplerInfo.addressModeV = ConvertType(desc.vAddressMode);
    samplerInfo.addressModeW = ConvertType(desc.wAddressMode);
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // In case of address mode clamp to border
    samplerInfo.unnormalizedCoordinates = VK_FALSE; // Enable to use address mode [0, texDimension] instead of [0, 1]
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    
    if(desc.anisotropy != 0 && mDevice->IsFeatureSupported(PAL::RenderAPI::DeviceFeature::AnisotropicFiltering))
    {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = desc.anisotropy;
    }
    else
    {
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1;
    }
    
    // Mip-mapping
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    
    VkSampler samplerHandle{ VK_NULL_HANDLE };
    mDevice->CreateSampler(&samplerInfo, nullptr, &samplerHandle);
    
    return samplerHandle;
}

void Renderer::VulkanRenderer::Deinitialize()
{
    mDevice->~VulkanDevice();
}

void VulkanRenderer::CreateDevice(DeviceType type)
{
	const auto& vulkanAPI = VulkanAPI::Service();

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

    std::vector<const char*> mEnabledDeviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    std::vector<const char*> mEnabledDeviceValidationLayers{ "VK_LAYER_LUNARG_parameter_validation" };

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

	mDevice = std::make_shared<PAL::RenderAPI::VulkanDevice>(deviceData);
}

std::unique_ptr<SwapChainBase> VulkanRenderer::CreateSwapChain(const DeviceObject& surface, const DeviceObject& renderPass, uint32_t width, uint32_t height)
{
    const auto& physicalDevice = mDevice->GetPhysicalDevice();
    
    // Get vulkan surface handle
    SurfaceVisitor surfaceVisitor;
    surface.Accept(surfaceVisitor);
    
    const auto& vulkanAPI = VulkanAPI::Service();
    if(!vulkanAPI.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 0, surfaceVisitor.surface))
    {
        LOG(Error) << "Failed to create swap chain, unsupported surface";
        return nullptr;
    }
    
    VkSwapchainKHR newVulkanSwapchain{ VK_NULL_HANDLE }, oldVulkanSwapchain{ VK_NULL_HANDLE };
    
    // Get old vulkan swapchain handle if this is recreate call
//    if(swapChain)
//    {
//        /*  Wait for device to finish its work, this isn't very effective, but
//            for swap chain recreation it's ok*/
//        mDevice->WaitIdle();
//
//        VulkanSwapChainVisitor visitor;
//
//        const auto& swapDeviceObject = swapChain->GetDeviceObject();
//        swapDeviceObject.Accept(visitor);
//
//        oldVulkanSwapchain = visitor.swapChain;
//    }
    
    const auto capabilities = vulkanAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surfaceVisitor.surface);
    const auto formats = vulkanAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surfaceVisitor.surface);
    const auto presentationModes = vulkanAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surfaceVisitor.surface);
    
    const auto& format = formats.front();      // Get B8G8R8A8_unorm
    const auto& presentMode = presentationModes.front();   // Immeadiate
    
    VkExtent2D screenExtent;
    screenExtent.width = width;
    screenExtent.height = height;
    
    auto imagesCount = capabilities.minImageCount + 1;
    
    if (1/*mEnableTrippleBuffering*/)
    {
        if (capabilities.maxImageCount > 0 && imagesCount > capabilities.maxImageCount)
        {
            imagesCount = capabilities.maxImageCount;
        }
    }
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surfaceVisitor.surface;
    createInfo.minImageCount = imagesCount;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageFormat = format.format;
    createInfo.imageExtent = screenExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //Use transfer if I want to render to other buffer and apply post process and then copy;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldVulkanSwapchain;
    
    mDevice->CreateSwapchainKHR(&createInfo, nullptr, &newVulkanSwapchain);
    
    // Create sync primitives
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    auto imgSemaphore = mDevice->CreateManagedSemaphore(&semaphoreInfo, nullptr);
    auto renderSemaphore = mDevice->CreateManagedSemaphore(&semaphoreInfo, nullptr);
    
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    auto frameFence = mDevice->CreateManagedFence(&fenceInfo, nullptr);
    
    SwapChainDeviceObject gpuSwapChain{ newVulkanSwapchain,
        std::move(imgSemaphore),
        std::move(renderSemaphore),
        std::move(frameFence)
    };
    
    auto swapChain = std::make_unique<VulkanSwapChain>(mDevice, Basify(gpuSwapChain));
    
    VulkanAttachmentDeviceObject depthAttachmentDO = CreateAttachment(width, height, Format::D32F, ImageUsage::DepthStencilAttachment);
    
    AttachableDescriptor depthAttachmentDesc;
    depthAttachmentDesc.width = width;
    depthAttachmentDesc.height = height;
    depthAttachmentDesc.format = Format::D32F;
    depthAttachmentDesc.usage = ImageUsage::DepthStencilAttachment;
    
    auto depthAttachment = std::make_shared<Attachment>(depthAttachmentDesc, Graphics::Color(255, 0, 0, 0));
    depthAttachment->SetDeviceObject(Basify(std::move(depthAttachmentDO)));
    swapChain->SetDepthAttachment(depthAttachment);
    
    // Create framebuffers & attach them to swap chain
    const auto swapChainImages = mDevice->GetSwapchainImagesKHR(newVulkanSwapchain);
    
    RenderPassVisitor rpv;
    renderPass.Accept(rpv);
    
    for(const auto& swapImage : swapChainImages)
    {
        auto swapImageView = CreateImageView(swapImage, format.format, VK_IMAGE_ASPECT_COLOR_BIT);
        VulkanAttachmentDeviceObject attachmentDO{ swapImage, VK_NULL_HANDLE, swapImageView };
        
        AttachableDescriptor attachmentDesc;
        attachmentDesc.width = width;
        attachmentDesc.height = height;
        attachmentDesc.format = Format::B8G8R8A8;
        attachmentDesc.usage = ImageUsage::ColorAttachment;
        
        Framebuffer framebuffer;
        framebuffer.Resize(width, height);
        framebuffer.AddAttachment(depthAttachment);
        framebuffer.AddAttachment(std::make_shared<Attachment>(attachmentDesc, Graphics::Color(20, 128, 224, 255), Basify(attachmentDO)));
        framebuffer.SetDeviceObject(Basify(CreateFramebufferImpl(width, height, { depthAttachmentDO.view, swapImageView,  }, rpv.renderPass)));
        
        swapChain->AddFramebuffer(std::move(framebuffer));
    }
    
    return swapChain;
}

void VulkanRenderer::CreateShader(DeviceObject& shader, const std::vector<uint8_t>& code) const
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule module{ VK_NULL_HANDLE };
    mDevice->CreateShaderModule(&createInfo, nullptr, &module);
    
    shader = VulkanShaderDeviceObject(module);
}

void VulkanRenderer::CreatePipeline(Pipeline& pipeline, const DeviceObject& renderPass)
{
    auto& effect = pipeline.effect;
    
    // Prepare modules
    const auto stageInfos = PrepareModules(effect);
    
    // Setup attributes
    const auto bindingCount = effect.GetBindingCount();
    
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    bindingDescriptions.reserve(bindingCount);
    
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    
    for(uint8_t bindingId{ 0 }; bindingId < bindingCount; ++bindingId)
    {
        VkVertexInputBindingDescription desc{};
        desc.binding = bindingId;
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        const auto& attributes = effect.GetBindingDescriptor(bindingId);
        const auto attributeCount = attributes.size();
        
        for(uint8_t location{ 0 }; location < attributeCount; ++location)
        {
            const auto attributeFormat = attributes[location];
            
            VkVertexInputAttributeDescription attribDesc;
            attribDesc.binding = bindingId;
            attribDesc.location = (attributeCount == 1) ? bindingId : location;
            attribDesc.format = ConvertType(attributeFormat);
            attribDesc.offset = desc.stride;
            
            attributeDescriptions.push_back(std::move(attribDesc));
            
            desc.stride += GetSizeFromFormat(attributeFormat);
        }
        
        bindingDescriptions.push_back(std::move(desc));
    }
    
    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    // Setup descriptor set layout
    const auto& bindings = effect.GetUniformBindings();
    
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    
    std::unordered_map<VkDescriptorType, uint32_t> descriptorPoolSizesMap;
    for(uint32_t bindingId{ 0 }; bindingId < bindings.size(); ++bindingId)
    {
        const auto& binding = bindings[bindingId];
        if(binding.empty())
            continue;
        
        for(const auto& uniform : binding)
        {
            const VkDescriptorType type = ConvertType(uniform.type);
            
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = bindingId;
            layoutBinding.descriptorType = type;
            layoutBinding.descriptorCount = uniform.count;
            layoutBinding.stageFlags = ConvertType(uniform.stage);
            layoutBinding.pImmutableSamplers = nullptr;
            
            layoutBindings.push_back(std::move(layoutBinding));
            
            descriptorPoolSizesMap[type]++;
        }
    }
    
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();
    
    VkDescriptorSetLayout descriptorSetLayout{ VK_NULL_HANDLE };
    mDevice->CreateDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout);
    
    // Create descriptor sets
    std::vector<VkDescriptorSetLayout> layouts(SWAP_CHAIN_IMAGE_COUNT, descriptorSetLayout); // Depends on swap chain images cnt
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(SWAP_CHAIN_IMAGE_COUNT);        // Depends on swap chain images cnt
    allocInfo.pSetLayouts = layouts.data();
    
    std::vector<VkDescriptorSet> descriptorSets;
    descriptorSets.resize(SWAP_CHAIN_IMAGE_COUNT);           // Depends on swap chain images cnt
    mDevice->AllocateDescriptorSets(&allocInfo, descriptorSets.data());
    
    DescriptorSetLayoutDeviceObject dslDeviceObject(descriptorSetLayout);
    effect.mDescriptorSetLayouts.push_back(Basify(std::move(dslDeviceObject)));
    
    for (size_t i = 0; i < SWAP_CHAIN_IMAGE_COUNT; ++i)
    {
        effect.mDescriptorSets.push_back(Basify(DescriptorSetDeviceObject(descriptorSets[i])));
    }
    
    // Viewport & scissor test
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = 1280.0f;   //TODO: Dependent on swapchain
    viewport.height = 720.0f;   //TODO: Dependent on swapchain
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = VkExtent2D{ 1280, 720 };
    
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    
    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    
    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//    colorBlending.logicOpEnable = VK_FALSE;
//    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
//    colorBlending.blendConstants[0] = 0.0f;
//    colorBlending.blendConstants[1] = 0.0f;
//    colorBlending.blendConstants[2] = 0.0f;
//    colorBlending.blendConstants[3] = 0.0f;
    
    // Dynamic states
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;
    
    // -------- Handle push constants ----------------------
    
    std::vector<VkPushConstantRange> pushConstRanges;
    pushConstRanges.reserve(effect.mConstantRanges.size());
    
    for(const auto& range : effect.mConstantRanges)
    {
        VkPushConstantRange vkRange;
        vkRange.stageFlags = ConvertType(range.stage);
        vkRange.offset = range.offset;
        vkRange.size = range.size;
        
        pushConstRanges.push_back(std::move(vkRange));
    }
    
    // -------- End of push constants handler --------------

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstRanges.size());
    pipelineLayoutInfo.pPushConstantRanges = pushConstRanges.data();
    
    VkPipelineLayout layout{ VK_NULL_HANDLE };
    mDevice->CreatePipelineLayout(&pipelineLayoutInfo, nullptr, &layout);
    
    RenderPassVisitor rpv;
    renderPass.Accept(rpv);
    
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(stageInfos.size());
    pipelineInfo.pStages = stageInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = rpv.renderPass;
    pipelineInfo.subpass = pipeline.mSubpassIndex;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.pDynamicState = &dynamicState;
    
    if(pipeline.depthTestEnabled || pipeline.depthWriteEnabled)
    {
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = pipeline.depthTestEnabled;
        depthStencil.depthWriteEnable = pipeline.depthWriteEnabled;
        depthStencil.depthCompareOp = pipeline.depthWriteEnabled ? VK_COMPARE_OP_LESS : VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};
        
        pipelineInfo.pDepthStencilState = &depthStencil;
    }
    
    VkPipeline pipelineHandle{ VK_NULL_HANDLE };
    mDevice->CreateGraphicsPipeline(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineHandle);
    
    pipeline.mDeviceObject = PipelineDeviceObject(pipelineHandle, layout);
    
    mResourceManager.push_back(&pipeline.mDeviceObject);
    
    for (size_t i = 0; i < SWAP_CHAIN_IMAGE_COUNT; ++i)          // Depends on swap chain images cnt
    {
        std::vector<VkWriteDescriptorSet> descriptorWrites(effect.mUniformBuffers.size() + effect.mTextures.size());
        
        uint32_t descriptorWriteIdx{ 0 };
        
        // TODO: Descriptor writes for ubos, they should bind to unique ubos per frame, do not share them
        for(const auto ubo : effect.mUniformBuffers)
        {
            const Buffer& uboBuffer = *ubo;
            
            BufferObjectVisitor bufferVisitor;
            uboBuffer.deviceObject.Accept(bufferVisitor);
            
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = bufferVisitor.buffer;
            bufferInfo.offset = uboBuffer.offset;
            bufferInfo.range = uboBuffer.dataSize;
            
            descriptorWrites[descriptorWriteIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[descriptorWriteIdx].dstSet = descriptorSets[i];
            descriptorWrites[descriptorWriteIdx].dstBinding = 0;
            descriptorWrites[descriptorWriteIdx].dstArrayElement = 0;
            descriptorWrites[descriptorWriteIdx].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[descriptorWriteIdx].descriptorCount = 1;
            descriptorWrites[descriptorWriteIdx].pBufferInfo = &bufferInfo;
            
            ++descriptorWriteIdx;
        }
        
        // TODO: Descriptor writes for textures, they should bind to unique ubos per frame, do not share them
        for(const auto texture : effect.mTextures)
        {
            const Attachable& image = *texture;
            
            const auto& texDeviceObject = image.GetDeviceObject();
            
            AttachableVisitor attachable;
            texDeviceObject.Accept(attachable);
            
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = attachable.imageView;
            imageInfo.sampler = [this, &attachable](){
                if(attachable.sampler == VK_NULL_HANDLE)
                {
                    SamplerDesc samplerDesc;
                    samplerDesc.anisotropy = 0;
                    samplerDesc.minFilter = FilterMode::Linear;
                    samplerDesc.magFilter = FilterMode::Linear;
                    samplerDesc.uAddressMode = AddressMode::Repeat;
                    samplerDesc.vAddressMode = AddressMode::Repeat;
                    samplerDesc.wAddressMode = AddressMode::Repeat;
                    
                    return CreateSamplerImpl(samplerDesc);
                }
                
                return attachable.sampler;
            }();
            
            descriptorWrites[descriptorWriteIdx].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[descriptorWriteIdx].dstSet = descriptorSets[i];
            descriptorWrites[descriptorWriteIdx].dstBinding = 1;
            descriptorWrites[descriptorWriteIdx].dstArrayElement = 0;
            descriptorWrites[descriptorWriteIdx].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[descriptorWriteIdx].descriptorCount = 1;
            descriptorWrites[descriptorWriteIdx].pImageInfo = &imageInfo;
            
            ++descriptorWriteIdx;
        }
        
        mDevice->UpdateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

uint32_t VulkanRenderer::FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties{ VK_NULL_HANDLE };
    
    const auto& renderAPI = VulkanAPI::Service();
    renderAPI.GetPhysicalDeviceMemoryProperties(mDevice->GetPhysicalDevice(), &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    
    return 0;
}

BufferDeviceObject VulkanRenderer::CreateBufferImpl(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode) const
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.flags = 0;
    
    VkBuffer buffer{ VK_NULL_HANDLE };
    mDevice->CreateBuffer(&bufferInfo, nullptr, &buffer);
    
    VkMemoryRequirements memRequirements{};
    mDevice->GetBufferMemoryRequirements(buffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties);
    
    VkDeviceMemory memory{ VK_NULL_HANDLE };
    mDevice->AllocateMemory(&allocInfo, nullptr, &memory);
    mDevice->BindBufferMemory(buffer, memory, 0);
    
    return BufferDeviceObject(buffer, memory);
}

ImageDeviceObject VulkanRenderer::CreateImageImpl(const VulkanImageDesc& descriptor) const
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = descriptor.type;
    imageInfo.extent.width = descriptor.width;
    imageInfo.extent.height = descriptor.height;
    imageInfo.extent.depth = descriptor.depth;
    imageInfo.mipLevels = descriptor.mipMapLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = descriptor.format;
    imageInfo.tiling = descriptor.tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = descriptor.usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = descriptor.sharingMode;
    imageInfo.flags = 0;
    
    VkImage image{ VK_NULL_HANDLE };
    mDevice->CreateImage(&imageInfo, nullptr, &image);
    
    VkMemoryRequirements memRequirements;
    mDevice->GetImageMemoryRequirements(image, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryTypeIndex(memRequirements.memoryTypeBits, descriptor.memoryProps);
    
    VkDeviceMemory imageMemory{ VK_NULL_HANDLE };
    mDevice->AllocateMemory(&allocInfo, nullptr, &imageMemory);
    mDevice->BindImageMemory(image, imageMemory, 0);
    
    return ImageDeviceObject(image, imageMemory);
}

void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
    mCommandBufferFactory->CreateScopeCommandBuffer().CopyBuffer(srcBuffer, dstBuffer, size);
}

void VulkanRenderer::CopyBufferToImage(VkBuffer buffer, VkImage image, const uint32_t width, const uint32_t height) const
{
    mCommandBufferFactory->CreateScopeCommandBuffer().CopyBufferToImage(buffer, image, width, height);
}

void VulkanRenderer::CreateBuffer(const BufferDesc& desc, DeviceObject& bufferObject)
{
    BufferDeviceObject bdo;
    
    const auto vulkanMemoryType = ConvertType(desc.memoryUsage);
    const auto vulkanBufferUsage = ConvertType(desc.usage);
    
    if(desc.memoryUsage & MemoryType::DeviceLocal)
    {
        constexpr auto stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        constexpr auto stagingMemoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        const auto stagingBuffer = CreateBufferImpl(desc.bufferSize, stagingBufferUsage, stagingMemoryType, VK_SHARING_MODE_EXCLUSIVE);
        
        void* data{ nullptr };
        mDevice->MapMemory(stagingBuffer.memory, 0, desc.bufferSize, 0, &data);
        memcpy(data, desc.data, (size_t)desc.bufferSize);
        mDevice->UnmapMemory(stagingBuffer.memory);
        
        bdo = CreateBufferImpl(desc.bufferSize, vulkanBufferUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, vulkanMemoryType, VK_SHARING_MODE_EXCLUSIVE);
        
        CopyBuffer(stagingBuffer.buffer, bdo.buffer, desc.bufferSize);
        
        mDevice->DestroyBuffer(stagingBuffer.buffer, nullptr);
        mDevice->FreeMemory(stagingBuffer.memory, nullptr);
    }
    else if(desc.memoryUsage & MemoryType::HostVisible)
    {
        bdo = CreateBufferImpl(desc.bufferSize, vulkanBufferUsage, vulkanMemoryType, VK_SHARING_MODE_EXCLUSIVE);
        if(desc.data)
        {
            void* data{ nullptr };
            mDevice->MapMemory(bdo.memory, 0, desc.bufferSize, 0, &data);
            memcpy(data, desc.data, (size_t)desc.bufferSize);
            
            VkMappedMemoryRange mappedRange{};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory = bdo.memory;
            mappedRange.offset = 0;
            mappedRange.size = VK_WHOLE_SIZE;
            
            mDevice->FlushMappedMemoryRanges(1, &mappedRange);
            
            bdo.mappedMemory = data;
        }
    }
    
    bufferObject = bdo;
    
    mResourceManager.push_back(&bufferObject);
}

void VulkanRenderer::CreateFramebuffer(Framebuffer& framebuffer, const RenderPass& renderPass)
{
    std::vector<VkImageView> imageViewAttachments;
    imageViewAttachments.reserve(framebuffer.mAttachments.size());
    
    for(auto attachment : framebuffer.mAttachments)
    {
        const bool isDepthAttachment(attachment->GetUsage() & ImageUsage::DepthStencilAttachment);
        
        VulkanImageDesc vulkanImageDescriptor;
        vulkanImageDescriptor.width = attachment->GetWidth();
        vulkanImageDescriptor.height = attachment->GetHeight();
        vulkanImageDescriptor.depth = 1;
        vulkanImageDescriptor.mipMapLevels = 1;
        vulkanImageDescriptor.data = nullptr;
        vulkanImageDescriptor.format = ConvertType(attachment->GetFormat());
        vulkanImageDescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vulkanImageDescriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        vulkanImageDescriptor.memoryProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; //ConvertType(attachment->GetUsage());
        vulkanImageDescriptor.usage = isDepthAttachment ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        //vulkanImageDescriptor.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        
        const ImageDeviceObject imageDeviceObject = CreateImageImpl(vulkanImageDescriptor);
        
        TransitionImageLayout(imageDeviceObject.image,
                              vulkanImageDescriptor.format,
                              VK_IMAGE_LAYOUT_UNDEFINED,
                              isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        VkImageView imageView = CreateImageView(imageDeviceObject.image,
                                                vulkanImageDescriptor.format,
                                                isDepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
        
        attachment->SetDeviceObject(Basify(VulkanAttachmentDeviceObject{ imageDeviceObject.image, imageDeviceObject.memory, imageView }));
        
        imageViewAttachments.push_back(imageView);
    }
    
    const auto& renderPassDO = renderPass.GetDeviceObject();

    RenderPassVisitor rpVisitor;
    renderPassDO.Accept(rpVisitor);
    
    auto framebufferDO = CreateFramebufferImpl(framebuffer.GetWidth(), framebuffer.GetHeight(), imageViewAttachments, rpVisitor.renderPass);
    framebuffer.SetDeviceObject(Basify(std::move(framebufferDO)));
}

DeviceObject VulkanRenderer::CreateImage(const ImageDesc& desc)
{
    
}

void VulkanRenderer::CreateTexture(const ImageDesc& desc, const SamplerDesc& samplerDesc, DeviceObject& texture)
{    
    if(desc.memoryUsage & MemoryType::DeviceLocal)
    {
        constexpr auto stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        constexpr auto stagingMemoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        const auto imageSize = desc.width * desc.height * GetSizeFromFormat(desc.format);
        const auto stagingBdo = CreateBufferImpl(imageSize, stagingBufferUsage, stagingMemoryType, VK_SHARING_MODE_EXCLUSIVE);
        
        void* data{ nullptr };
        mDevice->MapMemory(stagingBdo.memory, 0, imageSize, 0, &data);
        memcpy(data, desc.data, (size_t)imageSize);
        mDevice->UnmapMemory(stagingBdo.memory);
        
        VulkanImageDesc vulkanImageDescriptor;
        vulkanImageDescriptor.width = desc.width;
        vulkanImageDescriptor.height = desc.height;
        vulkanImageDescriptor.depth = desc.depth;
        vulkanImageDescriptor.mipMapLevels = desc.mipMapLevels;
        vulkanImageDescriptor.data = desc.data;
        vulkanImageDescriptor.format = ConvertType(desc.format);
        vulkanImageDescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vulkanImageDescriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        vulkanImageDescriptor.memoryProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        vulkanImageDescriptor.usage = ConvertType(desc.usage) | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        
        const auto imageDeviceObject = CreateImageImpl(vulkanImageDescriptor);
        
        TransitionImageLayout(imageDeviceObject.image, vulkanImageDescriptor.format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        CopyBufferToImage(stagingBdo.buffer, imageDeviceObject.image, desc.width, desc.height);
        TransitionImageLayout(imageDeviceObject.image, vulkanImageDescriptor.format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        mDevice->DestroyBuffer(stagingBdo.buffer, nullptr);
        mDevice->FreeMemory(stagingBdo.memory, nullptr);
        
        VkImageView imageView = CreateImageView(imageDeviceObject.image, vulkanImageDescriptor.format, VK_IMAGE_ASPECT_COLOR_BIT);
        VkSampler sampler = CreateSamplerImpl(samplerDesc);
        
        texture = TextureDeviceObject(imageDeviceObject.image, imageView, imageDeviceObject.memory, sampler);
    }
    else
    {
        //    // Create target image for copy
        //    VkImageCreateInfo imageInfo = vks::initializers::imageCreateInfo();
        //    imageInfo.extent.depth = 1;
        //    imageInfo.arrayLayers = 1;
        //    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        //    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        //    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        //    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //
        //    VK_CHECK_RESULT(vkCreateImage(device->logicalDevice, &imageInfo, nullptr, &fontImage));
        //
    }
}

DeviceObject VulkanRenderer::CreateSemaphore(const SemaphoreDescriptor& desc) const
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkSemaphore smph{ VK_NULL_HANDLE };
    mDevice->CreateSemaphore(&semaphoreInfo, nullptr, &smph);
    
    return Basify(SemaphoreDeviceObject{ smph });
}

DeviceObject VulkanRenderer::CreateFence(const FenceDescriptor& desc) const
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = desc.signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
    
    VkFence f{ VK_NULL_HANDLE };
    mDevice->CreateFence(&fenceInfo, nullptr, &f);
    
    return Basify(FenceDeviceObject{ f });
}

DeviceObject VulkanRenderer::CreateEvent(const EventDescriptor& desc) const
{
    throw std::runtime_error("Unimplemented!");
    return DeviceObject{};
}

void VulkanRenderer::MapMemory(const DeviceObject& deviceObject, uint32_t size, void* data)
{
    MemoryMapVisitor visitor;
    deviceObject.Accept(visitor);
    
    void* pdata{ nullptr };
    mDevice->MapMemory(visitor.memory, 0, size, 0, &pdata);
    memcpy(pdata, data, size);
    mDevice->UnmapMemory(visitor.memory);
}

void VulkanRenderer::UnmapMemory(const DeviceObject& deviceObject) const
{
    MemoryMapVisitor visitor;
    deviceObject.Accept(visitor);
    
    mDevice->UnmapMemory(visitor.memory);
}

void VulkanRenderer::Render(const Object3d& object, const Pipeline& pipeline)
{
    const auto& vb = object.GetVertexBuffer();
    
    if(!vb.mStreams[0].get())
        return;

    mCmdList.push_back(BindPipeline(pipeline.mDeviceObject));
    mCmdList.push_back(BindVertexBuffer(vb));
    mCmdList.push_back(BindIndexBuffer(vb));
    mCmdList.push_back(BindDescriptorSets(pipeline.mDeviceObject, pipeline.effect.mDescriptorSets[0]));
    mCmdList.push_back(DrawIndexed(vb.mStreams[1]->GetCount(), 0, 0));
}

void VulkanRenderer::DestroyDeviceObject(DeviceObject& buffer) const
{
    DestroyVisitor destroyVisitor(mDevice);
    buffer.Accept(destroyVisitor);
}

void VulkanRenderer::RenderGui(const VertexBufferBase& vb, const Pipeline& pipeline)
{
    const ImDrawData* imDrawData = ImGui::GetDrawData();
    
    if (imDrawData->CmdListsCount == 0)
        return;
    
    const ImVec2& imViewSize = ImGui::GetIO().DisplaySize;
    
    struct PushConstantsBlock
    {
        Vector2f uiScale;
        Vector2f translate;
    };
    
    PushConstantsBlock pcb;
    pcb.uiScale = Vector2f(2.0f / imViewSize.x, 2.0f / imViewSize.y);
    pcb.translate = Vector2f(-1.0f, -1.0f);
    
    mCmdList.push_back(BindDescriptorSets(pipeline.mDeviceObject, pipeline.effect.mDescriptorSets[0]));
    mCmdList.push_back(BindPipeline(pipeline.mDeviceObject));
    mCmdList.push_back(SetViewportCommand(Rectangle<float>(imViewSize.x, imViewSize.y)));
    mCmdList.push_back(PushConstants(pipeline.mDeviceObject, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsBlock), &pcb));
    mCmdList.push_back(BindVertexBuffer(vb));
    mCmdList.push_back(BindIndexBuffer(vb));
    
    int32_t vertexOffset{ 0 }, indexOffset{ 0 };
    for (int32_t i = 0; i < imDrawData->CmdListsCount; ++i)
    {
        const ImDrawList* cmd_list = imDrawData->CmdLists[i];
        for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
            
            Rectangle<uint32_t> scissorRect;
            scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
            scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
            scissorRect.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
            scissorRect.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
            
            mCmdList.push_back(SetScissorCommand(scissorRect));
            mCmdList.push_back(DrawIndexed(pcmd->ElemCount, indexOffset, vertexOffset));
            
            indexOffset += pcmd->ElemCount;
        }
        vertexOffset += cmd_list->VtxBuffer.Size;
    }
}

void VulkanRenderer::CreateRenderPass(RenderPass& renderPass) const
{
    _ASSERT(!renderPass.mSubPasses.empty() && "No subpasses defined for render pass");
    _ASSERT(!renderPass.mAttachments.empty() && "No attachments defined for render pass");
    
    std::vector<VkAttachmentDescription> attachments;
    attachments.reserve(renderPass.mAttachments.size());

    // Create attachment descriptions
    for(const auto& attachmentDesc : renderPass.mAttachments)
    {
        VkAttachmentDescription attachment{};
        attachment.flags = 0;
        attachment.format = ConvertType(attachmentDesc.format);
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = ConvertType(attachmentDesc.initialLayout);
        attachment.finalLayout = ConvertType(attachmentDesc.finalLayout);
        attachments.push_back(std::move(attachment));
    }
    
    std::vector<std::unique_ptr<std::vector<VkAttachmentReference>>> attachmentRefs;
    
    const auto CreateAttachmentReference = [&attachmentRefs](const Subpass& pass, const AttachmentType type) -> const std::vector<VkAttachmentReference>* {
        
        const auto subpassRefs = pass.GetAttachments(type);
        if(subpassRefs.empty())
        {
            attachmentRefs.push_back(nullptr);
            return attachmentRefs.back().get();
        }
        
        auto internalReferences = std::make_unique<std::vector<VkAttachmentReference>>();
        internalReferences->reserve(subpassRefs.size());
        
        for(const auto& subpassRef : subpassRefs)
        {
            VkAttachmentReference ref{};
            ref.attachment = subpassRef->attachmentId;
            ref.layout = ConvertType(subpassRef->attachmentLayout);
            internalReferences->push_back(std::move(ref));
        }
        
        attachmentRefs.push_back(std::move(internalReferences));
        return attachmentRefs.back().get();
    };
    
    std::vector<VkSubpassDescription> subpassDescs;
    subpassDescs.reserve(renderPass.mSubPasses.size());
    
    // Create subpass descriptions
    for(const auto& subPass : renderPass.mSubPasses)
    {
        const auto* inputRefs = CreateAttachmentReference(subPass, AttachmentType::Input);
        const auto* colorRefs = CreateAttachmentReference(subPass, AttachmentType::Color);
        const auto* depthRefs = CreateAttachmentReference(subPass, AttachmentType::DepthStencil);
        const auto* resolveRefs = CreateAttachmentReference(subPass, AttachmentType::Resolve);
        
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.flags = 0;
        subpass.inputAttachmentCount = !inputRefs ? 0 : static_cast<uint32_t>(inputRefs->size());;
        subpass.pInputAttachments = !inputRefs ? nullptr : inputRefs->data();
        subpass.colorAttachmentCount = !colorRefs ? 0 : static_cast<uint32_t>(colorRefs->size());
        subpass.pColorAttachments = !colorRefs ? nullptr : colorRefs->data();
        subpass.preserveAttachmentCount = 0;
        subpass.pResolveAttachments = nullptr;
        subpass.pPreserveAttachments = nullptr;
        subpass.pDepthStencilAttachment = [depthRefs](){
            return !depthRefs ? nullptr : depthRefs->data();
        }();
        
        subpassDescs.push_back(std::move(subpass));
    }
    
    std::vector<VkSubpassDependency> dependencies;
    dependencies.reserve(renderPass.mDependencies.size());
    
    for(const auto& subpassDepdendency : renderPass.mDependencies)
    {
        VkSubpassDependency dependency{};
        dependency.srcSubpass = subpassDepdendency.srcIdx;
        dependency.dstSubpass = subpassDepdendency.dstIdx;
        dependency.srcStageMask = ConvertType(subpassDepdendency.srcStageMask);
        dependency.dstStageMask = ConvertType(subpassDepdendency.dstStageMask);
        dependency.srcAccessMask = ConvertType(subpassDepdendency.srcAccessMask);
        dependency.dstAccessMask = ConvertType(subpassDepdendency.dstAccessMask);
        dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencies.push_back(std::move(dependency));
    }
    
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.flags = 0;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescs.size());
    renderPassInfo.pSubpasses = subpassDescs.data();
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    
    renderPass.SetDeviceObject(Basify(RenderPassDeviceObject{ mDevice->CreateManagedRenderPass(&renderPassInfo, nullptr) }));
}

VkImageView VulkanRenderer::CreateImageView(const VkImage& image, const VkFormat& format, const VkImageAspectFlags flags) const
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = flags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    
    VkImageView imageView{ VK_NULL_HANDLE };
    mDevice->CreateImageView(&imageViewCreateInfo, nullptr, &imageView);
    
    return imageView;
}

VulkanAttachmentDeviceObject VulkanRenderer::CreateAttachment(const uint32_t width, const uint32_t height, const Format format, const ImageUsage usage)
{    
    const VkFormat vulkanImageFormat = ConvertType(format);
    
    VulkanImageDesc vulkanImageDescriptor;
    vulkanImageDescriptor.width = width;
    vulkanImageDescriptor.height = height;
    vulkanImageDescriptor.depth = 1;
    vulkanImageDescriptor.format = vulkanImageFormat;
    vulkanImageDescriptor.mipMapLevels = 1;
    vulkanImageDescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vulkanImageDescriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
    vulkanImageDescriptor.memoryProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vulkanImageDescriptor.usage = ConvertType(usage) | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    
    auto imageObject = CreateImageImpl(vulkanImageDescriptor);
    auto imageView = CreateImageView(imageObject.image, vulkanImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    TransitionImageLayout(imageObject.image, vulkanImageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    
    return VulkanAttachmentDeviceObject(imageObject.image, imageObject.memory, imageView);
}

void VulkanRenderer::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const
{
    auto cmdBuffer = mCommandBufferFactory->CreateScopeCommandBuffer();
    
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Used to transfer ownership between queues
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // Used to transfer ownership between queues
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }
    
    cmdBuffer.PipelineBarrier(sourceStage, destinationStage, 0, {}, {}, { barrier });
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanRenderer::PrepareModules(Effect& effect) const
{
    const auto& moduleDescriptors = effect.GetModuleDescriptors();
    
    std::vector<VkPipelineShaderStageCreateInfo> stageInfos;
    stageInfos.reserve(moduleDescriptors.size());
    
    // Setup stages
    for(const auto& moduleDescriptor : moduleDescriptors)
    {
        File shaderFile(moduleDescriptor.filePath);
        shaderFile.Open(EFileAccessMode::Read);
        auto shaderSource = shaderFile.Read();
        
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderSource.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSource.data());
        
        VkShaderModule module{ VK_NULL_HANDLE };
        mDevice->CreateShaderModule(&createInfo, nullptr, &module);
        
        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = ConvertType(moduleDescriptor.type);
        shaderStageInfo.module = module;
        shaderStageInfo.pName = "main";
        
        stageInfos.push_back(std::move(shaderStageInfo));
        effect.mModules.push_back(Basify(VulkanShaderDeviceObject(module)));
    }
    
    return stageInfos;
}

CmdRecordResult VulkanRenderer::BeginCommandRecording()
{
    mDevice->FreeCommandBuffers(mCommandPool, 1, &mCmdBuff);
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    mDevice->AllocateCommandBuffers(&allocInfo, &mCmdBuff);
    
    mCmdList.push_back(BeginCommand(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT));
    
    return CmdRecordResult::Success;
}

CmdRecordResult VulkanRenderer::BeginRenderPass(const RenderPass& renderPass)
{
    const auto* activeFramebufferPtr = renderPass.GetActiveFramebuffer();
    if(!activeFramebufferPtr)
        return CmdRecordResult::RPFramebufferUnavailable;
    
    mCmdList.push_back(Vulkan::BeginRenderPass(renderPass));
    
    return CmdRecordResult::Success;
}

CmdRecordResult VulkanRenderer::NextSubpass()
{
    mCmdList.push_back(NextRenderPassCommand());
}

CmdRecordResult VulkanRenderer::SetViewport(const Rectangle<float>& viewport)
{
    mCmdList.push_back(SetViewportCommand(viewport));
    return CmdRecordResult::Success;
}

CmdRecordResult VulkanRenderer::SetScissor(const Rectangle<uint32_t>& scissor)
{
    mCmdList.push_back(SetScissorCommand(scissor));
    return CmdRecordResult::Success;
}

CmdRecordResult VulkanRenderer::EndRenderPass()
{
    mCmdList.push_back(Vulkan::EndRenderPass());
    return CmdRecordResult::Success;
}

CmdRecordResult VulkanRenderer::EndCommandRecording(SwapChainBase* swapChain)
{
    mCmdList.push_back(EndCommand());
    
    for(const auto& cmd : mCmdList)
    {
        cmd.Execute(*mDevice, Basify(CommandBufferDeviceObject{ mCmdBuff }));
    }
    
    mCmdList.clear();
    
    auto* vulkanSwapChain = (VulkanSwapChain*)swapChain;
    
    const auto& swapChainDeviceObject = vulkanSwapChain->GetDeviceObject();
    
    VulkanSwapChainVisitor swapChainVisitor;
    swapChainDeviceObject.Accept(swapChainVisitor);
    
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &swapChainVisitor.imgAvailableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCmdBuff;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &swapChainVisitor.renderFinishedSemaphore;
    
    mDevice->QueueSubmit(mGraphicsQueue, 1, &submitInfo, swapChainVisitor.frameFence);
    
    return CmdRecordResult::Success;
}
