#pragma once
#include "RudyPCH.h"   

#include <Rudy.h>

#include "Pass.h"

namespace Rudy
{

	class WorldToView :public Pass
	{
	public:
		virtual ~WorldToView() = default;
		WorldToView(
			uint32_t m_width, uint32_t m_height,
			std::map< TexType, Ref<Texture2D>> m_WorldToViewInputs,
			std::map< TexType, Ref<Texture2D>> m_WorldToViewOutputs
		);

		static Ref<WorldToView>  Create(
			uint32_t m_width, uint32_t m_height,
			std::map< TexType, Ref<Texture2D>> m_WorldToViewInputs,
			std::map< TexType, Ref<Texture2D>> m_WorldToViewOutputs
		);
		 
		virtual void Render(Ref<Camera>) override;


		void Init();




	public:

		uint32_t m_width, m_height;
		std::map< TexType, Ref<Texture2D>> m_Inputs;
		std::map< TexType, Ref<Texture2D>> m_Outputs;
		 
		 
		Ref<Shader> m_WorldToViewShader;  


	};
}