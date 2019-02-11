#include "VulkanRendererImpl.h"

#include <Renderer/VertexBuffer.h>
#include <PAL/RenderAPI/VulkanAPI.h>
#include <PAL/RenderAPI/VulkanDevice.h>
#include <PAL/FileSystem/File.h>

#include <Logging/LoggingService.h>

#include "VulkanSwapChainImpl.h"
#include "VulkanDeviceObjects.h"

#include <Math/Matrix4.h>
#include <Math/Math.h>

struct Transform
{
    Matrix4 model;
    Matrix4 view;
    Matrix4 projection;
};

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE('V','K','R','N')

using namespace Renderer;
using namespace PAL::FileSystem;

std::unique_ptr<IRenderer> RendererLocator::mService;

std::unique_ptr<IRenderer> Renderer::CreateRenderer()
{
    return std::make_unique<VulkanRenderer>();
}

namespace
{
    VkBufferUsageFlags ToVulkanBufferUsage(BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::VertexBuffer: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            case BufferUsage::IndexBuffer: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        
        return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    }
    
    VkSharingMode ToVulkanSharingMode(SharingMode mode)
    {
        switch (mode)
        {
            case SharingMode::Exclusive: return VK_SHARING_MODE_EXCLUSIVE;
        }
        
        return VK_SHARING_MODE_MAX_ENUM;
    }
    
    VkMemoryPropertyFlags ToVulkanMemoryProperty(MemoryType type)
    {
        VkMemoryPropertyFlags flags{ 0 };

        if(MemoryType::DeviceLocal & type)
            flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        if(MemoryType::HostVisible & type)
            flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        if(MemoryType::HostCoherent & type)
            flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        return flags;
    }
}

void VulkanRenderer::Initialize()
{
	const auto& vulkanAPI = PAL::RenderAPI::VulkanAPIServiceLocator::Service();

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
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    
    mDevice->CreateRenderPass(&renderPassInfo, nullptr, &mRenderPass);
    
    constexpr size_t MAX_FRAMES = 2;
    
    mFrameData.imageAvailableSemaphore.resize(MAX_FRAMES);
    mFrameData.renderFinishedSemaphore.resize(MAX_FRAMES);
    mFrameData.frameFence.resize(MAX_FRAMES);
    
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create in signaled state for first frame
    
    for(size_t id{ 0 }; id < MAX_FRAMES; ++id)
    {
        mDevice->CreateSemaphore(&semaphoreInfo, nullptr, &mFrameData.imageAvailableSemaphore[id]);
        mDevice->CreateSemaphore(&semaphoreInfo, nullptr, &mFrameData.renderFinishedSemaphore[id]);
        mDevice->CreateFence(&fenceInfo, nullptr, &mFrameData.frameFence[id]);
    }
    
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = 0;
    poolInfo.flags = 0; // Optional
    
    mDevice->CreateCommandPool(&poolInfo, nullptr, &mCommandPool);
    
    VkDeviceSize bufferSize = sizeof(Transform);
    
    uniformBuffers.resize(2);
    uniformBuffersMemory.resize(2);
    
    for (size_t i = 0; i < 2; ++i)
    {
        CreateBufferInternal(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VK_SHARING_MODE_EXCLUSIVE, uniformBuffers[i], uniformBuffersMemory[i]);
    }
    
    mImgFormat = VK_FORMAT_B8G8R8A8_UNORM;
    
    Matrix4 m;
    m.MakeIdentity();
    
    mView = m;
    mProjection = m;
    mModel = m;
    
    // Create descriptor set layout
    CreateDescriptorSetLayout();
    
    // Create descriptor pool
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(2);        // Depends on swap chain images cnt
    
    VkDescriptorPoolCreateInfo descPoolInfo{};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &poolSize;
    descPoolInfo.maxSets = static_cast<uint32_t>(2);        // Depends on swap chain images cnt
    
    mDevice->CreateDescriptorPool(&descPoolInfo, nullptr, &mDescriptorPool);
    
    // Create descriptor sets
    std::vector<VkDescriptorSetLayout> layouts(2, descriptorSetLayout); // Depends on swap chain images cnt
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(2);        // Depends on swap chain images cnt
    allocInfo.pSetLayouts = layouts.data();
    
    mDescriptorSets.resize(2);           // Depends on swap chain images cnt
    mDevice->AllocateDescriptorSets(&allocInfo, mDescriptorSets.data());
    
    for (size_t i = 0; i < 2; i++)          // Depends on swap chain images cnt
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(Transform);
        
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = mDescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        
        mDevice->UpdateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
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
        
        VkClearValue clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
        
        const auto& indexStream = object.mVertexBuffer.GetIndexDataStream();
        
        const auto& positionBuffer = object.mVertexBuffer.GetPositionDataStream().GetDeviceResourcePtr();
        const auto& colorBuffer = object.mVertexBuffer.GetColorDataStream().GetDeviceResourcePtr();
        const auto& indexBuffer = object.mVertexBuffer.GetIndexDataStream().GetDeviceResourcePtr();
        
        BufferObjectVisitor positionVisitor, colorVisitor, indexVisitor;
        positionBuffer.Accept(positionVisitor);
        colorBuffer.Accept(colorVisitor);
        indexBuffer.Accept(indexVisitor);
        
        VkDeviceSize offset[] = { 0, 0 };
        VkBuffer buffers[]{ positionVisitor.buffer, colorVisitor.buffer };
        
        PipelineObjectVisitor visitor;
        pipeline.mDeviceObject.Accept(visitor);
        
        mDevice->BeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        mDevice->BindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, visitor.pipeline);
        mDevice->CmdBindVertexBuffer(mCommandBuffers[i], 0, object.mVertexBuffer.GetDataStreamCount(), buffers, offset);
        mDevice->CmdBindIndexBuffer(mCommandBuffers[i], indexVisitor.buffer, 0, (indexStream.GetStride() == sizeof(uint16_t)) ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
        mDevice->CmdBindDescriptorSets(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, visitor.layout, 0, 1, &mDescriptorSets[i], 0, nullptr);
        mDevice->CmdDrawIndexed(mCommandBuffers[i], indexStream.GetElementCount(), 1, 0, 0, 0);
        mDevice->EndRenderPass(mCommandBuffers[i]);
        mDevice->EndCommandBuffer(mCommandBuffers[i]);
    }
}

void Renderer::VulkanRenderer::Deinitialize()
{
}

void Renderer::VulkanRenderer::UpdateCamera(uint32_t imageIndex)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    Transform ubo;
    ubo.view.MakeIdentity();
    ubo.view.Translate(0.0f, 1.0f, 0.0f);
    ubo.view.RotateX(Math::DegreesToRadians(-90.0f));
    
    ubo.model.MakeIdentity();
    ubo.model.RotateX(time * Math::DegreesToRadians(40.0f));
    ubo.projection = Matrix4::MakePerspective(Math::DegreesToRadians(60.0f), 1280/720.f, 0.1f, 10.0f); //Matrix4::MakeOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    
    void* data;
    mDevice->MapMemory(uniformBuffersMemory[imageIndex], 0, sizeof(Transform), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    mDevice->UnmapMemory(uniformBuffersMemory[imageIndex]);
}

void VulkanRenderer::CreateDevice(DeviceType type)
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

void VulkanRenderer::CreateSwapChain(std::unique_ptr<SwapChainResource>& swapChain, void* nativeHandle, uint32_t width, uint32_t height)
{
    const auto& vulkanAPI = PAL::RenderAPI::VulkanAPIServiceLocator::Service();
    const auto& physicalDevice = mDevice->GetPhysicalDevice();
    
    const auto vulkanSurface = vulkanAPI.CreateWindowSurface(nativeHandle);
    const auto supported = vulkanAPI.GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, 0, vulkanSurface);
    if(supported)
    {
        auto vulkanSwapChain = std::make_unique<SwapChainVK>(mDevice, vulkanSurface);
        vulkanSwapChain->Initialize(width, height);
        vulkanSwapChain->SetSemaphore(mGraphicsQueue);
        
        mFramebuffers = vulkanSwapChain->GetFramebuffers();
        swapChain = std::move(vulkanSwapChain);
    }
    else
    {
        LOG(Error) << "Failed to create swap chain, unsupported surface";
    }
}

void VulkanRenderer::CreateShader(std::unique_ptr<RendererResource>& shader, const std::vector<uint8_t>& code) const
{
    auto vulkanShaderResource = std::make_unique<VulkanShaderModule>();
    
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    mDevice->CreateShaderModule(&createInfo, nullptr, &vulkanShaderResource->vulkanShader);
}

void VulkanRenderer::CreatePipeline(DeviceObject& object, const std::string& vs, const std::string& fs) const
{
    File vertexShaderFile(vs), framentShaderFile(fs);
    vertexShaderFile.Open(EFileAccessMode::Read);
    auto vertexShaderSource = vertexShaderFile.Read();
    
    VkShaderModuleCreateInfo vShaderCreateInfo{};
    vShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vShaderCreateInfo.codeSize = vertexShaderSource.size();
    vShaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertexShaderSource.data());
    
    VkShaderModule vulkanVertexShader{ VK_NULL_HANDLE };
    mDevice->CreateShaderModule(&vShaderCreateInfo, nullptr, &vulkanVertexShader);
    
    framentShaderFile.Open(EFileAccessMode::Read);
    auto framentShaderSource = framentShaderFile.Read();
    
    VkShaderModuleCreateInfo fShaderCreateInfo{};
    fShaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fShaderCreateInfo.codeSize = framentShaderSource.size();
    fShaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(framentShaderSource.data());
    
    VkShaderModule vulkanFragmentShader{ VK_NULL_HANDLE };
    mDevice->CreateShaderModule(&fShaderCreateInfo, nullptr, &vulkanFragmentShader);
    
    std::vector<VkPipelineShaderStageCreateInfo> stageInfos;
    
    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = vulkanFragmentShader;
    fragmentShaderStageInfo.pName = "main";
    
    stageInfos.push_back(std::move(fragmentShaderStageInfo));
    
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vulkanVertexShader;
    vertShaderStageInfo.pName = "main";
    
    stageInfos.push_back(std::move(vertShaderStageInfo));
    
    std::array<VkVertexInputBindingDescription, 2> bindingDescxriptions{};
    bindingDescxriptions[0].binding = 0;
    bindingDescxriptions[0].stride = 12;
    bindingDescxriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    bindingDescxriptions[1].binding = 1;
    bindingDescxriptions[1].stride = 12;
    bindingDescxriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = 0;
    
    attributeDescriptions[1].binding = 1;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = 0;
    
    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescxriptions.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescxriptions.data();
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
    
    VkPipeline pipeline{ VK_NULL_HANDLE };
    mDevice->CreateGraphicsPipeline(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    
    object = PipelineDeviceObject(pipeline, layout);
}

const FrameSyncData VulkanRenderer::GetFrameSyncData() const
{
    FrameSyncData data;
    data.imageAvailableSemaphore = mFrameData.imageAvailableSemaphore[mCurrentFrameId];
    data.renderFinishedSemaphore = mFrameData.renderFinishedSemaphore[mCurrentFrameId];
    data.frameFence = mFrameData.frameFence[mCurrentFrameId];
    
    return data;
}

uint32_t VulkanRenderer::FindMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties{ VK_NULL_HANDLE };
    
    const auto& renderAPI = PAL::RenderAPI::VulkanAPIServiceLocator::Service();
    renderAPI.GetPhysicalDeviceMemoryProperties(mDevice->GetPhysicalDevice(), &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
}

void VulkanRenderer::CreateBufferInternal(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode sharingMode, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.flags = 0;
    
    mDevice->CreateBuffer(&bufferInfo, nullptr, &buffer);
    
    VkMemoryRequirements memRequirements{};
    mDevice->GetBufferMemoryRequirements(buffer, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryTypeIndex(memRequirements.memoryTypeBits, properties);
    
    mDevice->AllocateMemory(&allocInfo, nullptr, &bufferMemory);
    mDevice->BindBufferMemory(buffer, bufferMemory, 0);
}

void VulkanRenderer::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = mCommandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer{ VK_NULL_HANDLE };
    mDevice->AllocateCommandBuffers(&allocInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    mDevice->BeginCommandBuffer(commandBuffer, &beginInfo);
    
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    mDevice->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    mDevice->EndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    mDevice->QueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    mDevice->QueueWaitIdle(mGraphicsQueue);     // Rather use fence.
    
    mDevice->FreeCommandBuffers(mCommandPool, 1, &commandBuffer);
}

void VulkanRenderer::CreateBuffer(const BufferDesc& desc, DeviceObject& bufferObject) const
{
    VkBuffer buffer{ VK_NULL_HANDLE };
    VkDeviceMemory memory{ VK_NULL_HANDLE };
    
    if(desc.memoryUsage & MemoryType::DeviceLocal)
    {
        VkBuffer stagingBuffer{ VK_NULL_HANDLE };
        VkDeviceMemory stagingMemory{ VK_NULL_HANDLE };
        
        constexpr auto stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        constexpr auto stagingMemoryType = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        
        const auto vulkanSharingMode = ToVulkanSharingMode(desc.sharingMode);
        
        CreateBufferInternal(desc.bufferSize, stagingBufferUsage, stagingMemoryType, vulkanSharingMode, stagingBuffer, stagingMemory);
        
        void* data{ nullptr };
        mDevice->MapMemory(stagingMemory, 0, desc.bufferSize, 0, &data);
        memcpy(data, desc.data, (size_t)desc.bufferSize);
        mDevice->UnmapMemory(stagingMemory);
        
        const auto vulkanMemoryType = ToVulkanMemoryProperty(desc.memoryUsage);
        const auto vulkanBufferUsage = ToVulkanBufferUsage(desc.usage);
        
        CreateBufferInternal(desc.bufferSize, vulkanBufferUsage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, vulkanMemoryType, vulkanSharingMode, buffer, memory);
        
        CopyBuffer(stagingBuffer, buffer, desc.bufferSize);
        
        mDevice->DestroyBuffer(stagingBuffer, nullptr);
        mDevice->FreeMemory(stagingMemory, nullptr);
    }
    else if(desc.memoryUsage & MemoryType::HostVisible)
    {
        void* data{ nullptr };
        mDevice->MapMemory(memory, 0, desc.bufferSize, 0, &data);
        memcpy(data, desc.data, (size_t)desc.bufferSize);
        mDevice->UnmapMemory(memory);
    }
    
    bufferObject = BufferDeviceObject(buffer, memory);
}

void VulkanRenderer::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;
    
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding;
    
    mDevice->CreateDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout);
}
