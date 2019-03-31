#include "SummitDemo.h"

#include <Logging/LoggingService.h>
#include <Engine/Engine.h>
#include <Engine/Window.h>
#include <Renderer/View.h>
#include <Renderer/Image.h>
#include <Renderer/Effect.h>
#include <Math/Math.h>

using namespace Demo;
using namespace Summit;
using namespace Renderer;

#ifdef LOG_MODULE_ID
#undef LOG_MODULE_ID
#endif

#ifdef LOGGER_ID
#undef LOGGER_ID
#define LOGGER_ID "Application"
#endif

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

SummitDemo::SummitDemo(SummitEngine& engine)
{
    Logging::LoggingServiceLocator::Service().AddLogger(std::make_unique<Logging::Logger>(LOGGER_ID));
    LOG(Information) << "SummitApp did finish launching!";
    
    window = new Application::Window("SummitEngine", 1280, 720);
    window->SetTitle("SummitEngineApplication");
    
    auto& positionStream = triangle.mVertexBuffer.GetPositionDataStream();
    auto& colorStream = triangle.mVertexBuffer.GetColorDataStream();
    auto& indexStream = triangle.mVertexBuffer.GetIndexDataStream();
    auto& texCoordStream = triangle.mVertexBuffer.GetTexCoordDataStream();
    
    auto& positionData = positionStream.GetData();
    auto& colorData = colorStream.GetData();
    auto& indexData = indexStream.GetData();
    auto& texCoordData = texCoordStream.GetData();
    
    // Quad 1
    positionData.push_back({ -0.5f, 0.0f, -0.5f });
    positionData.push_back({ 0.5f, 0.0f, -0.5f });
    positionData.push_back({ 0.5f, 0.0f, 0.5f });
    positionData.push_back({ -0.5f, 0.0f, 0.5f });
    
    // Quad 2
    positionData.push_back({ -0.5f, 0.5f, -0.5f });
    positionData.push_back({ 0.5f, 0.5f, -0.5f });
    positionData.push_back({ 0.5f, 0.5f, 0.5f });
    positionData.push_back({ -0.5f, 0.5f, 0.5f });
    
    colorData.push_back({ 1.0f, 0.0f, 0.0f });
    colorData.push_back({ 0.0f, 1.0f, 0.0f });
    colorData.push_back({ 0.0f, 0.0f, 1.0f });
    colorData.push_back({ 1.0f, 0.0f, 1.0f });
    
    colorData.push_back({ 1.0f, 0.0f, 0.0f });
    colorData.push_back({ 0.0f, 1.0f, 0.0f });
    colorData.push_back({ 0.0f, 0.0f, 1.0f });
    colorData.push_back({ 1.0f, 0.0f, 1.0f });
    
    texCoordData.push_back({ 2.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 2.0f });
    texCoordData.push_back({ 2.0f, 2.0f });
    
    texCoordData.push_back({ 1.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 1.0f });
    texCoordData.push_back({ 1.0f, 1.0f });
    
    indexData = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4 };
    
    positionStream.Lock(CommitCommand::Commit);
    colorStream.Lock(CommitCommand::Commit);
    texCoordStream.Lock(CommitCommand::Commit);
    indexStream.Lock(CommitCommand::Commit);
    
    auto& renderer = Renderer::RendererLocator::GetRenderer();
    
    // Setup stages
    pipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/vert.spv");
    pipeline.effect.AddModule(ModuleStage::Fragment, "/Users/tomaskubovcik/Dev/SummitEngine/frag.spv");
    
    // Setup attributes
    pipeline.effect.AddAttribute(Format::R32G32B32F, 0);
    pipeline.effect.AddAttribute(Format::R32G32B32F, 1);
    pipeline.effect.AddAttribute(Format::R32G32F, 2);
    
    // Setup uniforms
    pipeline.effect.AddUniform(UniformType::Buffer, ModuleStage::Vertex, 0, 1);
    pipeline.effect.AddUniform(UniformType::Sampler, ModuleStage::Fragment, 1, 1);
    
    pipeline.Create();
    
    renderer.CreateCommandBuffers(pipeline, triangle);
    
    Matrix4 m;
    m.MakeIdentity();
    
    mView = m;
    mProjection = m;
    mModel = m;
}

void SummitDemo::PushToEngine(SummitEngine& engine)
{
    mEarlyUpdateConnection = engine.EarlyUpdate.connect(&Demo::SummitDemo::OnEarlyUpdate, this);
    mUpdateConnection = engine.Updatee.connect(&Demo::SummitDemo::OnUpdate, this);
    mLateUpdateConnection = engine.LateUpdate.connect(&Demo::SummitDemo::OnLateUpdate, this);
    mRenderConnection = engine.Render.connect(&Demo::SummitDemo::OnRender, this);
    mUIRenderConnection = engine.UIRender.connect(&Demo::SummitDemo::OnUIRender, this);
}

void SummitDemo::PopFromEngine(SummitEngine& engine)
{
    mEarlyUpdateConnection.disconnect();
    mUpdateConnection.disconnect();
    mLateUpdateConnection.disconnect();
    mRenderConnection.disconnect();
    mUIRenderConnection.disconnect();
}

void SummitDemo::UpdateCamera()
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    struct MVP
    {
        Matrix4 model;
        Matrix4 view;
        Matrix4 projection;
    };
    
    MVP mvp;
    mvp.view.MakeIdentity();
    mvp.view.Translate(0.0f, 1.0f, -2.0f);
    mvp.view.RotateX(Math::DegreesToRadians(-60.0f));
    mvp.model.MakeIdentity();
    mvp.model.RotateY(time * Math::DegreesToRadians(40.0f));
    mvp.projection = Matrix4::MakePerspective(Math::DegreesToRadians(60.0f), 1280/720.f, 0.1f, 10.0f); //Matrix4::MakeOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    auto& renderer = Renderer::RendererLocator::GetRenderer();
    renderer.MapMemory(3 * sizeof(Matrix4), &mvp);
}

void SummitDemo::OnEarlyUpdate(const FrameData& data)
{
    
}

void SummitDemo::OnUpdate(const FrameData& data)
{
    UpdateCamera();
    window->Update();
}

void SummitDemo::OnLateUpdate(const FrameData& data)
{
    
}

void SummitDemo::OnRender(const FrameData& data)
{
    
}

void SummitDemo::OnUIRender(const FrameData& data)
{
    
}
