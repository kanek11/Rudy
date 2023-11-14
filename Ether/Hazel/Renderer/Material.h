#pragma once

#include "EtherPCH.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Texture.h"

//turns out if shader and texture is ready,  the concept of material should be independent of renderer API.


//me:
//the pipeline at its core has 3 parts: 
//program (shader),  
//input (vertex buffer),
//output (frame buffer).
//dependencies variables from the host.
// 
// 
//material is a wrapper of textures or uniform surface parameters, for the shader to use.
//material is one of the key features in renderer.
//we will update the material system for sure, as we learn more.

//the shader's uniform is set upon creation.
//Bind() is to activate the shader, and activate the uniform values/textures.  used in loop.

// a texture has at least :  to work with shader.
// 1. texture id on GPU  , an attribute of texture class.
// 2. texture type,  for shading algorithm.
// 3. texture slot,  for the shader to sample from.  eg: texture(0, uv); 

//the later two should be material's responsibility.   
//the texture type is exposed,  the slot is simply the array index.

//refer to unity, a fixed material pipeline defines a "workflowMode"  
//where the shaders and sources name are predefined.

//we first implement these two fixed workflow mode,
//old-fashined blinn-phong 
//Metallic or specualr for  modern PBR  


//for learning purpose, there should also be certain customizability.
//to define the uniform values for the shader;  the name, the slot.

//also the shader itself , eg: Get the textures.size()  and add a new slot.

 
 

//besides writing shader directly,  
//material editor or shader graph using GUI based on graph, 
// we might try that as we become more familiar with GUI ; 

//assume the channel sources are all from textures;






//tiple translation units.

//since the textures may come in in any order, we need to define the texture type.
//to set its slot according to the type,  
//without ugly switch case,  the solution here is simply to use a "mirror" array of strings.

	//texture type is outer attribute of texture. so define in material class instead of texture itself.
	//although tempting, we don't use the enum number as the texture channel number. it's too rigid.

	//potential bug:  for same shader,  make sure the order of texture type is the same as the order of texture slot.
	//define a mirror map of texture type,  to get the string name of the texture type.
	//the string is also used to set the texture slot in shader.
	//inline since C++17,  the compiler will treat it as single even included in mul


namespace Hazel { 
	
	enum class TextureType
	{

		ScreenQuad = 0,
		//blinn-phong
		//DiffuseMap, //as basecolor
	
		//universal:
		AlbedoMap,
		SpecularMap,   //considered specular color ; can be omit if using metallic workflow

		NormalMap, 
		 
		RoughnessMap,   //or glossiness; 

		//pbr
		MetallicMap,

		AOMap,  //ambient occlusion 

		Skybox,  //cubemap 
		EnvironmentMap,  //cubemap

		DepthMap, //shadowmap
		//HeightMap, //displacement
		//MaskMap,
		//LightMap,   
		//

		//for gbuffer geometry pass
		gPosition,   
		gAlbedo,     
		gWorldNormal,  
		gWorldTangent,
		gSpecular,   
		gMetallic,   
		gRoughness,  
		gScreenDepth,


		//lightingPass output 
		lightingPassTexture,


		//IBL
		diffuseEnvMap,  //cubemap
		specularEnvMap,  //cubemap
		brdfLUT,  //2D

		//SSAO
		NoiseTexture,  //2D

	};

	 
	inline std::unordered_map<TextureType, std::string> TextureTypeName =
	{
		{TextureType::ScreenQuad, "u_ScreenQuad"},
		//{TextureType::DiffuseMap, "u_DiffuseMap"},

		{TextureType::AlbedoMap, "u_AlbedoMap"},

		{TextureType::SpecularMap, "u_SpecularMap"},
		{TextureType::NormalMap, "u_NormalMap"},
		{TextureType::RoughnessMap, "u_RoughnessMap"},

		{TextureType::MetallicMap, "u_MetallicMap"},
		{TextureType::AOMap, "u_AOMap"},
		//{TextureType::HeightMap, "u_HeightMap"},
		//{TextureType::MaskMap, "u_MaskMap"},
		{TextureType::Skybox, "u_Skybox"},
		{TextureType::EnvironmentMap, "u_EnvironmentMap"},


		{TextureType::gPosition,    "gPosition"},
		{TextureType::gAlbedo,      "gAlbedo"},
		{TextureType::gWorldNormal, "gWorldNormal"},
		{TextureType::gWorldTangent,"gWorldTangent"},
	 	{TextureType::gSpecular,    "gSpecular"},
		{TextureType::gMetallic,    "gMetallic"},
		{TextureType::gRoughness,   "gRoughness"},
		{TextureType::gScreenDepth, "gScreenDepth"},

		{TextureType::lightingPassTexture, "u_LightingPassTexture"},

		{TextureType::diffuseEnvMap, "u_DiffuseEnvMap"},
		{TextureType::specularEnvMap, "u_SpecularEnvMap"},
		{TextureType::brdfLUT, "u_BrdfLUT"},

		{TextureType::NoiseTexture, "u_NoiseTexture"},
		 
	};



		

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

	 
	 
	class Material
	{
	public : 
		~Material() = default;
		Material() = default; 
		//Material(MaterialType type);
		Material(Ref<Shader> shader);

		void Bind();   //before draw call, bind the material/textures
		void Unbind();  
	    

	
	    Ref<Shader> GetShader() const { return m_Shader; }
		void SetShader(Ref<Shader> shader) { m_Shader = shader; } 

		//MaterialType GetMaterialType() const { return materialType; }
		//void SetMaterialType(MaterialType type) { materialType = type; }
		 
		void SetTexture(TextureType type, Ref<Texture> texture)  { m_Textures[type] = texture;}
		std::unordered_map<TextureType, Ref<Texture>> GetTextures() const { return m_Textures; }

	 

		//material and shader works so closely here;
		static void SetMaterialProperties(Ref<Shader> shader);

		//default shader
		static Ref<Material> Create(Ref<Shader> shader =
			Shader::Create("Basic Shader", "Resources/Shaders/Basic_VS.glsl", "Resources/Shaders/Basic_FS.glsl"));

	

		//static Ref<Material> Create(MaterialType = MaterialType::Basic);
		//static void SetupMaterial();  //set up uniform variables in shader. etc.

		 

	private:
		Ref<Shader> m_Shader;
		//collection of textures
		std::unordered_map<TextureType, Ref<Texture>> m_Textures; 


		//MaterialType materialType;


	};


}



