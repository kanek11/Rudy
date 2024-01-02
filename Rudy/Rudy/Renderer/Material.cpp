#include "RudyPCH.h" 

#include "Material.h"

//#include "Rudy/Renderer/Renderer.h" 

namespace Rudy 
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
	//	: materialType(type)
	//{
	//	RD_CORE_WARN("Material:  Material is Created without Shader");
	//	//SetupMaterial();
	//}

	Material::Material(Ref<Shader> shader)
		: m_Shader(shader)
	{ 
		RD_CORE_INFO("Material:  Material is Created using shader: {0}", shader->GetName());
        
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
	    for (auto& texture : TextureTypeNames)
	    { 
			 shader->SetInt(texture.second, (int)texture.first); 
	    }
		
		for (auto& value: FloatDefaultValue)
		{
			shader->SetFloat(value.first, value.second);
		}

       for (auto& value : Vec3DefaultValue)
		{
			shader->SetVec3(value.first, value.second);
		}
	
		 
	}
 

	//RD_CORE_INFO("Material: sampler2D {0} is set to slot {1}", texture.second, (int)texture.first);

	//const std::string& textureName = TextureTypeName[texture.first];

	// Check if the uniform name is valid
	//if (textureName.empty()) {
	//	RD_CORE_WARN("setupMaterial: didn't recognize type {0}", static_cast<int>(texture.first));
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
		RD_CORE_ERROR("Material: no shader bound");


		//for unordered map, .first is the key, .second is the value 
		//texture object -> bind 
		//int slot = 0;
		for (auto& texture : m_Texture_map)
		{
			texture.second->Bind((int)texture.first);
			//RD_CORE_INFO("Material: textureid: {0} is bound to shader slot {1}", texture.second->GetTextureID(), (int)texture.first);
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

	}
	 

	void Material::Unbind()
	{ 
		if (m_Shader)
			m_Shader->Unbind();
		else
			RD_CORE_ERROR("Material: no shader bound");

		//int slot = 0;
		for (auto& texture : m_Texture_map)
		{
			texture.second->Unbind((int)texture.first);
		}


	}

 


























}