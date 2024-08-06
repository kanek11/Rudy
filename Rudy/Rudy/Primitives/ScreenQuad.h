#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Renderer.h"
#include "Rudy/Renderer/Mesh.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Object.h"

namespace Rudy
{

class ScreenQuad : public StaticMeshObject
{
public:
    ~ScreenQuad() = default;
    ScreenQuad();

    static SharedPtr<ScreenQuad> Create();

    SharedPtr<Mesh> CreateMeshGeometry();
};

} // namespace Rudy