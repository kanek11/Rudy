#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Object.h"

namespace Rudy
{

//<<terminal>>
class Cube : public StaticMeshObject
{
public:
    ~Cube() = default;

    Cube() :
        StaticMeshObject()
    {
        m_mesh = CreateMeshGeometry();
    }

    Ref<Mesh> CreateMeshGeometry();

    void DrawSkybox(Ref<Camera> cam); // need to set special view matrix, and gldepthfunc;

    static Ref<Cube> Create()
    {
        auto object = CreateRef<Cube>();

        object->StaticMeshObject::InitComponent(object);
        // object->InitComponent(object);

        object->GetRenderer()->SetMesh(object->m_mesh);
        return object;
    }
};

} // namespace Rudy