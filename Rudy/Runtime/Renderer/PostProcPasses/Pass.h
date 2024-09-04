#pragma once
#include "RudyPCH.h"

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Material.h"
#include "Runtime/Renderer/Framebuffer.h"
#include "Runtime/Actors/Primitives/ScreenQuad.h"

namespace Rudy
{

class Pass
{
public:
    virtual ~Pass() = default;
    Pass()          = default;
};

} // namespace Rudy