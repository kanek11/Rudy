#pragma once

#include "RudyPCH.h"

#include "Runtime/Actors/AStaticMeshActor.h"

namespace Rudy
{

//<<terminal>>
class Cube : public AStaticMeshActor
{
public:
    ~Cube() = default;
    Cube();

    void DrawSkybox(); // need to set special view matrix, and gldepthfunc;
private:
    SharedPtr<UMesh> CreateMeshGeometry();
};

} // namespace Rudy