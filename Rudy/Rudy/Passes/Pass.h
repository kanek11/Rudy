#pragma once
#include "RudyPCH.h"

#include "Rudy/Renderer/Camera.h"
#include "Rudy/Renderer/Shader.h"
#include "Rudy/Renderer/Material.h"
#include "Rudy/Renderer/Framebuffer.h"

#include "Rudy/Primitives/ScreenQuad.h"

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