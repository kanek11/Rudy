#include "RudyPCH.h" 

#include "Material.h"

//#include "Rudy/Renderer/Renderer.h" 

namespace Rudy 
{
 
	Ref<Material> Material::Create(Ref<Shader> shader, const std::string& name)
	{
		return CreateRef<Material>(shader, name);
	} 
 

	Material::Material(Ref<Shader> shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{ 
		if(shader!=nullptr)
		RD_CORE_INFO("Material:  Material:{0} is Created using shader: {1}", m_Name, shader->GetName());
		else
        RD_CORE_WARN("Material:  Material: {0} is Created without Shader", m_Name);
        
	}
	 

	void Material::SetMaterialProperties(Ref<Shader> shader)
	{
		// materialType = MaterialType::Custom;
		//m_Shader = shader;
		//RD_CORE_INFO("Material: Material setup is called, use shader id: {0}", m_Shader->GetShaderID());
  
	    RD_ASSERT(shader, " passed shader is null");
	    shader->Bind();  
	    
	    //loop through all textures and set the slot number;
	    //for unordered map, .first is the key, .second is the value  
		//texture use enum as key, textureName as value, 
	    for (auto& texture : TexTypeNames)
	    { 
			 shader->SetInt(texture.second, (int)texture.first); 
	    }
 
		 
	}
 

	 

	void Material::Bind()
	{ 
		if (m_Shader != nullptr)
		    m_Shader->Bind(); 
		else
		{ 
			RD_CORE_ERROR("Material: no shader bound");
			return;
		}


		//for unordered map, .first is the key, .second is the value 
		//texture object -> bind 
		//int slot = 0;
		for (auto& texture : m_Texture_map)
		{
			texture.second->Bind((int)texture.first);
			//RD_CORE_INFO("Material: textureid: {0} is bound to shader slot {1}", texture.second->GetID(), (int)texture.first);
		} 

         for (auto& value : m_Float_map)
         {
         	m_Shader->SetFloat(value.first, value.second);
         }

         for (auto& value : m_Vec3_map)
		 {
		 	m_Shader->SetVec3(value.first, value.second);
		 }

		 for (auto& value : m_Bool_map)
		 {
		 	m_Shader->SetBool(value.first, value.second);
			//RD_CORE_INFO("Material: bool {0} is set to {1}", value.first, value.second);
		 }


		 for (auto& SSBO : m_StorageBuffer_map)
		 {
		 	SSBO.second->BindBase(SSBO.first);
		 } 

	}
	 

	void Material::Unbind()
	{ 
		if (m_Shader != nullptr)
			m_Shader->Unbind();
		else
		{
			RD_CORE_ERROR("Material: no shader bound");
			return;
		}


		//int slot = 0;
		for (auto& texture : m_Texture_map)
		{
			texture.second->Unbind((int)texture.first);
		}


	}

 


























}