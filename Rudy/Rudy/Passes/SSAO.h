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
			std::map< TexType, Ref<Texture2D>>& m_SSAOInputs,
			std::map< TexType, Ref<Texture2D>>& m_SSAOOutputs
			); 

		virtual void Render(Ref<Camera>) override; 


	   void Init();


	    

	public:

		uint32_t m_width, m_height;
		std::map< TexType, Ref<Texture2D>>& m_SSAOInputs;
		std::map< TexType, Ref<Texture2D>>& m_SSAOOutputs; 



		Ref<FrameBuffer>  SSAOFBO;
		Ref<Shader> SSAOShader;
		Ref<Material> SSAOMaterial;

		Ref<ScreenQuad> SSAOQuad;

		Ref<Texture2D> SSAOOnlyTex;
		Ref<Texture2D> SSAOOutputTexture;



		float radius = 0.5f;
		float bias = 0.01; 
		float range_bias = 0.4;
		
		int kernelSize = 64;


		Ref<Shader> BlurShader;


		bool enableBlur = false;
		//blur
		//float sigma = 7.0;
		//int radius = 5;

		int ave_radius = 4;



	};
}