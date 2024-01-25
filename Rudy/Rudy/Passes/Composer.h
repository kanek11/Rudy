#pragma once
#include "RudyPCH.h"    
#include <Rudy.h>

#include "Pass.h"

namespace Rudy
{

	class Composer :public Pass
	{
	public:
		virtual ~Composer() = default;
		Composer(
			uint32_t m_width, uint32_t m_height,
			std::unordered_map< TexType, Ref<Texture>>& m_ComposerInputs,
			std::unordered_map< TexType, Ref<Texture>>& m_ComposerOutputs
		);

		virtual void Render(Ref<Camera>) override;


		void Init();


	public:

		uint32_t m_width, m_height;
		std::unordered_map< TexType, Ref<Texture>>& m_ComposerInputs;
		std::unordered_map< TexType, Ref<Texture>>& m_ComposerOutputs;


		Ref<Shader> ComposerShader; 


		float outline_scale = 0.1f;
		float SSR_scale = 1.0f;

	};

}