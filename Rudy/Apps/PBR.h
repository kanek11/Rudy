#pragma once
#include "RudyPCH.h"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw.h"
#include "ThirdParty/imgui/imgui_impl_opengl3.h"

#include "Application.h"

namespace Rudy
{

//==========PBR

inline std::unordered_map<std::string, glm::vec3> PBRDefaultVec3Map {
    { "u_Albedo", glm::vec3(1.0, 1.0, 1.0) },
};

inline std::unordered_map<std::string, float> PBRDefaultFloatMap {
    { "u_Metallic", 1.0f },
    { "u_Roughness", 1.0f },
    { "u_Specular", 1.0f },

    // intensity for techs
    //{"u_NormalScale", 1.0f},
    //{"u_AO",          1.0f},
};

inline std::unordered_map<std::string, bool> PBRDefaultBoolMap {
    { "Use_u_AlbedoMap", false },
    { "Use_u_SpecularMap", false },
    { "Use_u_MetallicMap", false },
    { "Use_u_RoughnessMap", false },

    { "Use_u_NormalMap", false },
};

class PBRMaterial : public Material
{
public:
    ~PBRMaterial() = default;
    PBRMaterial()  = default;

    // set preset values
    PBRMaterial(SharedPtr<Shader> shader, const std::string& name) :
        Material(shader, name)
    {
        this->SetFloatMap(PBRDefaultFloatMap);
        this->SetVec3Map(PBRDefaultVec3Map);
        this->SetBoolMap(PBRDefaultBoolMap);
    }

    static SharedPtr<PBRMaterial> Create(SharedPtr<Shader> shader = nullptr, const std::string& name = "UnnamedPBRMaterial")
    {
        return CreateShared<PBRMaterial>(shader, name);
    }
};

class PBR : public ViewportLayer
{
    // app common=======================
public:
    ~PBR() = default;
    PBR();

    void Init() override;
    void ShutDown() override;

    virtual void OnUpdate(float ts) override;
    virtual void OnImGuiRender() override;

    // app specific=======================

    const uint32_t BUFFER_WIDTH  = SCR_WIDTH / 4;
    const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;

    // shadowmap
    const uint32_t SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    SharedPtr<FrameBuffer> shadowMapFBO             = nullptr;
    SharedPtr<Material>    shadowMapMaterial        = nullptr;
    SharedPtr<Material>    shadowMapSkinnedMaterial = nullptr;

    SharedPtr<Texture2D> shadowMap = nullptr;

    // scene
    SharedPtr<DirectionalLight>              sunlight = nullptr;
    std::vector<SharedPtr<StaticMeshObject>> staticMeshObjects;
    std::vector<SharedPtr<Model>>            models;

    SharedPtr<Shader>      gBufferPassShader;
    SharedPtr<FrameBuffer> gBufferFBO;

    // float shadow_bias = 0.005f;
    float min_shadow_bias = 0.001f;
    float max_shadow_bias = 0.01f;

    //
    SharedPtr<Pass>                                 WorldToViewPass;
    std::unordered_map<TexType, SharedPtr<Texture>> WorldToViewInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> WorldToViewOutputs;

    //==========================================
    // Lit pass
    SharedPtr<FrameBuffer> litPassFBO;
    SharedPtr<Shader>      litPassShader;

    SharedPtr<ScreenQuad> litPassQuad;

    // SharedPtr<Pass> LitPass;
    std::map<TexType, SharedPtr<Texture2D>> litInputs;
    std::map<TexType, SharedPtr<Texture2D>> litOutputs;

    //=======================================================================================================
    // SSAO
    SharedPtr<SSAO> SSAOPass;

    std::unordered_map<TexType, SharedPtr<Texture>> SSAOInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> SSAOOutputs;

    //=======================================================================================================
    // Bloom

    SharedPtr<Bloom> BloomPass;

    std::unordered_map<TexType, SharedPtr<Texture>> BloomInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> BloomOutputs;

    //=======================================================================================================
    // Outline

    SharedPtr<Outline> OutlinePass;

    std::unordered_map<TexType, SharedPtr<Texture>> OutlineInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> OutlineOutputs;

    //=======================================================================================================
    // SSR
    SharedPtr<SSR> SSRPass;

    std::unordered_map<TexType, SharedPtr<Texture>> SSRInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> SSROutputs;

    // tonemapping
    SharedPtr<ToneMap> ToneMapPass;

    std::unordered_map<TexType, SharedPtr<Texture>> ToneMapInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> ToneMapOutputs;

public:
    bool enableSSR     = false;
    bool enableOutline = false;
    bool enableBloom   = false;
    bool enableSSAO    = false;

    bool enableToneMap = false;

    bool visualize_gbuffer = true;
    bool enableSkyBox      = true;

    // todo:make these more elegant
    SharedPtr<Shader>     screenQuadShader;
    SharedPtr<ScreenQuad> screenQuad;

    SharedPtr<Texture> visualizeBuffer;
    float              bufferMipLevel = 0;

    SharedPtr<Shader> skyboxShader;
    SharedPtr<Cube>   skybox;

    Navigation*       nav               = nullptr;
    SharedPtr<Shader> pure_color_shader = nullptr;

    float timer = 0.0f;

    void ShowTreeNode(const SharedPtr<Transform> transform);
    void ShowHierarchy();
};

} // namespace Rudy