#include "SummitDemo.h"

#include <Engine/Engine.h>
#include <Engine/Window.h>
#include <Renderer/View.h>
#include <Renderer/Resources/Texture.h>
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
    
    mUniformBuffer.offset = 0;
    mUniformBuffer.dataSize = 3 * sizeof(Matrix4);
    
    BufferDesc mvpUboDesc;
    mvpUboDesc.bufferSize = mUniformBuffer.dataSize;
    mvpUboDesc.usage = BufferUsage::UniformBuffer;
    mvpUboDesc.memoryUsage = MemoryType(MemoryType::HostVisible | MemoryType::HostCoherent);
    renderer.CreateBuffer(mvpUboDesc, mUniformBuffer.deviceObject);
    
    SetupRenderPass();
    
    mWindow = std::make_unique<Window>("SummitEngine", defaultViewWidth, defaultViewHeight);
    mWindow->CreateView(defaultViewWidth, defaultViewHeight, 0, 0);
    
    const auto viewPtr = mWindow->GetView();
    viewPtr->SetSwapChain(renderer.CreateSwapChain(viewPtr->GetDeviceObject(), mAdvancedRenderPass.GetDeviceObject(), viewPtr->GetWidth(), viewPtr->GetHeight()));
    
    depthPrePassPipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/depth_pre_pass.spv");
    depthPrePassPipeline.effect.AddAttribute(Format::R32G32B32F, 0);
    depthPrePassPipeline.effect.AddUniformBuffer(ModuleStage::Vertex, 0, mUniformBuffer);
    depthPrePassPipeline.depthTestEnabled = true;
    depthPrePassPipeline.depthWriteEnabled = true;
    renderer.CreatePipeline(depthPrePassPipeline, mAdvancedRenderPass.GetDeviceObject());

    mQuad = std::make_unique<Quad>();

    // Setup stages
    mQuadPipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/quad_vert.spv");
    mQuadPipeline.effect.AddModule(ModuleStage::Fragment, "/Users/tomaskubovcik/Dev/SummitEngine/quad_frag.spv");

    // Setup attributes
    mQuadPipeline.effect.AddAttribute(Format::R32G32F, 0);
    mQuadPipeline.effect.AddAttribute(Format::R32G32F, 1);
    
    auto& framebuffer = mWindow->GetView()->GetSwapChain()->GetActiveFramebuffer();
    const auto depthAttachments = framebuffer.GetAttachment(AttachmentType::DepthStencil);
    if(!depthAttachments.empty())
    {
        mQuadPipeline.effect.AddUniformBuffer(ModuleStage::Vertex, 0, mUniformBuffer);
        mQuadPipeline.effect.AddTexture(ModuleStage::Fragment, 1, *depthAttachments.back());

        renderer.CreatePipeline(mQuadPipeline, mAdvancedRenderPass.GetDeviceObject());
        mQuadPipeline.mViewPort = Vector2f(200.0f, 40.0f);
        mQuadPipeline.mOffset = Vector2f(400.0f, 400.0f);
    }
    
    PrepareCube();
}

void SummitDemo::PrepareCube()
{
    mObject = std::make_unique<Cube>();
    
    auto& renderer = Renderer::RendererLocator::GetRenderer();
    
    mTexture = std::make_unique<Texture>(Texture::CreateFromFile("/Users/tomaskubovcik/Dev/SummitEngine/texture.jpg"));

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
    pipeline.mSubpassIndex = 1;
    renderer.CreatePipeline(pipeline, mAdvancedRenderPass.GetDeviceObject());
}

void SummitDemo::PrepareChalet()
{
    mObject = std::make_unique<Chalet>();
    
    auto& renderer = Renderer::RendererLocator::GetRenderer();
    
    mTexture = std::make_unique<Texture>(Texture::CreateFromFile("/Users/tomaskubovcik/Dev/SummitEngine/chalet.jpg"));
    
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
    pipeline.mSubpassIndex = 1;
    renderer.CreatePipeline(pipeline, mAdvancedRenderPass.GetDeviceObject());
}

void SummitDemo::PrepareSponza()
{
//    auto& renderer = Renderer::RendererLocator::GetRenderer();
//    
//    // Setup render pass
//    mSponzaRenderPass.AddAttachment(AttachmentType::Color, Format::R32G32B32F, ImageLayout::ShaderReadOnly);
//    mSponzaRenderPass.AddAttachment(AttachmentType::Color, Format::R32G32B32F, ImageLayout::ShaderReadOnly);
//    mSponzaRenderPass.AddAttachment(AttachmentType::Color, Format::B8G8R8A8, ImageLayout::ShaderReadOnly);
//    mSponzaRenderPass.AddAttachment(AttachmentType::DepthStencil, Format::D32F, ImageLayout::DepthAttachment);
//    renderer.CreateRenderPass(mSponzaRenderPass);
//    
//    mGBuffer.Resize(1280, 720);
//    mGBuffer.AddAttachment(Format::R32G32B32F, AttachmentType::Color, Graphics::ColorBlack);
//    mGBuffer.AddAttachment(Format::R32G32B32F, AttachmentType::Color, Graphics::ColorBlack);
//    mGBuffer.AddAttachment(Format::B8G8R8A8, AttachmentType::Color, Graphics::ColorBlack);
//    mGBuffer.AddAttachment(Format::D32F, AttachmentType::DepthStencil, Graphics::ClearValueDepthStencil);
//    
//    renderer.CreateFramebuffer(mGBuffer, mSponzaRenderPass);
//    
//    mSponzaRenderPass.SetActiveFramebuffer(mGBuffer);
//    mSponzaRenderPass.BeginEmitter.connect(&Demo::SummitDemo::OnRender, this);
}

void SummitDemo::SetupRenderPass()
{
    const auto depthId = mAdvancedRenderPass.AddAttachment(AttachmentDesc{ Format::D32F, ImageLayout::Undefined, ImageLayout::DepthAttachment});
    const auto colorId = mAdvancedRenderPass.AddAttachment(AttachmentDesc{ Format::B8G8R8A8, ImageLayout::Undefined, ImageLayout::Present});
    
    // Setup sub passes
    auto& depthPrePass = mAdvancedRenderPass.CreateSubpass();
    depthPrePass.AddAttachmentRef(AttachmentType::DepthStencil, depthId, ImageLayout::DepthAttachment);
    
    auto& colorPass = mAdvancedRenderPass.CreateSubpass();
    colorPass.AddAttachmentRef(AttachmentType::DepthStencil, depthId, ImageLayout::DepthStencilReadOnly);
    colorPass.AddAttachmentRef(AttachmentType::Color, colorId, ImageLayout::ColorAttachment);
    
    DependencyDesc depthPassDependency;
    depthPassDependency.srcIdx = ~0U;
    depthPassDependency.dstIdx = 0;
    depthPassDependency.srcStageMask = StageMask::LateFragmentTest;
    depthPassDependency.dstStageMask = StageMask::EarlyFragmentTest;
    depthPassDependency.srcAccessMask = AccessMask::DepthStencilWrite;
    depthPassDependency.dstAccessMask = AccessMask::DepthStencilRead;
    
    DependencyDesc colorPassDependency;
    colorPassDependency.srcIdx = 0;
    colorPassDependency.dstIdx = 1;
    colorPassDependency.srcStageMask = StageMask::ColorAttachment;
    colorPassDependency.dstStageMask = StageMask::FragmentShader;
    colorPassDependency.srcAccessMask = AccessMask::ColorWrite;
    colorPassDependency.dstAccessMask = AccessMask::ShaderRead;
    
    mAdvancedRenderPass.SetDependency(depthPassDependency);
    mAdvancedRenderPass.SetDependency(colorPassDependency);
    
    mEngine->GetRenderer().CreateRenderPass(mAdvancedRenderPass);
    
    mAdvancedRenderPass.EarlyBeginEmitter.connect(&Demo::SummitDemo::OnEarlyRender, this);
    mAdvancedRenderPass.BeginEmitter.connect(&Demo::SummitDemo::OnRender, this);
}

void SummitDemo::PushToEngine(SummitEngine& engine)
{
    mEarlyUpdateConnection = engine.EarlyUpdate.connect(&Demo::SummitDemo::OnEarlyUpdate, this);
    mUpdateConnection = engine.Updatee.connect(&Demo::SummitDemo::OnUpdate, this);
    mLateUpdateConnection = engine.LateUpdate.connect(&Demo::SummitDemo::OnLateUpdate, this);
    
    engine.RegisterRenderPass(mAdvancedRenderPass);
    engine.SetMainView(mWindow->GetView());
    
    mWindow->GetView()->MouseEvent.connect(&SummitDemo::OnMouseEvent, this);
}

void SummitDemo::PopFromEngine(SummitEngine& engine)
{
    mEarlyUpdateConnection.disconnect();
    mUpdateConnection.disconnect();
    mLateUpdateConnection.disconnect();
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
    const auto framebufferWidth = mWindow->GetView()->GetWidth();
    const auto framebufferHeight = mWindow->GetView()->GetHeight();
    
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

void SummitDemo::OnDepthPrePass()
{
//    const auto width = mDepthPrePass.GetActiveFramebuffer()->GetWidth();
//    const auto height = mDepthPrePass.GetActiveFramebuffer()->GetHeight();
//
//    mEngine->GetRenderer().SetViewport(Rectangle<float>(width, height));
//    mEngine->GetRenderer().SetScissor(Rectangle<uint32_t>(width, height));
//    mEngine->RenderObject(*mObject, depthPrePassPipeline);
}

void SummitDemo::OnEarlyRender()
{
    mAdvancedRenderPass.SetActiveFramebuffer(mWindow->GetView()->GetSwapChain()->GetActiveFramebuffer());
}

void SummitDemo::OnRender()
{
    const auto width = mAdvancedRenderPass.GetActiveFramebuffer()->GetWidth();
    const auto height = mAdvancedRenderPass.GetActiveFramebuffer()->GetHeight();
    
    mEngine->GetRenderer().SetViewport(Rectangle<float>(width, height));
    mEngine->GetRenderer().SetScissor(Rectangle<uint32_t>(width, height));
    mEngine->RenderObject(*mObject, depthPrePassPipeline);
    
    mEngine->GetRenderer().NextSubpass();
    mEngine->RenderObject(*mObject, pipeline);
    
//    mEngine->GetRenderer().SetViewport(Rectangle<float>(280.0f, 280.0f * height/ width, 1000.0f, 0.0f));
//    mEngine->GetRenderer().SetScissor(Rectangle<uint32_t>(280.0f, 280.0f * height/ width, 1000.0f, 0.0f));
//    mEngine->RenderObject(*mQuad, mQuadPipeline);
}
