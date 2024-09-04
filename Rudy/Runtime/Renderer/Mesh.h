#pragma once
#include "RudyPCH.h"

#include "Runtime/CoreFramework/Object.h"
#include "Runtime/Renderer/Buffer.h"
// todo: validate

// #define MAX_BONE_INFLUENCE 4

namespace Rudy
{

enum class MeshTopology
{
    TRIANGLES, // triangle is default for games;
    POINTS,
    LINES,
    STRIP,
    QUADS,
};

enum class MeshDrawCommand
{
    INDEXED,
    ARRAYS,
};

// standard 4 bones per vertex
// designed to be a effective array ;
// for now it can avoid more nested struct , and low-level vector representation;

// make sure the fields
struct BoneWeight
{
    int BoneIndex0;
    int BoneIndex1;
    int BoneIndex2;
    int BoneIndex3;

    float Weight0;
    float Weight1;
    float Weight2;
    float Weight3;
};

// pre-defined vertex DS , to be used by model loader/creator
// the loader neednot to care the API setup but to fill the data.
struct Vertex
{
    glm::vec3 Position = glm::vec3(0, 0, 0);
    glm::vec2 UV       = glm::vec2(0, 0);
    glm::vec3 Normal   = glm::vec3(0, 0, 0);
    glm::vec3 Tangent  = glm::vec3(0, 0, 0);

    //
    glm::ivec4 BoneIndices = glm::ivec4(-1); // invalid
    glm::vec4  BoneWeights = glm::vec4(0, 0, 0, 0);

    Vertex() = default;
    Vertex(glm::vec3  position,
           glm::vec2  uv,
           glm::vec3  normal,
           glm::vec3  tangent,
           glm::ivec4 boneIndices = glm::ivec4(0, 0, 0, 0),
           glm::vec4  boneWeights = glm::vec4(0, 0, 0, 0)) :
        Position(position),
        Normal(normal), Tangent(tangent), UV(uv),
        BoneIndices(boneIndices), BoneWeights(boneWeights) { }
};

class FMeshRenderData
{
public:
    FMeshRenderData()
    {
        m_vertexArray  = FRHIVertexArray::Create();
        m_vertexBuffer = FRHIVertexBuffer::Create();
        m_indexBuffer  = FRHIIndexBuffer::Create();
    }

    SharedPtr<FRHIVertexArray>  m_vertexArray  = nullptr;
    SharedPtr<FRHIVertexBuffer> m_vertexBuffer = nullptr;
    SharedPtr<FRHIIndexBuffer>  m_indexBuffer  = nullptr;
};

class UMesh : public UObject
{
public:
    ~UMesh() = default;
    UMesh() :
        UObject()
    {
        InitRenderData();
    }

    static SharedPtr<UMesh> Create()
    {
        return NewObject<UMesh>("unnamed mesh");
    }

    // for drawcall.
    uint32_t GetIndexCount()
    {
        if (indices.empty())
            RD_CORE_WARN("Mesh: no indices data");
        return static_cast<uint32_t>(indices.size());
    }
    uint32_t GetVertexSize()
    {
        return sizeof(Vertex);
    }

    uint32_t GetVertexCount()
    {
        if (positions.empty())
            RD_CORE_WARN("Mesh: no data");
        return static_cast<uint32_t>(positions.size());
    }

    std::vector<Vertex> AssembleVertices();

public:
    // interface fields for easy to define;
    // some workflow might prefer each fields to be set individually; instead of a vertex struct;
    // and we should standardize in this way;  not expose the vertex struct to the user;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> UVs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;

    std::vector<BoneWeight> boneWeights;

    // parameters for drawcall;
    MeshTopology    topology    = MeshTopology::TRIANGLES;
    MeshDrawCommand drawCommand = MeshDrawCommand::INDEXED;

    std::vector<uint32_t> indices;

public:
    void                       InitRenderData();
    void                       SetupRenderData();
    SharedPtr<FMeshRenderData> renderData;
};

} // namespace Rudy
