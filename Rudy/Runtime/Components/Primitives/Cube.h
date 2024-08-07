#pragma once

#include "RudyPCH.h"

#include "Runtime/Components/Renderer/Object.h"

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

    SharedPtr<Mesh> CreateMeshGeometry();

    void DrawSkybox(SharedPtr<Camera> cam); // need to set special view matrix, and gldepthfunc;

    static SharedPtr<Cube> Create()
    {
        auto object = CreateShared<Cube>();

        object->StaticMeshObject::InitComponent(object);
        // object->InitComponent(object);

        object->GetRenderer()->SetMesh(object->m_mesh);
        return object;
    }
};

} // namespace Rudy