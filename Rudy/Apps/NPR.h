#pragma once
#include "RudyPCH.h"

#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_glfw.h"
#include "ThirdParty/imgui/imgui_impl_opengl3.h"

#include "Application.h"

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

    static SharedPtr<NPRMaterial> Create(SharedPtr<Shader> shader = nullptr, const std::string& name = "UnnamedNPRMaterial")
    {
        return CreateShared<NPRMaterial>(shader, name);
    }

    // set preset values
    NPRMaterial(SharedPtr<Shader> shader, const std::string& name) :
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
    SharedPtr<DirectionalLight>              sunlight = nullptr;
    std::vector<SharedPtr<StaticMeshObject>> staticMeshObjects;
    std::vector<SharedPtr<Model>>            models;

    // float shadow_bias = 0.005f;
    float min_shadow_bias = 0.001f;
    float max_shadow_bias = 0.01f;

    //==========================================
    // Lit
    SharedPtr<FrameBuffer> litPassFBO;
    SharedPtr<Shader>      litPassShader;

    //
    //
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

    SharedPtr<Composer> ComposerPass;

    std::unordered_map<TexType, SharedPtr<Texture>> ComposerInputs;
    std::unordered_map<TexType, SharedPtr<Texture>> ComposerOutputs;

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

    SharedPtr<Texture> visualizeBuffer;
    float              bufferMipLevel = 0;

    SharedPtr<Texture2D> shadowMap;

    float     direct_light_intensity = 0.9f;
    glm::vec3 direct_light_dir       = glm::vec3(0.3f, -0.5f, -1.0f);

    float ambient_coeff = 0.2f;

    glm::vec3 litColor    = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 shadowColor = glm::vec3(0.88f, 0.73f, 0.70f);

    float diffuse_cutoff = 0.3f;

    // todo:make these more elegant
    SharedPtr<Shader>     screenQuadShader;
    SharedPtr<ScreenQuad> screenQuad;

    Navigation*       nav;
    SharedPtr<Shader> pure_color_shader;

    float timer = 0.0f;

    SharedPtr<FrameBuffer> shadowMapFBO             = nullptr;
    SharedPtr<Material>    shadowMapMaterial        = nullptr;
    SharedPtr<Material>    shadowMapSkinnedMaterial = nullptr;

    SharedPtr<Model> model = nullptr;
};

} // namespace Rudy