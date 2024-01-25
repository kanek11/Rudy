#pragma once
#include "RudyPCH.h"    
#include <Rudy.h>

#include "Pass.h"

namespace Rudy
{

	class ToneMap :public Pass
	{
	public:
		virtual ~ToneMap() = default;
		ToneMap(
			uint32_t m_width, uint32_t m_height,
			std::unordered_map< TexType, Ref<Texture>>& m_ToneMapInputs,
			std::unordered_map< TexType, Ref<Texture>>& m_ToneMapOutputs
		);

		virtual void Render(Ref<Camera>) override;

		void Init();


	public:

		uint32_t m_width, m_height;
		std::unordered_map< TexType, Ref<Texture>>& m_ToneMapInputs;
		std::unordered_map< TexType, Ref<Texture>>& m_ToneMapOutputs;


		Ref<Shader> m_ToneMapShader;

		Ref<Texture> m_ToneMapScreenTexture; 

		float exposure = 1.0f;

	};


}	