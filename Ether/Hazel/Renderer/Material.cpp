#include "EtherPCH.h" 

#include "Material.h"

//#include "Hazel/Renderer/Renderer.h" 

namespace Hazel 
{

	  
	Ref<Material> Material::Create(MaterialType type)
	{ 
		 return CreateRef<Material>(type); 
	}

	Ref<Material> Material::Create(Ref<Shader> shader)
	{
		return CreateRef<Material>(shader);
	}



	void Material::CreateMaterialType(Ref<Shader> shader)
	{
		// m_MaterialType = MaterialType::Custom;
		//m_Shader = shader;
			//HZ_CORE_INFO("Material: Material setup is called, use shader id: {0}", m_Shader->GetShaderID());
 
	    shader->Bind();
	    //TODO: error check 
	    
	    //loop through all textures and bind them
	    //for unordered map, .first is the key, .second is the value 
	    //int slot = 0;
	    for (auto& texture : TextureTypeName)
	    {
	    	//const std::string& textureName = TextureTypeName[texture.first];
	    
	    	// Check if the uniform name is valid
	    	//if (textureName.empty()) {
	    	//	HZ_CORE_WARN("setupMaterial: didn't recognize type {0}", static_cast<int>(texture.first));
	    	//}
	    	//else
	    	 shader->SetInt(texture.second, (int)texture.first);

	    	//eg: SetInt("u_AlbedoMap", 0);
	     }


	}
 

	 
	Material::Material(MaterialType type)
		:  m_MaterialType(type)
	{ 
		HZ_CORE_INFO("Material: Pre-defined Material is Created");
		//SetupMaterial();
	}

	Material::Material(Ref<Shader> shader)
		: m_Shader(shader)
	{
		//m_MaterialType = MaterialType::Custom;
		HZ_CORE_INFO("Material:  Material is Created using shader: {0}", shader->GetName());
		//SetupMaterial();
	}

	 

	//void Material::SetupMaterial()
	//{ 
	//}


	void Material::Bind()
	{

		m_Shader->Bind();

		//for unordered map, .first is the key, .second is the value 
		//texture object -> bind 
		//int slot = 0;
		for (auto& texture : m_Textures)
		{
			texture.second->Bind((int)texture.first);
		} 

	}
	 

	void Material::Unbind()
	{

		m_Shader->Unbind();

		//int slot = 0;
		for (auto& texture : m_Textures)
		{
			texture.second->Unbind((int)texture.first);
		}


	}

 


























}