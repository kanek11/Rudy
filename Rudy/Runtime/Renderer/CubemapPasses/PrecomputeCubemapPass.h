#pragma once

#include "RudyPCH.h"

#include "Runtime/Renderer/Texture.h"

namespace Rudy
{
class PrecomputeCubemapPass
{
public:
    ~PrecomputeCubemapPass() = default;
    PrecomputeCubemapPass() {};

    SharedPtr<UTextureCube> RecToCube(const SharedPtr<UTexture2D>& recTex);

    // SharedPtr<UTextureCube> PrefilterDiffuseEnvMap(const SharedPtr<UTextureCube>& envMap);
    SharedPtr<UTextureCube> SpecularPrefilter(const SharedPtr<UTextureCube>& envMap);

    uint32_t output_width  = 512;
    uint32_t output_height = 512;
};

} // namespace Rudy