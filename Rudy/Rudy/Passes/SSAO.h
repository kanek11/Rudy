#include "RudyPCH.h"   

#include <Rudy.h>

#include "Pass.h"

namespace Rudy
{

	class SSAO :public Pass
	{
	public:
		virtual ~ SSAO() = default;
	    SSAO(
			uint32_t m_width, uint32_t m_height,
			std::map< TextureType, Ref<Texture2D>> m_ssaoInputs,
			std::map< TextureType, Ref<Texture2D>> m_ssaoOutputs
			);

	    static Ref<SSAO> Create(
			uint32_t m_width, uint32_t m_height,
		std::map< TextureType, Ref<Texture2D>> m_ssaoInputs,
		std::map< TextureType, Ref<Texture2D>> m_ssaoOutputs
		);

		virtual void Render(Ref<Camera>) override;



	   void Init();





	public:

		uint32_t m_width, m_height;
		std::map< TextureType, Ref<Texture2D>> m_ssaoInputs;
		std::map< TextureType, Ref<Texture2D>> m_ssaoOutputs; 



		Ref<FrameBuffer>  ssaoFBO;
		Ref<Shader> ssaoShader;

		Ref<ScreenQuad> ssaoQuad;




	};
}