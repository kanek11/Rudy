#include "RudyPCH.h"

#include <Rudy.h>

#include "Application.h"
#include "Rudy/Passes/SSAO.h"



namespace Rudy
{


	class PBR : public Application
	{
	public:
		~PBR() = default;
		PBR();
		static Ref<PBR> Create();

		void Init() override;
		void Start() override; 









		//=======================================================================================================
		//SSAO
		Ref<Pass> ssaoPass;

		std::map< TextureType, Ref<Texture2D>> ssaoInputs;
		std::map< TextureType, Ref<Texture2D>> ssaoOutputs;




	public:
		bool  enableSSAO = true;
		bool  enableSSR = false;


		bool  visualize_buffer = true;
		bool  enableSkyBox = false;

	};













}