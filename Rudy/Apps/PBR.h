#include "RudyPCH.h"

#include <Rudy.h>

#include "Rudy/Vendor/imgui/imgui.h"
#include "Rudy/Vendor/imgui/imgui_impl_glfw.h"
#include "Rudy/Vendor/imgui/imgui_impl_opengl3.h"


#include "Application.h"
#include "Rudy/Passes/SSAO.h"

#include "Rudy/Passes/WorldToView.h"



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


		void InitGUI();
		void ShutDownGUI();
		void PrepareGUI();
		void DrawGUI();
		void RenderGUI();


		//
		Ref<Camera> main_camera;
		Ref<Window> window;




		//
		Ref<Pass> WorldToViewPass;
		std::map< TexType, Ref<Texture2D>> WorldToViewInputs;
		std::map< TexType, Ref<Texture2D>> WorldToViewOutputs;

		//=======================================================================================================
		//SSAO
		Ref<SSAO> SSAOPass;

		std::map< TexType, Ref<Texture2D>> SSAOInputs;
		std::map< TexType, Ref<Texture2D>> SSAOOutputs;




	public:

		//2560:1440 = 16:9
		const uint32_t SCR_WIDTH = 2560;
		const uint32_t SCR_HEIGHT = 1440;

		const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
		const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
		const uint32_t SHADOW_WIDTH = 512, SHADOW_HEIGHT = 512;

		const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 1.5f, 5.0f);



	public:
		bool  enableSSAO = true;
		bool  enableSSR = false;


		bool  visualize_buffer = false;
		bool  enableSkyBox = false;

 

	};




	



}