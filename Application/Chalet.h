#pragma once

#include <Renderer/Object3D.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

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

class Chalet : public Renderer::Object3d
{
public:
    Chalet()
    {
        auto vb = std::make_unique<Renderer::VertexBufferPTCI<Vector3f, Vector2f, Vector3f, uint32_t>>();
        
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "/Users/tomaskubovcik/Dev/SummitEngine/chalet.obj"))
        {
            throw std::runtime_error(warn + err);
        }
        
        auto& positionStream = vb->GetPositionDataStream();
        auto& colorStream = vb->GetColorDataStream();
        auto& indexStream = vb->GetIndexDataStream();
        auto& texCoordStream = vb->GetTexCoordDataStream();
        
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
        
        positionStream.Lock(Renderer::CommitCommand::Commit);
        colorStream.Lock(Renderer::CommitCommand::Commit);
        texCoordStream.Lock(Renderer::CommitCommand::Commit);
        indexStream.Lock(Renderer::CommitCommand::Commit);
        
        mVertexBuffer = std::move(vb);
    }
};
