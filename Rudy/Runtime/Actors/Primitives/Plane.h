#pragma once

#include "RudyPCH.h"

#include "Runtime/Actors/AStaticMeshActor.h"

// a plane , lying on the xz plane , facing up;   i.e.  -1,0,1 -1,0,-1  1,0,-1  1,0,1
// basically a reference plane for the world space;
// if attached material , it will be rendered;  otherwise will in wireframe mode;

// for now, i want the pre-defined model to be self-contained. not relying on object class design.

// all public, free to set;

namespace Rudy
{

class Plane : public AStaticMeshActor
{
public:
    ~Plane() = default;
    Plane(uint32_t subdivison = 1);

private:
    SharedPtr<UMesh> CreateMeshGeometry(uint32_t subdivison);
};

} // namespace Rudy
