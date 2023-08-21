

#include "OpenGLMaterial.h"


//cpp  
namespace Hazel {
	OpenGLMaterial::OpenGLMaterial(Ref<Shader> shader, WorkflowMode workflowMode)
		: m_Shader(shader), m_WorkflowMode(workflowMode)
	{
		HZ_CORE_INFO("OpenGLMaterial: Material is Created");
		SetupMaterial();
	}


 


	void OpenGLMaterial::SetupMaterial()
	{
		HZ_CORE_INFO("OpenGLMaterial: Material is set up, use shader");
		m_Shader->Bind();

		switch (m_WorkflowMode)
		{
		case WorkflowMode::Metallic:
			m_Shader->SetInt("u_AlbedoMap", 0);
			m_Shader->SetInt("u_NormalMap", 1);
			m_Shader->SetInt("u_MetallicMap", 2);
			m_Shader->SetInt("u_RoughnessMap", 3);
			m_Shader->SetInt("u_AO", 4);
			break;

		case WorkflowMode::Specular:
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
		switch (m_WorkflowMode)
		{
		case WorkflowMode::Metallic:
			m_Textures[TextureType::AlbedoMap]->Bind(0);
			m_Textures[TextureType::NormalMap]->Bind(1);
			m_Textures[TextureType::MetallicMap]->Bind(2);
			m_Textures[TextureType::RoughnessMap]->Bind(3);
			m_Textures[TextureType::AOMap]->Bind(4);
			break;


		case WorkflowMode::Specular:
			m_Textures[TextureType::DiffuseMap]->Bind(0);
			m_Textures[TextureType::SpecularMap]->Bind(1);
			m_Textures[TextureType::NormalMap]->Bind(2);

			break;

		}
	}



	void OpenGLMaterial::Unbind()
	{

		m_Shader->Unbind();

		switch (m_WorkflowMode)
		{
		case WorkflowMode::Metallic:
			m_Textures[TextureType::AlbedoMap]->Unbind(0);
			m_Textures[TextureType::NormalMap]->Unbind(1);
			m_Textures[TextureType::MetallicMap]->Unbind(2);
			m_Textures[TextureType::RoughnessMap]->Unbind(3);
			m_Textures[TextureType::AOMap]->Unbind(4);
			break;


		case WorkflowMode::Specular:
			m_Textures[TextureType::DiffuseMap]->Unbind(0);
			m_Textures[TextureType::SpecularMap]->Unbind(1);
			m_Textures[TextureType::NormalMap]->Unbind(2);

			break;



		}


	}




}

