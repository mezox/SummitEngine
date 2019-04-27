#include "SummitDemo.h"

#include <Engine/Engine.h>
#include <Engine/Window.h>
#include <Renderer/View.h>
#include <Renderer/Image.h>
#include <Renderer/Effect.h>
#include <Math/Math.h>
#include <Core/TupleHash.h>

#include "Cube.h"
#include "Chalet.h"

#include <iostream>

using namespace Demo;
using namespace Summit;
using namespace Renderer;
using namespace Application;

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

SummitDemo::SummitDemo(SummitEngine& engine)
{
    constexpr uint32_t defaultViewWidth = 1280;
    constexpr uint32_t defaultViewHeight = 720;
    
    mEngine = &engine;
    
    auto& renderer = mEngine->GetRenderer();
    
    // ----- setup depth pre pass
    mDepthPrePass.AddAttachment(AttachmentType::Depth, Format::D32F, ImageLayout::DepthAttachment);
    renderer.CreateRenderPass(mDepthPrePass);

    mDepthPrePassFB.Resize(defaultViewWidth, defaultViewHeight);
    
    Attachment depthAttachment(Format::D32F, AttachmentType::Depth);
    depthAttachment.SetClearValue(Graphics::Color(255, 0, 0, 0));
    mDepthPrePassFB.AddAttachment(std::make_shared<Attachment>(std::move(depthAttachment)));
    renderer.CreateFramebuffer(mDepthPrePassFB, mDepthPrePass);
    
    mDepthPrePass.SetActiveFramebuffer(mDepthPrePassFB);
    // ----- end of setup of depth pre pass
    
    mDefaultRenderPass.AddAttachment(AttachmentType::Color, Format::B8G8R8A8, ImageLayout::Present);
    mDefaultRenderPass.AddAttachment(AttachmentType::Depth, Format::D32F, ImageLayout::DepthAttachment);
    renderer.CreateRenderPass(mDefaultRenderPass);
    
    mWindow = std::make_unique<Window>("SummitEngine", defaultViewWidth, defaultViewHeight);
    mWindow->CreateView(defaultViewWidth, defaultViewHeight, 0, 0);
    
    const auto viewPtr = mWindow->GetView();
    viewPtr->SetSwapChain(renderer.CreateSwapChain(viewPtr->GetDeviceObject(), mDefaultRenderPass.GetDeviceObject(), viewPtr->GetWidth(), viewPtr->GetHeight()));
    
//    mEngine->RegisterRenderPass(mDepthPrePass);
//    mEngine->RegisterRenderPass(mDefaultRenderPass);
    
    PrepareCube();
}

void SummitDemo::PrepareCube()
{
    mObject = std::make_unique<Cube>();
    
    auto& renderer = Renderer::RendererLocator::GetRenderer();
    
    mUniformBuffer.offset = 0;
    mUniformBuffer.dataSize = 3 * sizeof(Matrix4);
    
    BufferDesc mvpUboDesc;
    mvpUboDesc.bufferSize = mUniformBuffer.dataSize;
    mvpUboDesc.usage = BufferUsage::UniformBuffer;
    mvpUboDesc.memoryUsage = MemoryType(MemoryType::HostVisible | MemoryType::HostCoherent);
    renderer.CreateBuffer(mvpUboDesc, mUniformBuffer.deviceObject);
    
    mTexture = std::make_unique<Image>(Image::CreateFromFile("/Users/tomaskubovcik/Dev/SummitEngine/texture.jpg"));
    
    // Setup stages
    pipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/vert.spv");
    pipeline.effect.AddModule(ModuleStage::Fragment, "/Users/tomaskubovcik/Dev/SummitEngine/frag.spv");
    
    // Setup attributes
    pipeline.effect.AddAttribute(Format::R32G32B32F, 0);
    pipeline.effect.AddAttribute(Format::R32G32B32F, 1);
    pipeline.effect.AddAttribute(Format::R32G32F, 2);
    
    // Setup uniforms
    pipeline.effect.AddUniformBuffer(ModuleStage::Vertex, 0, mUniformBuffer);
    pipeline.effect.AddTexture(ModuleStage::Fragment, 1, *mTexture.get());
    
    pipeline.depthTestEnabled = true;
    renderer.CreatePipeline(pipeline, mDefaultRenderPass.GetDeviceObject());
    
    depthPrePassPipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/depth_pre_pass.spv");
    depthPrePassPipeline.effect.AddAttribute(Format::R32G32B32F, 0);
    depthPrePassPipeline.effect.AddUniformBuffer(ModuleStage::Vertex, 0, mUniformBuffer);
    depthPrePassPipeline.depthTestEnabled = true;
    renderer.CreatePipeline(depthPrePassPipeline, mDepthPrePass.GetDeviceObject());
}

void SummitDemo::PrepareChalet()
{
    mObject = std::make_unique<Chalet>();
    
    auto& renderer = Renderer::RendererLocator::GetRenderer();
    
    mUniformBuffer.offset = 0;
    mUniformBuffer.dataSize = 3 * sizeof(Matrix4);
    
    BufferDesc mvpUboDesc;
    mvpUboDesc.bufferSize = mUniformBuffer.dataSize;
    mvpUboDesc.usage = BufferUsage::UniformBuffer;
    mvpUboDesc.memoryUsage = MemoryType(MemoryType::HostVisible | MemoryType::HostCoherent);
    renderer.CreateBuffer(mvpUboDesc, mUniformBuffer.deviceObject);
    
    mTexture = std::make_unique<Image>(Image::CreateFromFile("/Users/tomaskubovcik/Dev/SummitEngine/chalet.jpg"));
    
    // Setup stages
    pipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/vert.spv");
    pipeline.effect.AddModule(ModuleStage::Fragment, "/Users/tomaskubovcik/Dev/SummitEngine/frag.spv");
    
    // Setup attributes
    pipeline.effect.AddAttribute(Format::R32G32B32F, 0);
    pipeline.effect.AddAttribute(Format::R32G32B32F, 1);
    pipeline.effect.AddAttribute(Format::R32G32F, 2);
    
    // Setup uniforms
    pipeline.effect.AddUniformBuffer(ModuleStage::Vertex, 0, mUniformBuffer);
    pipeline.effect.AddTexture(ModuleStage::Fragment, 1, *mTexture.get());
    
    pipeline.depthTestEnabled = true;
    renderer.CreatePipeline(pipeline, mDefaultRenderPass.GetDeviceObject());
}

void SummitDemo::PushToEngine(SummitEngine& engine)
{
    mEarlyUpdateConnection = engine.EarlyUpdate.connect(&Demo::SummitDemo::OnEarlyUpdate, this);
    mUpdateConnection = engine.Updatee.connect(&Demo::SummitDemo::OnUpdate, this);
    mLateUpdateConnection = engine.LateUpdate.connect(&Demo::SummitDemo::OnLateUpdate, this);
    mRenderConnection = engine.Render.connect(&Demo::SummitDemo::OnRender, this);
    mUIRenderConnection = engine.UIRender.connect(&Demo::SummitDemo::OnUIRender, this);
    
    engine.SetMainView(mWindow->GetView());
    
    mWindow->GetView()->MouseEvent.connect(&SummitDemo::OnMouseEvent, this);
}

void SummitDemo::PopFromEngine(SummitEngine& engine)
{
    mEarlyUpdateConnection.disconnect();
    mUpdateConnection.disconnect();
    mLateUpdateConnection.disconnect();
    mRenderConnection.disconnect();
    mUIRenderConnection.disconnect();
}

void SummitDemo::OnMouseEvent(Core::MouseEvent& event)
{
    if(event.type == Core::MouseEventType::RightDrag)
    {
        mCamera.mTransform.rotation.x += Math::DegreesToRadians(event.offsetY);
        mCamera.mTransform.rotation.y += Math::DegreesToRadians(event.offsetX);
    }
}

void SummitDemo::UpdateCamera()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    //float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    const auto framebufferWidth = mWindow->GetView()->GetWidth();
    const auto framebufferHeight = mWindow->GetView()->GetHeight();
    //mModel.RotateY(time * Math::DegreesToRadians(40.0f));
    
    mCamera.Update(framebufferWidth, framebufferHeight);
    
    struct MVP
    {
        Matrix4 model;
        Matrix4 view;
        Matrix4 projection;
    };
    
    Matrix4 modelMatrix;
    modelMatrix.MakeIdentity();
    
    MVP mvp;
    mvp.model = modelMatrix;
    mvp.view = mCamera.GetViewMatrix();
    mvp.projection = mCamera.GetProjectionMatrix();
    
    mEngine->GetRenderer().MapMemory(mUniformBuffer.deviceObject, mUniformBuffer.dataSize, &mvp);
}

void SummitDemo::OnEarlyUpdate(const FrameData& data)
{
    mEngine->SetActiveSwapChain(mWindow->GetView()->GetSwapChain());
}

void SummitDemo::OnUpdate(const FrameData& data)
{
    UpdateCamera();
}

void SummitDemo::OnLateUpdate(const FrameData& data)
{
    
}

void SummitDemo::OnRender(const FrameData& data)
{
    mDepthPrePass.SetActiveFramebuffer(mDepthPrePassFB);
    mEngine->GetRenderer().BeginRenderPass(mDepthPrePass);
    mEngine->RenderObject(*mObject, depthPrePassPipeline);
    mEngine->GetRenderer().EndRenderPass();
    
    mDefaultRenderPass.SetActiveFramebuffer(mWindow->GetView()->GetSwapChain()->GetActiveFramebuffer());
    mEngine->GetRenderer().BeginRenderPass(mDefaultRenderPass);
    mEngine->RenderObject(*mObject, pipeline);
    mEngine->GetRenderer().EndRenderPass();
}

void SummitDemo::OnUIRender(const FrameData& data)
{
}
