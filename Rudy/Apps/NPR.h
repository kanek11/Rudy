#include "RudyPCH.h"

#include <Rudy.h>

#include "Vendor/imgui/imgui.h"
#include "Vendor/imgui/imgui_impl_glfw.h"
#include "Vendor/imgui/imgui_impl_opengl3.h"


#include "Application.h"


namespace Rudy
{

	inline std::unordered_map<std::string, glm::vec3> NPRDefaultVec3Map
	{
		{"u_Albedo", glm::vec3(1.0,1.0,1.0)},
			//{"u_litColor", glm::vec3(1.0, 1.0, 1.0)},
	//{ "u_shadowColor", glm::vec3(1.0,1.0,1.0) },
	};

	inline std::unordered_map<std::string, float> NPRDefaultFloatMap
	{
		//{"u_Metallic",    1.0f},
		//{"u_Roughness",   1.0f},
		//{"u_Specular",    1.0f},

		//intensity for techs
		//{"u_NormalScale", 1.0f},
		//{"u_AO",          1.0f},
	};


	inline std::unordered_map<std::string, bool> NPRDefaultBoolMap
	{
		{"Use_u_AlbedoMap", false},
		{"Use_u_NormalMap", false},


		{"u_face", false},
		{"u_skipNormal" ,false},
		{"u_receive_shadow", true},
	};


	class NPRMaterial : public Material
	{
	public:
		~NPRMaterial() = default;
		NPRMaterial() = default;

		//set preset values
		NPRMaterial(Ref<Shader> shader, const std::string& name) : Material(shader, name)
		{
			this->SetFloatMap(NPRDefaultFloatMap);
			this->SetVec3Map(NPRDefaultVec3Map);
			this->SetBoolMap(NPRDefaultBoolMap);
		}

		static Ref<NPRMaterial> Create(Ref<Shader> shader = nullptr, const std::string& name = "UnnamedNPRMaterial")
		{
			return CreateRef<NPRMaterial>(shader, name);
		}

	};  







	class NPR : public Application
	{
	public:
		~NPR() = default;
		NPR();
		static Ref<NPR> Create();

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
		std::vector< Ref<StaticMeshObject> >  staticMeshObjects;
		std::vector< Ref<Model> >  models;

		 

		//float shadow_bias = 0.005f;
		float min_shadow_bias = 0.001f;
		float max_shadow_bias = 0.01f;

 


		//==========================================
		//Lit
		Ref<FrameBuffer> litPassFBO;
		Ref<Shader> litPassShader;


		// 
		// 
		//Ref<Pass> LitPass;
		std::map< TexType, Ref<Texture2D>> litInputs;
		std::map< TexType, Ref<Texture2D>> litOutputs;



		//=======================================================================================================
		//SSAO
		Ref<SSAO> SSAOPass;

		std::unordered_map< TexType, Ref<Texture>> SSAOInputs;
		std::unordered_map< TexType, Ref<Texture>> SSAOOutputs;

		//=======================================================================================================
		//Bloom

		Ref<Bloom> BloomPass;

		std::unordered_map< TexType, Ref<Texture >> BloomInputs;
		std::unordered_map< TexType, Ref<Texture >> BloomOutputs;



		//=======================================================================================================
		//Outline

		Ref<Outline> OutlinePass;

		std::unordered_map< TexType, Ref<Texture >> OutlineInputs;
		std::unordered_map< TexType, Ref<Texture >> OutlineOutputs;


		//=======================================================================================================
		//SSR
		Ref<SSR> SSRPass;

		std::unordered_map< TexType, Ref<Texture >> SSRInputs;
		std::unordered_map< TexType, Ref<Texture >> SSROutputs;


		//tonemapping
		Ref<ToneMap> ToneMapPass;

		std::unordered_map< TexType, Ref<Texture >> ToneMapInputs;
		std::unordered_map< TexType, Ref<Texture >> ToneMapOutputs;


		Ref<Composer> ComposerPass;

		std::unordered_map< TexType, Ref<Texture >> ComposerInputs;
		std::unordered_map< TexType, Ref<Texture >> ComposerOutputs;



	public:

		//2560:1440 = 16:9
		const uint32_t SCR_WIDTH = 2560;
		const uint32_t SCR_HEIGHT = 1440;

		const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
		const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
		const uint32_t SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

		const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 1.5f, 5.0f); 


	public: 
		bool  enableSSAO = false;

		bool  enableSSR = false;
		bool  enableOutline = true;
		bool  enableBloom = true;

		bool  enableComposer = true;
		bool  enableToneMap = true; 

		bool  visualize_gbuffer = false;
		bool  enableSkyBox = false;

		Ref<Texture> visualizeBuffer;
		float bufferMipLevel = 0;



		Ref<Texture2D> shadowMap;  


		float direct_light_intensity = 1.0f; 
		float ambient_coeff = 0.2f;


		glm::vec3 litColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 shadowColor = glm::vec3(0.88f, 0.73f, 0.70f);


		float diffuse_cutoff = 0.3f;

	};








}