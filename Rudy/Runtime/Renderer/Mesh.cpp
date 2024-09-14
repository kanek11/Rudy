#include "RudyPCH.h"

#include "Mesh.h"

namespace Rudy
{

std::vector<Vertex> UMesh::AssembleVertices()
{
    std::vector<Vertex> _vertices;
    for (size_t i = 0; i < positions.size(); ++i)
    {
        Vertex _vertex;

        if (UVs.size() > 0)
            _vertex.UV = UVs[i];

        // must have position
        _vertex.Position = positions[i];

        if (normals.size() > 0)
            _vertex.Normal = normals[i];

        if (tangents.size() > 0)
            _vertex.Tangent = tangents[i];

        if (boneWeights.size() > 0)
        {
            auto _indices = glm::ivec4(boneWeights[i].BoneIndex0, boneWeights[i].BoneIndex1, boneWeights[i].BoneIndex2, boneWeights[i].BoneIndex3);

            auto _weights = glm::vec4(boneWeights[i].Weight0, boneWeights[i].Weight1, boneWeights[i].Weight2, boneWeights[i].Weight3);

            _vertex.BoneIndices = _indices;
            _vertex.BoneWeights = _weights;
        }

        _vertices.push_back(_vertex);
    }

    // return std::move(_vertices);
    return _vertices; // RVO is done by compiler
}

void UMesh::InitRenderData()
{
    this->renderData = CreateShared<FStaticMeshRenderData>();
}

void UMesh::SetupRenderData()
{
    // RD_PROFILE_FUNCTION();
    if (renderData)
    {
        std::vector<Vertex> _vertices = this->AssembleVertices();

        // data copy
        renderData->m_vertexBuffer->SetData(_vertices.data(), static_cast<uint32_t>(_vertices.size()) * this->GetVertexSize(), BufferUsage::STATIC_DRAW);
        renderData->m_indexBuffer->SetData(this->indices.data(), static_cast<uint32_t>(this->indices.size()) * sizeof(uint32_t), BufferUsage::STATIC_DRAW);

        // attach to vao
        renderData->m_vertexArray->AttachBuffer(renderData->m_vertexBuffer->GetBufferID(), 0, this->GetVertexSize());
        renderData->m_vertexArray->AttachIndexBuffer(renderData->m_indexBuffer->GetBufferID());
        renderData->m_vertexArray->AddAttribute(0, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Position));
        renderData->m_vertexArray->AddAttribute(1, 0, 2, BufferDataType::FLOAT32, offsetof(Vertex, UV));
        renderData->m_vertexArray->AddAttribute(2, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Normal));
        renderData->m_vertexArray->AddAttribute(3, 0, 3, BufferDataType::FLOAT32, offsetof(Vertex, Tangent));
        renderData->m_vertexArray->AddAttribute(4, 0, 4, BufferDataType::INT32, offsetof(Vertex, BoneIndices));
        renderData->m_vertexArray->AddAttribute(5, 0, 4, BufferDataType::FLOAT32, offsetof(Vertex, BoneWeights));

        // RD_CORE_INFO("MeshObject::SetupBuffers: mesh buffers setup finished");
    }
    else
    {
        RD_CORE_ERROR("MeshObject::SetupBuffers: no render data");
        RD_CORE_ASSERT(renderData == nullptr, "MeshObject::SetupBuffers: no render data");
    }
}
} // namespace Rudy