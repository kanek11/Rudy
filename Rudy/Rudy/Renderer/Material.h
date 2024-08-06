#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Shader.h"
#include "Rudy/Renderer/Texture.h"
#include "Rudy/Renderer/Buffer.h"
// turns out if shader and texture is ready,  the concept of material should be independent of renderer API.

// doc
namespace Rudy
{

// conventions:
// textures used as varying property on geometry surface is named []Map,
// and has a variable version ;
// textures for utilities is named []Texture,
// including output of a quad, noise, etc.

enum class TexType
{

    ScreenTexture = 0, // for screen quad

    // named []Map, that
    // DiffuseMap, //replaced as basecolor
    // universal:
    AlbedoMap,
    SpecularMap, // considered specular color ; can be omit if using metallic workflow
    NormalMap,
    RoughnessMap, // or glossiness;
    MetallicMap,
    AOMap,        // ambient occlusion
    DepthTexture, // for shadowmap
    // HeightMap, //displacement
    // MaskMap,   //for masking
    // LightMap,    //global illumination

    // for gbuffer geometry pass named g[]
    gWorldPosition,
    gWorldNormal,
    gWorldTangent,
    gAlbedo,
    gSpecular,
    gMetallic,
    gRoughness,
    gViewDepth,

    gViewPosition,
    gViewNormal,

    // pass outputs named "[]Texture"
    // litPass output
    litPassTexture,
    SkyboxTexture, // cubemap

    // custom texture,utilities  named "[]Texture"
    NoiseTexture, // 2D

    // IBL
    EnvironmentMap, // cubemap
    diffuseEnvMap,  // cubemap
    specularEnvMap, // cubemap
    brdfLUTTexture, // 2D

    // NPR
    ToonTexture,
    FaceLightMap,

    // new postprocessing
    SSAOOutputTexture,
    SSROutputTexture,
    BloomOutputTexture,
    OutlineOutputTexture,

};

// this map mirrors the shaders.
// to set slots, to default values .
// leverage the fact that shader differentiate names by type.
inline std::unordered_map<TexType, std::string> TexTypeNames = {
    { TexType::ScreenTexture, "u_ScreenTexture" },
    //{TexType::DiffuseMap, "u_DiffuseMap"},

    { TexType::AlbedoMap, "u_AlbedoMap" },
    { TexType::SpecularMap, "u_SpecularMap" },
    { TexType::NormalMap, "u_NormalMap" },
    { TexType::RoughnessMap, "u_RoughnessMap" },
    { TexType::MetallicMap, "u_MetallicMap" },
    { TexType::AOMap, "u_AOMap" },
    { TexType::EnvironmentMap, "u_EnvironmentMap" },

    { TexType::DepthTexture, "u_DepthTexture" },

    { TexType::gWorldPosition, "gWorldPosition" },
    { TexType::gAlbedo, "gAlbedo" },
    { TexType::gWorldNormal, "gWorldNormal" },
    { TexType::gWorldTangent, "gWorldTangent" },
    { TexType::gSpecular, "gSpecular" },
    { TexType::gMetallic, "gMetallic" },
    { TexType::gRoughness, "gRoughness" },

    { TexType::gViewPosition, "gViewPosition" },
    { TexType::gViewNormal, "gViewNormal" },

    { TexType::gViewDepth, "gViewDepth" },

    { TexType::litPassTexture, "u_litPassTexture" },
    { TexType::SkyboxTexture, "u_SkyboxTexture" },

    { TexType::diffuseEnvMap, "u_DiffuseEnvMap" },
    { TexType::specularEnvMap, "u_SpecularEnvMap" },
    { TexType::brdfLUTTexture, "u_brdfLUTTexture" },

    { TexType::NoiseTexture, "u_NoiseTexture" },

    { TexType::ToonTexture, "u_ToonTexture" },
    { TexType::FaceLightMap, "u_FaceLightMap" },

};

class Material
{
public:
    ~Material() = default;
    Material()
    {
        // m_Shader = ShaderLibrary::Get("Basic");
    }
    Material(SharedPtr<Shader> shader, const std::string& name);

    static SharedPtr<Material> Create(SharedPtr<Shader> shader = nullptr, const std::string& name = "UnnamedMaterial");

    // getters and setters
    void              SetShader(SharedPtr<Shader> shader) { m_Shader = shader; }
    SharedPtr<Shader> GetShader() const { return m_Shader; }

    void SetTexture(TexType type, SharedPtr<Texture> texture)
    {
        if (texture == nullptr)
        {
            RD_CORE_ERROR("Material::Texture is null");
            return; // if texture is null, do nothing
        }

        m_Texture_map[type] = texture;

        // set uniform bool used for texture, if exists
        std::string BoolName = "Use_" + TexTypeNames[type];
        auto        ite      = m_Bool_map.find(BoolName);
        if (ite != m_Bool_map.end())
        {
            m_Bool_map[BoolName] = true;
        }
    }

    std::unordered_map<TexType, SharedPtr<Texture>> GetTextures() const { return m_Texture_map; }
    void                                            SetTextures(std::unordered_map<TexType, SharedPtr<Texture>> map)
    {
        m_Texture_map = map;

        // set uniform bool used for texture, if exists
        for (auto& texture : m_Texture_map)
        {
            std::string BoolName = "Use_" + TexTypeNames[texture.first];
            auto        ite      = m_Bool_map.find(BoolName);
            if (ite != m_Bool_map.end())
            {
                m_Bool_map[BoolName] = true;
            }
        }
    }

    void SetFloat(const std::string& name, float value)
    {
        m_Float_map[name] = value;
    }
    void SetFloatMap(const std::unordered_map<std::string, float>& map)
    {
        m_Float_map = map;
    }
    void SetVec3(const std::string& name, const glm::vec3& value)
    {
        m_Vec3_map[name] = value;
    }
    void SetVec3Map(const std::unordered_map<std::string, glm::vec3>& map)
    {
        m_Vec3_map = map;
    }
    void SetBool(const std::string& name, bool value)
    {
        m_Bool_map[name] = value;
    }
    void SetBoolMap(const std::unordered_map<std::string, bool>& map)
    {
        m_Bool_map = map;
    }

    // before draw call, bind the material/textures
    void Bind();
    void Unbind();

    static void SetMaterialProperties(SharedPtr<Shader> shader);

public:
    SharedPtr<Shader> m_Shader      = nullptr;
    std::string       m_Name        = "UnnamedMaterial";
    uint32_t          m_GlobalIndex = 0; // unique id for each material

    // used for binding textures
    std::unordered_map<TexType, SharedPtr<Texture>> m_Texture_map;
    std::unordered_map<std::string, glm::vec3>      m_Vec3_map;
    std::unordered_map<std::string, float>          m_Float_map;
    std::unordered_map<std::string, bool>           m_Bool_map;
};

} // namespace Rudy

// legacy
// enum class MaterialType
//{
//	Basic = 0,   //flat color
//	BlinnPhong = 1,   //blinn-phong
//	Metallic = 2,   //PBR
//
//	ScreenQuad = 3,  //only slot 0;
//
//	Custom = 4,  //user defined shader and textures.
//
// };

// TODO: specify the workflow for model loading, and error checking.
// no fixed workflow,for now.
// std::vector<TexType> MetallicTextures =
//{ TexType::AlbedoMap, TexType::NormalMap, TexType::MetallicMap, TexType::RoughnessMap, TexType::AOMap };
//
// std::vector<TexType> BlinnPhongTextures =
//{ TexType::DiffuseMap, TexType::SpecularMap, TexType::NormalMap };
