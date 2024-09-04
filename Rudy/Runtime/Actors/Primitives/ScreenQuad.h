#pragma once

#include "RudyPCH.h"

#include "Runtime/Actors/AStaticMeshActor.h"

namespace Rudy
{

class ScreenQuad : public AStaticMeshActor
{
public:
    ~ScreenQuad() = default;
    ScreenQuad();

    void DrawSelfContained();

private:
    SharedPtr<UMesh> CreateMeshGeometry();
};

} // namespace Rudy