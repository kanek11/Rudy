#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Transform.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Camera.h"
#include "Rudy/Renderer/Buffer.h"

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

    virtual void Draw(Ref<Camera> cam, uint32_t count = 1, Ref<Material> mat = nullptr) = 0;

    // set/get
    void          SetMaterial(Ref<Material> mat) { m_material = mat; }
    Ref<Material> GetMaterial() { return m_material; }

    void SetShader(Ref<Shader> shader) { m_material->SetShader(shader); }
    void GetShader() { m_material->GetShader(); }

    void             SetVertexArray(Ref<VertexArray> va) { m_vertexArray = va; }
    Ref<VertexArray> GetVertexArray() { return m_vertexArray; }

    void SetSSBO(uint32_t bindingPoint, Ref<StorageBuffer> SSBO)
    {
        m_StorageBuffer_map[bindingPoint] = SSBO;
    }

    // bool
    bool hasMaterial() { return m_material != nullptr; }
    bool hasVertexArray() { return m_vertexArray != nullptr; }

protected:
    Ref<Material> m_material = nullptr;
    // optional
    Ref<VertexArray> m_vertexArray = nullptr;

    uint32_t m_instanceCount = 1;

    // new: <binding point, SSBO>, mainly for compute shader
    std::unordered_map<uint32_t, Ref<StorageBuffer>> m_StorageBuffer_map;

public:
    // hack:
    void           SetTransform(Ref<Transform> t) { m_transform = t; }
    Ref<Transform> m_transform = nullptr;
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

    static Ref<StaticMeshRenderer> Create()
    {
        return CreateRef<StaticMeshRenderer>();
    }

public:
    virtual void Draw(Ref<Camera> cam, uint32_t count = 1, Ref<Material> mat = nullptr) override;

    void      SetMesh(Ref<Mesh> m);
    Ref<Mesh> GetMesh() { return m_mesh; }
    bool      hasMesh() { return m_mesh != nullptr; }

    // system utilities
private:
    void SetupBuffers();

public:
    Ref<Mesh> m_mesh = nullptr;

    Ref<VertexBuffer> m_vertexBuffer = nullptr;
    Ref<IndexBuffer>  m_indexBuffer  = nullptr;
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

//	static Ref<PhysicalMeshRendererComponent> Create()
//	{
//		return CreateRef<PhysicalMeshRendererComponent>();
//	}

//	virtual void Draw(Ref<Camera> cam) override;

// public:
//	void SetMesh(Ref<Mesh> m);
//	Ref<Mesh> GetMesh() { return m_mesh; }
//	bool hasMesh() { return m_mesh != nullptr; }

// public:
//	Ref<Mesh> m_mesh = nullptr;
// };

class ParticleSpriteRenderer : public Renderer
{
public:
    ~ParticleSpriteRenderer() = default;
    ParticleSpriteRenderer()
    {
        RD_CORE_INFO(" ParticleSpriteRendererComponent created:");
    }

    static Ref<ParticleSpriteRenderer> Create()
    {
        return CreateRef<ParticleSpriteRenderer>();
    }

public:
    virtual void Draw(Ref<Camera> cam, uint32_t count = 1, Ref<Material> mat = nullptr) override;
};

} // namespace Rudy