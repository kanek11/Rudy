#include "EtherPCH.h"

#include "OpenGLMaterial.h"


namespace Hazel {
	OpenGLMaterial::OpenGLMaterial(Ref<Shader> shader, WorkflowMode workflowMode)
		: m_Shader(shader), m_WorkflowMode(workflowMode)
	{
		HZ_CORE_INFO("OpenGLMaterial: Material is Created");
		SetupMaterial();
	}
	  

	void OpenGLMaterial::SetupMaterial()
	{
		//HZ_CORE_INFO("OpenGLMaterial: Material setup is called, use shader id: {0}", m_Shader->GetShaderID());
		m_Shader->Bind();
		
		//set the texture id regardless of existence, make sure the shader handles such case.
		switch (m_WorkflowMode)
		{
		case WorkflowMode::Metallic:
			m_Shader->SetInt("u_AlbedoMap", 0);
			m_Shader->SetInt("u_NormalMap", 1);
			m_Shader->SetInt("u_MetallicMap", 2);
			m_Shader->SetInt("u_RoughnessMap", 3);
			m_Shader->SetInt("u_AOMap", 4);
			break;

		case WorkflowMode::BlinnPhong:
			m_Shader->SetInt("u_DiffuseMap", 0);
			m_Shader->SetInt("u_SpecularMap", 1);
			m_Shader->SetInt("u_NormalMap", 2); 
			break;
		}  
	}


	void OpenGLMaterial::Bind()
	{

		m_Shader->Bind();

		//loop through all textures and bind them
		//for unordered map, .first is the key, .second is the value
		//based on mode
 
		std::vector<TextureType> MetallicTextures =
		{ TextureType::AlbedoMap, TextureType::NormalMap, TextureType::MetallicMap, TextureType::RoughnessMap, TextureType::AOMap };

		std::vector<TextureType> BlinnPhongTextures =
		{ TextureType::DiffuseMap, TextureType::SpecularMap, TextureType::NormalMap };

 
	    // Define the texture order for each workflow mode
	    std::vector<TextureType> textureOrder;
	    if (m_WorkflowMode == WorkflowMode::Metallic)
	    	textureOrder = MetallicTextures;
	    else if (m_WorkflowMode == WorkflowMode::BlinnPhong)
	    	textureOrder =  BlinnPhongTextures;
	    
	    // Bind textures according to the order
	    for (size_t i = 0; i < textureOrder.size(); ++i)
	    {
	    	TextureType type = textureOrder[i];
	    	if (m_Textures[type])    //only when exist,  note this assumes the unordered map return nullptr if not found
	    		m_Textures[type]->Bind(static_cast<int>(i));
	    	//else
	    		//HZ_CORE_INFO("Missing texture of type {0}", static_cast<int>(type)); // Optional warning
	    }
		 


	}



	void OpenGLMaterial::Unbind()
	{

		m_Shader->Unbind();


		std::vector<TextureType> MetallicTextures =
		{ TextureType::AlbedoMap, TextureType::NormalMap, TextureType::MetallicMap, TextureType::RoughnessMap, TextureType::AOMap };

		std::vector<TextureType> BlinnPhongTextures =
		{ TextureType::DiffuseMap, TextureType::SpecularMap, TextureType::NormalMap };

		// Define the texture order for each workflow mode
		std::vector<TextureType> textureOrder;
		if (m_WorkflowMode == WorkflowMode::Metallic)
			textureOrder = MetallicTextures;
		else if (m_WorkflowMode == WorkflowMode::BlinnPhong)
			textureOrder = BlinnPhongTextures;


		// Bind textures according to the order
		for (size_t i = 0; i < textureOrder.size(); ++i)
		{
			TextureType type = textureOrder[i];
			if (m_Textures[type])    //only when exist,  note this assumes the unordered map return nullptr if not found
				m_Textures[type]->Unbind(static_cast<int>(i));
			//else
				//HZ_CORE_INFO("Missing texture of type {0}", static_cast<int>(type)); // Optional warning
		}



     }


}



 