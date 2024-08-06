#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/Object.h"

// for now, i want the pre-defined model to be self-contained. not relying on object class design.

// all public, free to set;

namespace Rudy
{

class Sphere : public StaticMeshObject
{
public:
    ~Sphere() = default;
    Sphere(uint32_t subdivision);

    static SharedPtr<Sphere> Create(uint32_t subdivision);

    SharedPtr<Mesh> CreateMeshGeometry(uint32_t subdivision);

    uint32_t m_subdivision = 20;
};

} // namespace Rudy
