#pragma once
#include "RudyPCH.h"    
#include <Rudy.h>

#include "Pass.h"

namespace Rudy
{

	class Bloom :public Pass
	{
	public:
		virtual ~Bloom() = default;
		Bloom(
			uint32_t m_width, uint32_t m_height,
			std::unordered_map< TexType, Ref<Texture>>& m_BloomInputs,
			std::unordered_map< TexType, Ref<Texture>>& m_BloomOutputs
		);

		virtual void Render(Ref<Camera>) override;


		void Init();


	public:

		uint32_t m_width, m_height;
		std::unordered_map< TexType, Ref<Texture>>& m_BloomInputs;
		std::unordered_map< TexType, Ref<Texture>>& m_BloomOutputs; 


		Ref<Shader> upsampleShader;
		Ref<Shader> downsampleShader;
		Ref<Shader> thresholdShader;
		Ref<Shader> addShader;

     	std::vector< Ref<Texture> > RT_Horizonal;
		std::vector< Ref<Texture> > RT_Vertical; 

		Ref<Texture> inputTexture;


		const int nMips = 6;

		//the [0] is not really used, just for convenience of indexing
		const int kernelRadiusArray[6] = { 1,3,5,7,9,11 };

		//to adjust the strength of each mip
		const float bloomFactors[6] = {1.2f, 1.2f, 1.2f, 1.2f, 0.3f, 0.2f};

		//the code is :  so 1.2 =no addition of current level ; large radius lean to lower level;
		//float mirrorFactor = 1.2 - factor;
		//return mix(factor, mirrorFactor, u_bloom_radius);


		float bloom_strength = 0.7f;
		float bloom_radius = 1.0f;
		float threshold = 1.5f;

	};


}