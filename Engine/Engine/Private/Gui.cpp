#include <Engine/Gui.h>
#include <Engine/Engine.h> //TODO: Remove, its here only because of frame data declaration

#include <Logging/LoggingService.h>

#include <imgui/imgui.h>
#include <iostream>

using namespace Summit::UI;
using namespace Renderer;

Gui::Gui(Renderer::View& parent)
{
    mMouseEventConnection = parent.MouseEvent.connect(&Gui::OnMouseEvent, this);
    
    mGuiContext = ImGui::CreateContext();
    
//    auto& style = ImGui::GetStyle();
//    style.FrameRounding = 0.f;
//    style.ScrollbarRounding = 0.f;
//    style.WindowRounding = 0.f;
//    
//    // Create font texture
//    unsigned char* fontData{ nullptr };
//    
//    int texWidth{ 0 }, texHeight{ 0 };
//    ImGuiIO& io = ImGui::GetIO();
//    io.Fonts->GetTexDataAsAlpha8(&fontData, &texWidth, &texHeight);
//    
//    mFontTexture = std::make_unique<Image>(texWidth, texHeight, Format::R8, fontData);
//    
//    auto& renderer = Renderer::RendererLocator::GetRenderer();
//    
//    mUniformBuffer.offset = 0;
//    mUniformBuffer.dataSize = 2 * sizeof(float) + 4 * sizeof(float);
//    
//    BufferDesc uboDesc;
//    uboDesc.bufferSize = mUniformBuffer.dataSize;
//    uboDesc.usage = BufferUsage::UniformBuffer;
//    uboDesc.memoryUsage = MemoryType(MemoryType::HostVisible | MemoryType::HostCoherent);
//    renderer.CreateBuffer(uboDesc, mUniformBuffer.deviceObject);
//    
//    // Setup stages
//    mGuiPipeline.effect.AddModule(ModuleStage::Vertex, "/Users/tomaskubovcik/Dev/SummitEngine/imgui_vert.spv");
//    mGuiPipeline.effect.AddModule(ModuleStage::Fragment, "/Users/tomaskubovcik/Dev/SummitEngine/imgui_frag.spv");
//    
//    // Setup attributes
//    mGuiPipeline.effect.AddAttribute(Format::R32G32F, 0);
//    mGuiPipeline.effect.AddAttribute(Format::R8G8B8A8, 1);
//    mGuiPipeline.effect.AddAttribute(Format::R32G32F, 2);
//    
//    // Setup uniforms
//    mGuiPipeline.effect.AddConstantRange(ModuleStage::Vertex, 0, 2 * sizeof(Vector2f));
//    mGuiPipeline.effect.AddTexture(ModuleStage::Fragment, 1, *mFontTexture.get());
//    
//    mGuiPipeline.Create();
}

Gui::~Gui()
{
    ImGui::DestroyContext((ImGuiContext*)mGuiContext);
}

void Gui::StartFrame(const FrameData& frameData)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = frameData.width;
    io.DisplaySize.y = frameData.height;
    
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();
}

void Gui::FinishFrame()
{
    // Generate draw buffers
    ImGui::Render();
    
    ImDrawData* imDrawData = ImGui::GetDrawData();
    if (imDrawData->TotalVtxCount == 0 || imDrawData->TotalIdxCount == 0)
        return;
    
    auto& positionStream = mVertexBuffer.GetPositionDataStream();
    auto& texCoordStream = mVertexBuffer.GetTexCoordDataStream();
    auto& colorStream = mVertexBuffer.GetColorDataStream();
    auto& indexStream = mVertexBuffer.GetIndexDataStream();
    
    positionStream.Invalidate();
    texCoordStream.Invalidate();
    colorStream.Invalidate();
    indexStream.Invalidate();
    
    auto& positionData = positionStream.GetData();
    auto& texCoordData = texCoordStream.GetData();
    auto& colorData = colorStream.GetData();
    auto& indexData = indexStream.GetData();
    
    positionData.reserve(imDrawData->TotalVtxCount);
    texCoordData.reserve(imDrawData->TotalVtxCount);
    colorData.reserve(imDrawData->TotalVtxCount);
    indexData.reserve(imDrawData->TotalIdxCount);
    
    for (int32_t i = 0; i < imDrawData->CmdListsCount; ++i)
    {
        const ImDrawList* cmd_list = imDrawData->CmdLists[i];
        const auto vertexCount = cmd_list->VtxBuffer.size();
        const auto indexCount = cmd_list->IdxBuffer.size();
        
        // Copy vertex data
        for (int32_t vertexId{ 0 }; vertexId < vertexCount; ++vertexId)
        {
            const auto& vertexData = cmd_list->VtxBuffer[vertexId];
            positionData.push_back({ vertexData.pos.x, vertexData.pos.y });
            texCoordData.push_back({ vertexData.uv.x, vertexData.uv.y });
            colorData.push_back(vertexData.col);
        }
        
        // Copy index data
        for (int32_t indexId{ 0 }; indexId < indexCount; ++indexId)
        {
            indexData.push_back(cmd_list->IdxBuffer[indexId]);
        }
    }
    
    // Map
    positionStream.Lock(CommitCommand::Map);
    texCoordStream.Lock(CommitCommand::Map);
    colorStream.Lock(CommitCommand::Map);
    indexStream.Lock(CommitCommand::Map);
}

void Gui::OnMouseEvent(Core::MouseEvent& event)
{
    using namespace Core;
    
    const float mousex = float(event.x);
    const float mousey = float(event.y);
    
    ImGuiIO& io = ImGui::GetIO();
    
    switch (event.type)
    {
        case MouseEventType::Move:
        case MouseEventType::LeftDrag:
        case MouseEventType::RightDrag:
            io.MousePos.x = mousex;
            io.MousePos.y = mousey;
            break;
        case MouseEventType::LeftPress:
            io.MouseDown[0] = true;
            break;
        case MouseEventType::LeftRelease:
            io.MouseDown[0] = false;
            break;
            
        default:
            break;
    }
}
