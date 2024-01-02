#pragma once

#include "RudyPCH.h"

#include "Rudy/Renderer/Shader.h"
#include "Rudy/Renderer/Texture.h"

//turns out if shader and texture is ready,  the concept of material should be independent of renderer API.


//doc


namespace Rudy { 
	
	//conventions:
    //textures used as varying property on geometry surface is named []Map,
	//and has a variable version ;
	//textures used as for utility purpose is named []Texture, 
	//including output of a quad, cubemap, and other custom textures.


	enum class TextureType
	{

		//named []Map, that 
		//DiffuseMap, //replaced as basecolor
		//universal:
		AlbedoMap,
		SpecularMap,   //considered specular color ; can be omit if using metallic workflow
		NormalMap, 
		RoughnessMap,   //or glossiness; 
		MetallicMap, 
		AOMap,  //ambient occlusion 
		DepthMap, //for shadowmap
		//HeightMap, //displacement
		//MaskMap,   //for masking
		//LightMap,    //global illumination

		//for gbuffer geometry pass named g[]
		gPosition,   
		gAlbedo,     
		gWorldNormal,  
		gWorldTangent,
		gSpecular,   
		gMetallic,   
		gRoughness,  
		gScreenDepth,


		//pass outputs named "[]Texture"
		//lightingPass output  
		ScreenQuadTexture,
		lightingPassTexture, 
		SkyboxTexture,  //cubemap 

		//custom texture,utilities  named "[]Texture"
		NoiseTexture,  //2D

		//IBL 
		EnvironmentMap,  //cubemap 
		diffuseEnvMap,  //cubemap
		specularEnvMap,   //cubemap
		brdfLUTMap,  //2D
		 

	};

	 
	//this map mirrors the shaders.  
	//to set slots, to default values .
	//leverage the fact that shader differentiate names by type.
	inline std::unordered_map<TextureType, std::string> TextureTypeNames =
	{
		{TextureType::ScreenQuadTexture, "u_ScreenQuadTexture"},
		//{TextureType::DiffuseMap, "u_DiffuseMap"},

		{TextureType::AlbedoMap, "u_AlbedoMap"}, 
		{TextureType::SpecularMap, "u_SpecularMap"},
		{TextureType::NormalMap, "u_NormalMap"},
		{TextureType::RoughnessMap, "u_RoughnessMap"}, 
		{TextureType::MetallicMap, "u_MetallicMap"},
		{TextureType::AOMap, "u_AOMap"}, 
		{TextureType::EnvironmentMap, "u_EnvironmentMap"},

		{TextureType::DepthMap, "u_DepthMap"},

		{TextureType::gPosition,    "gPosition"},
		{TextureType::gAlbedo,      "gAlbedo"},
		{TextureType::gWorldNormal, "gWorldNormal"},
		{TextureType::gWorldTangent,"gWorldTangent"},
	 	{TextureType::gSpecular,    "gSpecular"},
		{TextureType::gMetallic,    "gMetallic"},
		{TextureType::gRoughness,   "gRoughness"},
		{TextureType::gScreenDepth, "gScreenDepth"},

		{TextureType::lightingPassTexture, "u_LightingPassTexture"}, 
		{TextureType::SkyboxTexture, "u_SkyboxTexture"},


		{TextureType::diffuseEnvMap, "u_DiffuseEnvMap"},
		{TextureType::specularEnvMap, "u_SpecularEnvMap"},
		{TextureType::brdfLUTMap, "u_BrdfLUTMap"},

		{TextureType::NoiseTexture, "u_NoiseTexture"},
		 
	};


	inline std::unordered_map<std::string, float> FloatDefaultValue
	{
	    
	    {"u_Metallic",    1.0f},
	    {"u_Roughness",   1.0f},
		{"u_Specular",    0.0f},

		//intensity for techs
		//{"u_NormalScale", 1.0f},
	    //{"u_AO",          1.0f},
	};

	inline std::unordered_map<std::string, glm::vec3> Vec3DefaultValue
	{
		{"u_Albedo", glm::vec3(1.0,1.0,1.0)},
	};

	inline std::unordered_map<std::string, bool> BoolDefaultValue
	{
		{"Use_u_AlbedoMap", false},
	    {"Use_u_SpecularMap", false},
	    {"Use_u_MetallicMap", false},
	    {"Use_u_RoughnessMap", false},


		{"Use_u_NormalMap", false},
	};
	 
	 
	class Material
	{
	public : 
		~Material() = default;
		Material() = default;  
		Material(Ref<Shader> shader);


	   //getters and setters
		void SetShader(Ref<Shader> shader) { m_Shader = shader; } 
	    Ref<Shader> GetShader() const { return m_Shader; } 

		void SetTexture(TextureType type, Ref<Texture> texture)  { 
			m_Texture_map[type] = texture;  
			//set uniform bool used for texture, if exists
			std::string BoolName = "Use_" + TextureTypeNames[type]; 
			auto ite = m_Bool_map.find(BoolName);
			if (ite != m_Bool_map.end()) {
				m_Bool_map[BoolName] = true;
			} 
           
		}
		std::unordered_map<TextureType, Ref<Texture>> GetTextures() const { return m_Texture_map; }

       void SetFloat(const std::string& name, float value) 
	   {
            m_Float_map[name] = value;
	   } 
       void SetVec3(const std::string& name, const glm::vec3& value)
	   {
		    m_Vec3_map[name] = value;
	   }
	   void SetBool(const std::string& name, bool value)
	   {
		    m_Bool_map[name] = value;
	   }
 

		void Bind();   //before draw call, bind the material/textures
		void Unbind();

		//material and shader works so closely here;
		static void SetMaterialProperties(Ref<Shader> shader);

		//default shader
		static Ref<Material> Create(Ref<Shader> shader =
			Shader::Create("Basic Shader", "Resources/Shaders/Default_VS.glsl", "Resources/Shaders/Default_FS.glsl"));

	

		//static Ref<Material> Create(MaterialType = MaterialType::Basic);
		//static void SetupMaterial();  //set up uniform variables in shader. etc.

		 

	private:
		Ref<Shader> m_Shader;

		//used for binding textures
		std::unordered_map<TextureType, Ref<Texture>> m_Texture_map;
		std::unordered_map<std::string, glm::vec3>  m_Vec3_map = Vec3DefaultValue;
		std::unordered_map<std::string, float>      m_Float_map = FloatDefaultValue;
		std::unordered_map<std::string, bool>       m_Bool_map = BoolDefaultValue;

		 
		//MaterialType materialType;


	};


}







//legacy
//enum class MaterialType
//{
//	Basic = 0,   //flat color
//	BlinnPhong = 1,   //blinn-phong
//	Metallic = 2,   //PBR
//
//	ScreenQuad = 3,  //only slot 0; 
//
//	Custom = 4,  //user defined shader and textures.
//
//};

//TODO: specify the workflow for model loading, and error checking.
//no fixed workflow,for now.
//std::vector<TextureType> MetallicTextures =
//{ TextureType::AlbedoMap, TextureType::NormalMap, TextureType::MetallicMap, TextureType::RoughnessMap, TextureType::AOMap };
//
//std::vector<TextureType> BlinnPhongTextures =
//{ TextureType::DiffuseMap, TextureType::SpecularMap, TextureType::NormalMap };
