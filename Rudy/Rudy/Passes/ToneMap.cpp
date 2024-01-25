#include "RudyPCH.h"   

#include "ToneMap.h"

namespace  Rudy
{

	ToneMap::ToneMap(
		uint32_t m_width, uint32_t m_height,
		std::unordered_map< TexType, Ref<Texture>>& m_ToneMapInputs,
		std::unordered_map< TexType, Ref<Texture>>& m_ToneMapOutputs)
		:m_width(m_width), m_height(m_height),
		m_ToneMapInputs(m_ToneMapInputs), m_ToneMapOutputs(m_ToneMapOutputs)
	{
		Init();
	}


	void ToneMap::Init()
	{
		m_ToneMapShader = Shader::CreateComputeShader("ToneMap Shader",
						"Shaders/PostProcess/toneMapping_CS.glsl");
	
	
		//in case it's global computation,  use an extra texture;
		m_ToneMapScreenTexture = Texture2D::CreateEmpty( 
		 			TextureSpec{ m_width, m_height,
						TextureInternalFormat::RGBA32F,
						false, WrapMode::ClampToBorder, FilterMode::Linear });
	
	    m_ToneMapOutputs[TexType::ScreenTexture] = m_ToneMapScreenTexture;
	
	}

	void ToneMap::Render(Ref<Camera> main_camera)
	{
		m_ToneMapShader->Bind();

		m_ToneMapShader->SetFloat("u_exposure", exposure); 

		glBindImageTexture(0, m_ToneMapInputs[TexType::ScreenTexture]->GetID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

		glBindImageTexture(1, m_ToneMapOutputs[TexType::ScreenTexture]->GetID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	

		glDispatchCompute(std::ceil(m_width / 8), std::ceil(m_height / 8), 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		m_ToneMapShader->Unbind(); 

	}


}
