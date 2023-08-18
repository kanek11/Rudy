#pragma once

#include "Hazel/Renderer/Material.h"

namespace Hazel {


	class OpenGLMaterial: public Material
	{
	public :
		OpenGLMaterial() = default;
		OpenGLMaterial(Ref<Shader> shader, WorkflowMode workflowMode);
		~OpenGLMaterial() = default; 


		virtual void Bind()        override ;
		virtual void Unbind()        override;

		virtual void SetTexture(TextureType type, Ref<Texture2D> texture) override
		{
			m_Textures[type] = texture;
		}

		virtual Ref<Shader> GetShader() const override { return m_Shader; }

	private:
		Ref<Shader> m_Shader;
		WorkflowMode m_WorkflowMode;

		//collection of textures
		std::unordered_map<TextureType, Ref<Texture2D>> m_Textures;

	

		virtual void SetupMaterial() override;
	}; 

}
 
