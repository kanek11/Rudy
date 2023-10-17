#include "EtherPCH.h" 

#include "Material.h"

//#include "Hazel/Renderer/Renderer.h" 

namespace Hazel 
{

	 //TODO: built-in material types with shader organization
	//Ref<Material> Material::Create(MaterialType type)
	//{ 
	//	 return CreateRef<Material>(type); 
	//}

	Ref<Material> Material::Create(Ref<Shader> shader)
	{
		return CreateRef<Material>(shader);
	}

	 
	//Material::Material(MaterialType type)
	//	: m_MaterialType(type)
	//{
	//	HZ_CORE_WARN("Material:  Material is Created without Shader");
	//	//SetupMaterial();
	//}

	Material::Material(Ref<Shader> shader)
		: m_Shader(shader)
	{ 
		HZ_CORE_INFO("Material:  Material is Created using shader: {0}", shader->GetName());
 
	}
	 

	void Material::SetMaterialProperties(Ref<Shader> shader)
	{
		// m_MaterialType = MaterialType::Custom;
		//m_Shader = shader;
		//HZ_CORE_INFO("Material: Material setup is called, use shader id: {0}", m_Shader->GetShaderID());
  
	    HZ_ASSERT(shader, "Material:passed shader is null");
	    shader->Bind(); 
		 
	    
	    //loop through all textures and set the slot number;
	    //for unordered map, .first is the key, .second is the value  
	    for (auto& texture : TextureTypeName)
	    {
			//eg: SetInt("u_AlbedoMap", 0);
			 shader->SetInt(texture.second, (int)texture.first);
	    	
	    }
		 
	}
 

	//HZ_CORE_INFO("Material: sampler2D {0} is set to slot {1}", texture.second, (int)texture.first);

	//const std::string& textureName = TextureTypeName[texture.first];

	// Check if the uniform name is valid
	//if (textureName.empty()) {
	//	HZ_CORE_WARN("setupMaterial: didn't recognize type {0}", static_cast<int>(texture.first));
	//}
	//else

	 

	//void Material::SetupMaterial()
	//{ 
	//}


	void Material::Bind()
	{ 
		if (m_Shader)
		m_Shader->Bind(); 
		else
		HZ_CORE_ERROR("Material: no shader bound");


		//for unordered map, .first is the key, .second is the value 
		//texture object -> bind 
		//int slot = 0;
		for (auto& texture : m_Textures)
		{
			texture.second->Bind((int)texture.first);
			//HZ_CORE_INFO("Material: textureid: {0} is bound to shader slot {1}", texture.second->GetTextureID(), (int)texture.first);
		} 

	}
	 

	void Material::Unbind()
	{ 
		if (m_Shader)
			m_Shader->Unbind();
		else
			HZ_CORE_ERROR("Material: no shader bound");

		//int slot = 0;
		for (auto& texture : m_Textures)
		{
			texture.second->Unbind((int)texture.first);
		}


	}

 


























}