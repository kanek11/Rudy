#pragma once
#include "RudyPCH.h"

#include "SceneComponent.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Buffer.h"

#include "Apps/Layer.h"

namespace Rudy
{

// for now indrect is discarded, it overcomplicates things.
struct DrawArraysIndirectCommand
{
    uint32_t vertex_count   = 0;
    uint32_t instance_count = 1;
    uint32_t first          = 0;
    uint32_t base_instance  = 0;
};

struct DrawIndexedIndirectCommand
{
    uint32_t index_count    = 0;
    uint32_t instance_count = 1;
    uint32_t first_index    = 0;
    uint32_t base_vertex    = 0;
    uint32_t base_instance  = 0;
};

// for now, it impose a draw function,
// every draw call must use a shader program/ material
// vertex array is somehow not limited to mesh, but also useful for instanced rendering etc

//<<abstract>>
class Renderer
{
public:
    virtual ~Renderer() = default;

    virtual void Draw(SharedPtr<UCameraComponent> cam, uint32_t count = 1, SharedPtr<Material> mat = nullptr) = 0;

    // set/get
    void                SetMaterial(SharedPtr<Material> mat) { m_material = mat; }
    SharedPtr<Material> GetMaterial() { return m_material; }

    void SetShader(SharedPtr<Shader> shader) { m_material->SetShader(shader); }
    void GetShader() { m_material->GetShader(); }

    void                   SetVertexArray(SharedPtr<VertexArray> va) { m_vertexArray = va; }
    SharedPtr<VertexArray> GetVertexArray() { return m_vertexArray; }

    void SetSSBO(uint32_t bindingPoint, SharedPtr<StorageBuffer> SSBO)
    {
        m_StorageBuffer_map[bindingPoint] = SSBO;
    }

    // bool
    bool hasMaterial() { return m_material != nullptr; }
    bool hasVertexArray() { return m_vertexArray != nullptr; }

protected:
    SharedPtr<Material> m_material = nullptr;
    // optional
    SharedPtr<VertexArray> m_vertexArray = nullptr;

    uint32_t m_instanceCount = 1;

    // new: <binding point, SSBO>, mainly for compute shader
    std::unordered_map<uint32_t, SharedPtr<StorageBuffer>> m_StorageBuffer_map;

public:
    // hack:
    void                      SetTransform(SharedPtr<USceneComponent> t) { m_transform = t; }
    SharedPtr<USceneComponent> m_transform = nullptr;
};

//<<terminal>>
class StaticMeshRenderer : public Renderer
{
public:
    ~StaticMeshRenderer() = default;
    StaticMeshRenderer()
    {
        RD_CORE_INFO("MeshRendererComponent created:");
        m_vertexArray  = VertexArray::Create();
        m_vertexBuffer = VertexBuffer::Create();
        m_indexBuffer  = IndexBuffer::Create();
    }

    static SharedPtr<StaticMeshRenderer> Create()
    {
        return CreateShared<StaticMeshRenderer>();
    }

public:
    virtual void Draw(SharedPtr<UCameraComponent> cam, uint32_t count = 1, SharedPtr<Material> mat = nullptr) override;

    void            SetMesh(SharedPtr<Mesh> m);
    SharedPtr<Mesh> GetMesh() { return m_mesh; }
    bool            hasMesh() { return m_mesh != nullptr; }

    // system utilities
private:
    void SetupBuffers();

public:
    SharedPtr<Mesh> m_mesh = nullptr;

    SharedPtr<VertexBuffer> m_vertexBuffer = nullptr;
    SharedPtr<IndexBuffer>  m_indexBuffer  = nullptr;
};

//<<terminal>>
// only supply the mesh , all other things are handled by the class for now;
// class PhysicalMeshRendererComponent : public RendererComponent
//{
// public:
//	~PhysicalMeshRendererComponent() = default;
//	PhysicalMeshRendererComponent()
//	{
//		RD_CORE_INFO("Cloth RendererComponent created:");
//		m_vertexArray = VertexArray::Create();
//	}

//	static SharedPtr<PhysicalMeshRendererComponent> Create()
//	{
//		return CreateShared<PhysicalMeshRendererComponent>();
//	}

//	virtual void Draw(SharedPtr<Camera> cam) override;

// public:
//	void SetMesh(SharedPtr<Mesh> m);
//	SharedPtr<Mesh> GetMesh() { return m_mesh; }
//	bool hasMesh() { return m_mesh != nullptr; }

// public:
//	SharedPtr<Mesh> m_mesh = nullptr;
// };

class ParticleSpriteRenderer : public Renderer
{
public:
    ~ParticleSpriteRenderer() = default;
    ParticleSpriteRenderer()
    {
        RD_CORE_INFO(" ParticleSpriteRendererComponent created:");
    }

    static SharedPtr<ParticleSpriteRenderer> Create()
    {
        return CreateShared<ParticleSpriteRenderer>();
    }

public:
    virtual void Draw(SharedPtr<UCameraComponent> cam, uint32_t count = 1, SharedPtr<Material> mat = nullptr) override;
};

} // namespace Rudy