#include "SummitDemo.h"

#include <Engine/Engine.h>
#include <Engine/Window.h>
#include <Renderer/View.h>
#include <Renderer/Image.h>
#include <Renderer/Effect.h>
#include <Math/Math.h>

using namespace Demo;
using namespace Summit;
using namespace Renderer;
using namespace Application;

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

SummitDemo::SummitDemo(SummitEngine& engine)
{
    mEngine = &engine;
    
    mWindow = std::make_unique<Window>("SummitEngine", 1280, 720);
    mWindow->CreateView(1280, 720, 0, 0);
    //mWindow->CreateView(640, 720, 640, 0);
    
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
    
    pipeline.Create();
    
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
    
    engine.SetMainView(mWindow->GetView());
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
    
    mView.MakeIdentity();
    mView.Translate(0.0f, 1.0f, -2.0f);
    mView.RotateX(Math::DegreesToRadians(-60.0f));
    mModel.MakeIdentity();
    mModel.RotateY(time * Math::DegreesToRadians(40.0f));
    mProjection = Matrix4::MakePerspective(Math::DegreesToRadians(60.0f), 1400/900.f, 0.1f, 10.0f); //Matrix4::MakeOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
}

void SummitDemo::OnEarlyUpdate(const FrameData& data)
{
    
}

void SummitDemo::OnUpdate(const FrameData& data)
{
    UpdateCamera();
    
    mEngine->SetActiveSwapChain(mWindow->GetView()->GetSwapChain());
}

void SummitDemo::OnLateUpdate(const FrameData& data)
{
    
}

void SummitDemo::OnRender(const FrameData& data)
{
    auto& renderer = RendererLocator::GetRenderer();
    
    struct MVP
    {
        Matrix4 model;
        Matrix4 view;
        Matrix4 projection;
    };
    
    // TODO move MVP struct to class as member, this is unnecesary copy every frame I know
    MVP mvp;
    mvp.model = mModel;
    mvp.view = mView;
    mvp.projection = mProjection;
    
    renderer.MapMemory(mUniformBuffer.deviceObject, mUniformBuffer.dataSize, &mvp);
    
    mEngine->RenderObject(triangle, pipeline);
}

void SummitDemo::OnUIRender(const FrameData& data)
{
}
