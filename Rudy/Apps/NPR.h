#pragma once
#include "RudyPCH.h"

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"

#include "Application.h"

#include "Rudy/Renderer/Light.h"

namespace Rudy
{

inline std::unordered_map<std::string, glm::vec3> NPRDefaultVec3Map {
    { "u_Albedo", glm::vec3(1.0, 1.0, 1.0) },
    //{"u_litColor", glm::vec3(1.0, 1.0, 1.0)},
    //{ "u_shadowColor", glm::vec3(1.0,1.0,1.0) },
};

inline std::unordered_map<std::string, float> NPRDefaultFloatMap {

    // intensity for techs
    //{"u_NormalScale", 1.0f},
    //{"u_AO",          1.0f},
};

inline std::unordered_map<std::string, bool> NPRDefaultBoolMap {
    { "Use_u_AlbedoMap", false },
    { "Use_u_NormalMap", false },
    { "u_face", false },
    { "u_skipNormal", false },
    { "u_receive_shadow", true },
};

class NPRMaterial : public Material
{
public:
    ~NPRMaterial() = default;
    NPRMaterial()  = default;

    static Ref<NPRMaterial> Create(Ref<Shader> shader = nullptr, const std::string& name = "UnnamedNPRMaterial")
    {
        return CreateRef<NPRMaterial>(shader, name);
    }

    // set preset values
    NPRMaterial(Ref<Shader> shader, const std::string& name) :
        Material(shader, name)
    {
        this->SetFloatMap(NPRDefaultFloatMap);
        this->SetVec3Map(NPRDefaultVec3Map);
        this->SetBoolMap(NPRDefaultBoolMap);
    }
};

class NPR : public ViewportLayer
{
public:
    ~NPR() = default;
    NPR();

    void Init() override;
    void ShutDown() override;

    virtual void OnUpdate(float ts) override;
    virtual void OnImGuiRender() override;

    //
    Ref<DirectionalLight>              sunlight = nullptr;
    std::vector<Ref<StaticMeshObject>> staticMeshObjects;
    std::vector<Ref<Model>>            models;

    // float shadow_bias = 0.005f;
    float min_shadow_bias = 0.001f;
    float max_shadow_bias = 0.01f;

    //==========================================
    // Lit
    Ref<FrameBuffer> litPassFBO;
    Ref<Shader>      litPassShader;

    //
    //
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

    Ref<Composer> ComposerPass;

    std::unordered_map<TexType, Ref<Texture>> ComposerInputs;
    std::unordered_map<TexType, Ref<Texture>> ComposerOutputs;

public:
    const uint32_t BUFFER_WIDTH  = SCR_WIDTH / 4;
    const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
    const uint32_t SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

public:
    bool enableSSAO = false;

    bool enableSSR     = false;
    bool enableOutline = true;
    bool enableBloom   = true;

    bool enableComposer = true;
    bool enableToneMap  = true;

    bool visualize_gbuffer = false;
    bool enableSkyBox      = false;

    Ref<Texture> visualizeBuffer;
    float        bufferMipLevel = 0;

    Ref<Texture2D> shadowMap;

    float     direct_light_intensity = 0.9f;
    glm::vec3 direct_light_dir       = glm::vec3(0.3f, -0.5f, -1.0f);

    float ambient_coeff = 0.2f;

    glm::vec3 litColor    = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 shadowColor = glm::vec3(0.88f, 0.73f, 0.70f);

    float diffuse_cutoff = 0.3f;

    // todo:make these more elegant
    Ref<Shader>     screenQuadShader;
    Ref<ScreenQuad> screenQuad;

    Navigation* nav;
    Ref<Shader> pure_color_shader;

    float timer = 0.0f;

    Ref<FrameBuffer> shadowMapFBO             = nullptr;
    Ref<Material>    shadowMapMaterial        = nullptr;
    Ref<Material>    shadowMapSkinnedMaterial = nullptr;

    Ref<Model> model = nullptr;
};

} // namespace Rudy