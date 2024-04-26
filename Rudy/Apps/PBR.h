#pragma once
#include "RudyPCH.h"

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"

#include "Application.h"

#include "Rudy/Renderer/Light.h"
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
    PBRMaterial(Ref<Shader> shader, const std::string& name) :
        Material(shader, name)
    {
        this->SetFloatMap(PBRDefaultFloatMap);
        this->SetVec3Map(PBRDefaultVec3Map);
        this->SetBoolMap(PBRDefaultBoolMap);
    }

    static Ref<PBRMaterial> Create(Ref<Shader> shader = nullptr, const std::string& name = "UnnamedPBRMaterial")
    {
        return CreateRef<PBRMaterial>(shader, name);
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

    Ref<FrameBuffer> shadowMapFBO             = nullptr;
    Ref<Material>    shadowMapMaterial        = nullptr;
    Ref<Material>    shadowMapSkinnedMaterial = nullptr;

    Ref<Texture2D> shadowMap = nullptr;

    // scene
    Ref<DirectionalLight>              sunlight = nullptr;
    std::vector<Ref<StaticMeshObject>> staticMeshObjects;
    std::vector<Ref<Model>>            models;

    Ref<Shader>      gBufferPassShader;
    Ref<FrameBuffer> gBufferFBO;

    // float shadow_bias = 0.005f;
    float min_shadow_bias = 0.001f;
    float max_shadow_bias = 0.01f;

    //
    Ref<Pass>                                 WorldToViewPass;
    std::unordered_map<TexType, Ref<Texture>> WorldToViewInputs;
    std::unordered_map<TexType, Ref<Texture>> WorldToViewOutputs;

    //==========================================
    // Lit pass
    Ref<FrameBuffer> litPassFBO;
    Ref<Shader>      litPassShader;

    Ref<ScreenQuad> litPassQuad;

    // Ref<Pass> LitPass;
    std::map<TexType, Ref<Texture2D>> litInputs;
    std::map<TexType, Ref<Texture2D>> litOutputs;

    //=======================================================================================================
    // SSAO
    Ref<SSAO> SSAOPass;

    std::unordered_map<TexType, Ref<Texture>> SSAOInputs;
    std::unordered_map<TexType, Ref<Texture>> SSAOOutputs;

    //=======================================================================================================
    // Bloom

    Ref<Bloom> BloomPass;

    std::unordered_map<TexType, Ref<Texture>> BloomInputs;
    std::unordered_map<TexType, Ref<Texture>> BloomOutputs;

    //=======================================================================================================
    // Outline

    Ref<Outline> OutlinePass;

    std::unordered_map<TexType, Ref<Texture>> OutlineInputs;
    std::unordered_map<TexType, Ref<Texture>> OutlineOutputs;

    //=======================================================================================================
    // SSR
    Ref<SSR> SSRPass;

    std::unordered_map<TexType, Ref<Texture>> SSRInputs;
    std::unordered_map<TexType, Ref<Texture>> SSROutputs;

    // tonemapping
    Ref<ToneMap> ToneMapPass;

    std::unordered_map<TexType, Ref<Texture>> ToneMapInputs;
    std::unordered_map<TexType, Ref<Texture>> ToneMapOutputs;

public:
    bool enableSSR     = false;
    bool enableOutline = false;
    bool enableBloom   = false;
    bool enableSSAO    = false;

    bool enableToneMap = false;

    bool visualize_gbuffer = true;
    bool enableSkyBox      = true;

    // todo:make these more elegant
    Ref<Shader>     screenQuadShader;
    Ref<ScreenQuad> screenQuad;

    Ref<Texture> visualizeBuffer;
    float        bufferMipLevel = 0;

    Ref<Shader> skyboxShader;
    Ref<Cube>   skybox;

    Navigation* nav               = nullptr;
    Ref<Shader> pure_color_shader = nullptr;

    float timer = 0.0f;

    void ShowTreeNode(const Ref<Transform> transform);
    void ShowHierarchy();
};

} // namespace Rudy