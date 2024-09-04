#pragma once
#include "RudyPCH.h"

#include "Runtime/Actors/AStaticMeshActor.h"

// for now, i want the pre-defined model to be self-contained. not relying on object class design.

// all public, free to set;

namespace Rudy
{

class Sphere : public AStaticMeshActor
{
public:
    ~Sphere() = default;
    Sphere(uint32_t subdivision);

    uint32_t m_subdivision = 20;

private:
    SharedPtr<UMesh> CreateMeshGeometry(uint32_t subdivision);
};

} // namespace Rudy
