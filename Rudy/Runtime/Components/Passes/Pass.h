#pragma once
#include "RudyPCH.h"

#include "Runtime/Components/Renderer/Camera.h"
#include "Runtime/Components/Renderer/Shader.h"
#include "Runtime/Components/Renderer/Material.h"
#include "Runtime/Components/Renderer/Framebuffer.h"
#include "Runtime/Components/Primitives/ScreenQuad.h"

namespace Rudy
{

class Pass
{
public:
    virtual ~Pass() = default;
    Pass()          = default;

    virtual void Render(SharedPtr<Camera>) = 0;
};

} // namespace Rudy