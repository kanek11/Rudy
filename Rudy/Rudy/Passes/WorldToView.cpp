#include "RudyPCH.h"   

#include "WorldToView.h"

namespace  Rudy
{

	Ref<WorldToView> WorldToView::Create(
		uint32_t m_width, uint32_t m_height,
		std::map< TexType, Ref<Texture2D>> m_WorldToViewInputs,
		std::map< TexType, Ref<Texture2D>> m_WorldToViewOutputs)
	{
		return CreateRef<WorldToView>(m_width, m_height, m_WorldToViewInputs, m_WorldToViewOutputs);
	}

	WorldToView::WorldToView(
		uint32_t m_width, uint32_t m_height,
		std::map< TexType, Ref<Texture2D>> m_WorldToViewInputs,
		std::map< TexType, Ref<Texture2D>> m_WorldToViewOutputs)
		:m_width(m_width), m_height(m_height),
		m_Inputs(m_WorldToViewInputs), m_Outputs(m_WorldToViewOutputs)
	{
		Init();
	}



	void WorldToView::Init()
	{

		m_WorldToViewShader = Shader::CreateComputeShader("WorldToView Shader",
						"Shaders/PostProcess/WorldToView_CS.glsl");  
		  

	}


	void WorldToView::Render(Ref<Camera> main_camera)
	{


		m_WorldToViewShader->Bind(); 

		m_WorldToViewShader->SetMat4("u_view",  main_camera->GetViewMatrix());

		glBindImageTexture(0, m_Inputs[TexType::gWorldPosition]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(1, m_Inputs[TexType::gWorldNormal]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

		glBindImageTexture(2, m_Outputs[TexType::gViewPosition]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(3, m_Outputs[TexType::gViewNormal]->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


		glDispatchCompute(std::ceil(m_width/8), std::ceil(m_height/ 8), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		m_WorldToViewShader->Unbind();

	}









}