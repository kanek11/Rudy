#pragma once

#include "RudyPCH.h"

#include "Runtime/Components/Renderer/Renderer.h"
#include "Runtime/Components/Renderer/Mesh.h"
#include "Runtime/Components/Renderer/Material.h"
#include "Runtime/Components/Renderer/Object.h"

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