#pragma once
#include "RudyPCH.h"

#include "Pass.h"

namespace Rudy
{

class Outline : public Pass
{
public:
    virtual ~Outline() = default;
    Outline(
        uint32_t m_width, uint32_t m_height, std::unordered_map<TexType, SharedPtr<UTexture>>& m_OutlineInputs, std::unordered_map<TexType, SharedPtr<UTexture>>& m_OutlineOutputs);

    void Render();

    void Init();

public:
    uint32_t                                          m_width, m_height;
    std::unordered_map<TexType, SharedPtr<UTexture>>& m_OutlineInputs;
    std::unordered_map<TexType, SharedPtr<UTexture>>& m_OutlineOutputs;

    SharedPtr<Shader>   outlineShader;
    SharedPtr<UTexture> outputTexture;

    // parameters
    float depth_thres           = 0.05f;
    float depth_thres_scale     = 10.0f;
    float depth_NdotV_threshold = 0.5f;
    float normal_threshold      = 0.35f;
    float outline_width         = 1.0f;
    bool  enable_normal         = true;
    bool  enable_depth          = true;
};

} // namespace Rudy