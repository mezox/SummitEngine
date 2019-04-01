#include "VulkanRendererImpl.h"

#include <Renderer/VertexBuffer.h>
#include <Renderer/Resources/Buffer.h>
#include <Renderer/Image.h>
#include <Renderer/Resources/Synchronization.h>
#include <PAL/RenderAPI/VulkanAPI.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include <PAL/FileSystem/File.h>

#include <Logging/LoggingService.h>

#include "VulkanTypes.h"
#include "VulkanSwapChainImpl.h"
#include "VulkanDeviceObjects.h"
#include "VulkanCommandBuffer.h"

#include <Math/Matrix4.h>
#include <Math/Math.h>

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
    
    CreateRenderPass();
    
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0;
    poolInfo.flags = 0; // Optional
    
    mDevice->CreateCommandPool(&poolInfo, nullptr, &mCommandPool);
    
    mCommandBufferFactory = std::make_shared<CommandBufferFactory>(mDevice, mCommandPool, mGraphicsQueue);
    
    mImgFormat = VK_FORMAT_B8G8R8A8_UNORM;
}

void Renderer::VulkanRenderer::CreateCommandBuffers(const Pipeline& pipeline, Object3D& object)
{
    mCommandBuffers.resize(2);
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();
    
    mDevice->AllocateCommandBuffers(&allocInfo, mCommandBuffers.data());
    
    for (size_t i = 0; i < mCommandBuffers.size(); ++i)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        
        mDevice->BeginCommandBuffer(mCommandBuffers[i], &beginInfo);
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = mRenderPass;
        renderPassInfo.framebuffer = mFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = { 1280, 720 };
        
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = {1.0f, 0};
        
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
        
        const auto& indexStream = object.mVertexBuffer.GetIndexDataStream();
        
        const auto& positionBuffer = object.mVertexBuffer.GetPositionDataStream().GetDeviceResourcePtr();
        const auto& colorBuffer = object.mVertexBuffer.GetColorDataStream().GetDeviceResourcePtr();
        const auto& texCoordBuffer = object.mVertexBuffer.GetTexCoordDataStream().GetDeviceResourcePtr();
        const auto& indexBuffer = object.mVertexBuffer.GetIndexDataStream().GetDeviceResourcePtr();
        
        BufferObjectVisitor positionVisitor, colorVisitor, texCoordVisitor, indexVisitor;
        positionBuffer.Accept(positionVisitor);
        colorBuffer.Accept(colorVisitor);
        texCoordBuffer.Accept(texCoordVisitor);
        indexBuffer.Accept(indexVisitor);
        
        VkDeviceSize offset[] = { 0, 0, 0 };
        VkBuffer buffers[]{ positionVisitor.buffer, colorVisitor.buffer, texCoordVisitor.buffer };
        
        PipelineObjectVisitor visitor;
        pipeline.mDeviceObject.Accept(visitor);
        
        DescriptorSetVisitor dsVisitor;
        pipeline.effect.mDescriptorSets[i].Accept(dsVisitor);
        
        mDevice->BeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        mDevice->BindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, visitor.pipeline);
        mDevice->CmdBindVertexBuffer(mCommandBuffers[i], 0, object.mVertexBuffer.GetDataStreamCount(), buffers, offset);
        mDevice->CmdBindIndexBuffer(mCommandBuffers[i], indexVisitor.buffer, 0, (indexStream.GetStride() == sizeof(uint16_t)) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
        mDevice->CmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, visitor.layout, 0, 1, &dsVisitor.descriptorSet, 0, nullptr);
        mDevice->CmdDrawIndexed(mCommandBuffers[i], indexStream.GetElementCount(), 1, 0, 0, 0);
        mDevice->EndRenderPass(mCommandBuffers[i]);
        mDevice->EndCommandBuffer(mCommandBuffers[i]);
    }
}

void VulkanRenderer::CreateSampler(const SamplerDesc& desc, DeviceObject& sampler)
{
    CreateSamplerImpl(desc);
    
    //TODO: Create device object
    //TODO: Create image view
}

VkFramebuffer VulkanRenderer::CreateFramebufferImpl(uint32_t width, uint32_t height, const std::vector<VkImageView>& attachments, const VkRenderPass& renderPass) const
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass; // TODO: Move this to desc?
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    
    VkFramebuffer framebuffer{ VK_NULL_HANDLE };
    mDevice->CreateFramebuffer(&framebufferInfo, nullptr, &framebuffer);
    
    return framebuffer;
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

void VulkanRenderer::CreateSwapChain(std::unique_ptr<SwapChainBase>& swapChain, void* nativeHandle, uint32_t width, uint32_t height)
{
    const auto& vulkanAPI = VulkanAPI::Service();
    const auto& physicalDevice = mDevice->GetPhysicalDevice();
    
    VkSurfaceKHR vulkanSurface{ VK_NULL_HANDLE };
    if(swapChain)
    {
        swapChain->Destroy();
        
        auto vulkanSwapChain = static_cast<VulkanSwapChain*>(swapChain.get());
        
        VulkanSwapChainVisitor visitor;
        
        const auto& swapDeviceObject = vulkanSwapChain->GetDeviceObject();
        swapDeviceObject.Accept(visitor);
        
        vulkanSurface = visitor.surface;
    }
    else
    {
        vulkanSurface = vulkanAPI.CreateWindowSurface(nativeHandle);
    }
    
    const auto surfaceSupported = vulkanAPI.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 0, vulkanSurface);
    
    if(surfaceSupported)
    {
        const auto capabilities = vulkanAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, vulkanSurface);
        const auto formats = vulkanAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, vulkanSurface);
        const auto presentationModes = vulkanAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, vulkanSurface);
        
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
        createInfo.surface = vulkanSurface;
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
        createInfo.oldSwapchain = VkSwapchainKHR{};
        
        VkSwapchainKHR swapChainHandle{ VK_NULL_HANDLE };
        mDevice->CreateSwapchainKHR(&createInfo, nullptr, &swapChainHandle);
        
        VulkanSwapChainDeviceObject gpuSwapChain(swapChainHandle, vulkanSurface);
        
        const auto swapChainImages = mDevice->GetSwapchainImagesKHR(swapChainHandle);
        
        VulkanAttachmentDeviceObject depthAttachmentDO = CreateAttachment(width, height, Format::D32F, ImageUsage::DepthStencilAttachment);

        std::vector<FramebufferDeviceObject> vulkanFramebuffers;
        vulkanFramebuffers.reserve(swapChainImages.size());
        
        for(const auto& swapImage : swapChainImages)
        {
            FramebufferDeviceObject framebufferDO;
            framebufferDO.image = swapImage;
            framebufferDO.imageView = CreateImageView(swapImage, format.format, VK_IMAGE_ASPECT_COLOR_BIT);
            framebufferDO.framebuffer = CreateFramebufferImpl(width, height, { framebufferDO.imageView, depthAttachmentDO.view }, mRenderPass);
            
            vulkanFramebuffers.push_back(std::move(framebufferDO));
        }
        
        auto depthAttachment = std::make_shared<Attachment>(Format::D32F, AttachmentType::Depth, Basify(std::move(depthAttachmentDO)));
        swapChain = std::make_unique<VulkanSwapChain>(mDevice, Basify(gpuSwapChain), mGraphicsQueue);
        swapChain->SetDepthAttachment(std::move(depthAttachment));
        
        mFramebuffers.clear();
        for(auto& fb : vulkanFramebuffers)
        {
            mFramebuffers.push_back(fb.framebuffer);
            
            Framebuffer framebuffer(width, height, Basify(std::move(fb)));
            framebuffer.AddAttachment(depthAttachment);
            
            swapChain->AddFramebuffer(std::move(framebuffer));
        }
    }
    else
    {
        LOG(Error) << "Failed to create swap chain, unsupported surface";
    }
    
    // TODO: Delete surface if swap chain creation failed
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

void VulkanRenderer::CreatePipeline(Pipeline& pipeline)
{
    auto& effect = pipeline.effect;
    const auto& moduleDescriptors = effect.GetModuleDescriptors();
    
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
    
    // Setup descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.reserve(descriptorPoolSizesMap.size());
    
    for(const auto& descriptorTypeData : descriptorPoolSizesMap)
    {
        VkDescriptorPoolSize size{};
        size.descriptorCount = descriptorTypeData.second * SWAP_CHAIN_IMAGE_COUNT;
        size.type = descriptorTypeData.first;
        poolSizes.push_back(std::move(size));
    }
    
    // Pre-allocate this for whole renderer/ per render thread?
    VkDescriptorPoolCreateInfo descPoolInfo{};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descPoolInfo.pPoolSizes = poolSizes.data();
    descPoolInfo.maxSets = static_cast<uint32_t>(SWAP_CHAIN_IMAGE_COUNT);        // Depends on swap chain images cnt
    
    mDevice->CreateDescriptorPool(&descPoolInfo, nullptr, &mDescriptorPool);
    
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
    
    for (size_t i = 0; i < SWAP_CHAIN_IMAGE_COUNT; i++)          // Depends on swap chain images cnt
    {
        // TODO: Handle this generically
        const Buffer& mvpBuf = *effect.mUniformBuffers[0];
        
        BufferObjectVisitor bufferVisitor;
        mvpBuf.deviceObject.Accept(bufferVisitor);
        
        const auto& texDeviceObject = effect.mTextures[0]->GetDeviceObject();
        TextureVisitor textureVisitor;
        texDeviceObject.Accept(textureVisitor);
        
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = bufferVisitor.buffer;
        bufferInfo.offset = mvpBuf.offset;
        bufferInfo.range = mvpBuf.dataSize;
        
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureVisitor.imageView;
        imageInfo.sampler = textureVisitor.sampler;
        
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
        
        mDevice->UpdateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
    
    DescriptorSetLayoutDeviceObject dslDeviceObject(descriptorSetLayout);
    effect.mDescriptorSetLayouts.push_back(Basify(std::move(dslDeviceObject)));
    
    for (size_t i = 0; i < SWAP_CHAIN_IMAGE_COUNT; ++i)
    {
        effect.mDescriptorSets.push_back(Basify(DescriptorSetDeviceObject(descriptorSets[i])));
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
    scissor.extent = VkExtent2D{ 1280, 720 }; //TODO: Dependent on swapchain
    
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
    colorBlendAttachment.blendEnable = VK_FALSE;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    
    // Dynamic states
    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
    };
    
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 1;
    dynamicState.pDynamicStates = dynamicStates;

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
    
    VkPipelineLayout layout{ VK_NULL_HANDLE };
    mDevice->CreatePipelineLayout(&pipelineLayoutInfo, nullptr, &layout);
    
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stageInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = mRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    VkPipeline pipelineHandle{ VK_NULL_HANDLE };
    mDevice->CreateGraphicsPipeline(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipelineHandle);
    
    pipeline.mDeviceObject = PipelineDeviceObject(pipelineHandle, layout);
    
    mResourceManager.push_back(&pipeline.mDeviceObject);
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
    }
    
    bufferObject = bdo;
    
    mResourceManager.push_back(&bufferObject);
}

void VulkanRenderer::CreateFramebuffer(const FramebufferDesc& desc, DeviceObject& framebuffer)
{
    VulkanFramebufferDesc& fbDesc = *(VulkanFramebufferDesc*)(desc.data);
    
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = mRenderPass; // TODO: Move this to desc?
    framebufferInfo.attachmentCount = static_cast<uint32_t>(fbDesc.attachments.size());
    framebufferInfo.pAttachments = fbDesc.attachments.data();
    framebufferInfo.width = desc.width;
    framebufferInfo.height = desc.height;
    framebufferInfo.layers = 1;
    
    VkFramebuffer fb{ VK_NULL_HANDLE };
    mDevice->CreateFramebuffer(&framebufferInfo, nullptr, &fb);
    
    framebuffer = FramebufferDeviceObject(); //TODO: Implement this
}

void VulkanRenderer::CreateImage(const ImageDesc& desc, DeviceObject& image)
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
        
        TransitionImageLayout(imageDeviceObject.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        CopyBufferToImage(stagingBdo.buffer, imageDeviceObject.image, static_cast<uint32_t>(desc.width), static_cast<uint32_t>(desc.height));
        TransitionImageLayout(imageDeviceObject.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        mDevice->DestroyBuffer(stagingBdo.buffer, nullptr);
        mDevice->FreeMemory(stagingBdo.memory, nullptr);
    }
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
        
        TransitionImageLayout(imageDeviceObject.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        CopyBufferToImage(stagingBdo.buffer, imageDeviceObject.image, static_cast<uint32_t>(desc.width), static_cast<uint32_t>(desc.height));
        TransitionImageLayout(imageDeviceObject.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        
        mDevice->DestroyBuffer(stagingBdo.buffer, nullptr);
        mDevice->FreeMemory(stagingBdo.memory, nullptr);
        
        VkImageView imageView = CreateImageView(imageDeviceObject.image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
        VkSampler sampler = CreateSamplerImpl(samplerDesc);
        
        texture = TextureDeviceObject(imageDeviceObject.image, imageView, imageDeviceObject.memory, sampler);
    }
}

void VulkanRenderer::CreateSemaphore(const SemaphoreDescriptor& desc, DeviceObject& semaphore) const
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkSemaphore smph{ VK_NULL_HANDLE };
    mDevice->CreateSemaphore(&semaphoreInfo, nullptr, &smph);
    
    semaphore = SemaphoreDeviceObject{ smph };
}

void VulkanRenderer::CreateFence(const FenceDescriptor& desc, DeviceObject& fence) const
{
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = desc.signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
    
    VkFence f{ VK_NULL_HANDLE };
    mDevice->CreateFence(&fenceInfo, nullptr, &f);
    
    fence = FenceDeviceObject{ f };
}

void VulkanRenderer::CreateEvent(const EventDescriptor& desc, DeviceObject& event) const
{
    throw std::runtime_error("Unimplemented!");
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

void VulkanRenderer::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mImgFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    
    std::vector<VkAttachmentDescription> attachments{ colorAttachment, depthAttachment };
    
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    
    VkRenderPass renderPass{ VK_NULL_HANDLE };
    mDevice->CreateRenderPass(&renderPassInfo, nullptr, &renderPass);
    
    mDefaultRenderPass.mGpuRenderPass = VulkanRenderPassDeviceObject(renderPass);
    
    mRenderPass = renderPass;
}

VkImageView VulkanRenderer::CreateImageView(const VkImage& image, const VkFormat& format, const VkImageAspectFlags flags)
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
    vulkanImageDescriptor.usage = ConvertType(usage);
    
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
