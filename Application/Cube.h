#pragma once

#include <Renderer/Object3D.h>

class Cube : public Renderer::Object3d
{
public:
    Cube()
    {
        auto vb = std::make_unique<Renderer::VertexBufferPTCI<Vector3f, Vector2f, Vector3f, uint16_t>>();
        
        auto& positionStream = vb->GetPositionDataStream();
        auto& colorStream = vb->GetColorDataStream();
        auto& indexStream = vb->GetIndexDataStream();
        auto& texCoordStream = vb->GetTexCoordDataStream();
        
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
        
        positionStream.Lock(Renderer::CommitCommand::Commit);
        colorStream.Lock(Renderer::CommitCommand::Commit);
        texCoordStream.Lock(Renderer::CommitCommand::Commit);
        indexStream.Lock(Renderer::CommitCommand::Commit);
        
        mVertexBuffer = std::move(vb);
    }
};
