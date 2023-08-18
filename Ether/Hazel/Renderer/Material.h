#pragma once

#include "Ether.h"

#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/Texture.h"


//material must refer to a shader.
//refer to unity, each shader corresponds to a "workflowMode"  

//we first implement these two:
//Specular = old-fashined blinn-phong 
//Metallic = modern PBR


//material is one of the most complicated features in renderer.
//we will redesign the material system for sure, as we learn more.

//besides writing shader directly,  
//material editor or shader graph using GUI based on graph, 
// we might try that as we become more familiar with GUI ;


//assume the channel sources are all from textures;



namespace Hazel {

	//texture type is outer attribute of texture. so define in material class instead of texture itself.
	//although tempting, we don't use the enum number as the texture channel number.
	enum class TextureType
	{
		None = 0,
		//blinn-phong
		DiffuseMap, //or diffuse,basecolor
		SpecularMap,   //smoothness paramter.
		NormalMap, 
		 
		//PBR
		MetallicMap,  //0
		AlbedoMap,
		RoughnessMap,  
	    //normal map; 
		AOMap,  //ambient occlusion

		HeightMap, //displacement
		MaskMap,
	};
	 


	enum class WorkflowMode
	{
		Specular = 0,   //blinn-phong
		Metallic = 1,   //PBR

		FlatColor = 2,  //no texture, just flat color
	};


	class Material
	{
	public :

		~Material() = default; 

		virtual void Bind() = 0;   //before draw call, bind the material/textures
		virtual void Unbind() = 0; 
		virtual void SetupMaterial() = 0;  //set up uniform variables in shader. etc.
	
	
	    virtual Ref<Shader> GetShader() const = 0;

		static Ref<Material> Create(Ref<Shader> shader, WorkflowMode workflowMode = WorkflowMode::Specular);


	private:
		Ref<Shader> m_Shader;
		WorkflowMode m_WorkflowMode;

		//collection of textures
		std::unordered_map<TextureType, Ref<Texture2D>> m_Textures; 

	};


}



