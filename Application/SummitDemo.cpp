#include "SummitDemo.h"

#include <Engine/Engine.h>
#include <Engine/Window.h>
#include <Renderer/View.h>
#include <Renderer/Image.h>
#include <Renderer/Effect.h>
#include <Math/Math.h>
#include <Core/TupleHash.h>

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace Demo;
using namespace Summit;
using namespace Renderer;
using namespace Application;

struct Vertex
{
    bool operator==(const Vertex& other) const {
        return  position.x == other.position.x &&
        position.y == other.position.y &&
        position.z == other.position.z &&
        color.x == other.color.x &&
        color.y == other.color.y &&
        color.z == other.color.z &&
        texCoord.x == other.texCoord.x &&
        texCoord.y == other.texCoord.y;
    }
    
    Vector3f position;
    Vector3f color;
    Vector2f texCoord;
};

namespace std
{
    template<>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            auto t = std::tie(vertex.position, vertex.color, vertex.texCoord);
            return std::hash<decltype(t)>()(t);
        }
    };
}

#define LOG_MODULE_ID LOG_MODULE_4BYTE(' ','A','P','P')

SummitDemo::SummitDemo(SummitEngine& engine)
{
    mEngine = &engine;
    
    mWindow = std::make_unique<Window>("SummitEngine", 1280, 720);
    mWindow->CreateView(1280, 720, 0, 0);
    
    PrepareChalet();


    mModel.MakeIdentity();
    
    mLastCursorPosition = Vector2<uint16_t>(640, 360);
}

void SummitDemo::PrepareCube()
{
    auto& positionStream = triangle.mVertexBuffer.GetPositionDataStream();
    auto& colorStream = triangle.mVertexBuffer.GetColorDataStream();
    auto& indexStream = triangle.mVertexBuffer.GetIndexDataStream();
    auto& texCoordStream = triangle.mVertexBuffer.GetTexCoordDataStream();
    
    auto& positionData = positionStream.GetData();
    auto& colorData = colorStream.GetData();
    auto& indexData = indexStream.GetData();
    auto& texCoordData = texCoordStream.GetData();
    
    // Quad 1
    positionData.push_back({ -0.5f, -0.5f, -0.5f });
    positionData.push_back({ 0.5f, -0.5f, -0.5f });
    positionData.push_back({ 0.5f, -0.5f, 0.5f });
    positionData.push_back({ -0.5f, -0.5f, 0.5f });
    
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
    
    texCoordData.push_back({ 1.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 1.0f });
    texCoordData.push_back({ 1.0f, 1.0f });
    
    texCoordData.push_back({ 1.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 0.0f });
    texCoordData.push_back({ 0.0f, 1.0f });
    texCoordData.push_back({ 1.0f, 1.0f });
    
    indexData = {
        0, 1, 2, 2, 3, 0, // bottom
        4, 5, 6, 6, 7, 4, // top
        4, 7, 3, 3, 0, 4, // left
        6, 5, 1, 1, 2, 6, // right
        4, 5, 1, 1, 0, 4, // back
        7, 6, 2, 2, 3, 7  // forward
    };
    
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
    
    pipeline.depthTestEnabled = true;
    pipeline.Create();
}

void SummitDemo::PrepareChalet()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "/Users/tomaskubovcik/Dev/SummitEngine/chalet.obj"))
    {
        throw std::runtime_error(warn + err);
    }
    
    auto& positionStream = mChalet.mVertexBuffer.GetPositionDataStream();
    auto& colorStream = mChalet.mVertexBuffer.GetColorDataStream();
    auto& indexStream = mChalet.mVertexBuffer.GetIndexDataStream();
    auto& texCoordStream = mChalet.mVertexBuffer.GetTexCoordDataStream();
    
    auto& positionData = positionStream.GetData();
    auto& colorData = colorStream.GetData();
    auto& indexData = indexStream.GetData();
    auto& texCoordData = texCoordStream.GetData();
    
    std::vector<Vertex> vertices;
    std::unordered_map<Vertex, uint32_t> uniqueVertices = {};
    
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex v;
            v.position = Vector3f({
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]});
            
            v.texCoord = Vector2f({
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            });
            
            v.color = Vector3f({ 1.0f, 1.0f, 1.0f });

            if (uniqueVertices.count(v) == 0)
            {
                uniqueVertices[v] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(v);
            }
            
            indexData.push_back(uniqueVertices[v]);
        }
    }
    
    for(const auto& vertex : vertices)
    {
        positionData.push_back(vertex.position);
        colorData.push_back(vertex.color);
        texCoordData.push_back(vertex.texCoord);
    }
    
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
    pipeline.Create();
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
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    const auto framebufferWidth = mWindow->GetView()->GetWidth();
    const auto framebufferHeight = mWindow->GetView()->GetHeight();
    
    mModel.MakeIdentity();
    //mModel.RotateY(time * Math::DegreesToRadians(40.0f));
    
    mCamera.Update(framebufferWidth, framebufferHeight);
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
    mvp.view = mCamera.GetViewMatrix();
    mvp.projection = mCamera.GetProjectionMatrix();
    
    renderer.MapMemory(mUniformBuffer.deviceObject, mUniformBuffer.dataSize, &mvp);
    
    mEngine->RenderObject(mChalet, pipeline);
}

void SummitDemo::OnUIRender(const FrameData& data)
{
}
