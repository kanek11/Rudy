#pragma once
#include "RudyPCH.h" 
#include <Rudy.h> 

#include "Application.h"


namespace Rudy
{ 

	class Particles : public Application
	{
	public:
		~Particles() = default;
		Particles();
		static Ref<Particles> Create();

		void Init() override ;
		void Start() override ;

		virtual void InitGUI() override {};
		virtual void DrawGUI() override {};

		//
		Ref<Camera> main_camera;
		Ref<Window> window;











		//2560:1440 = 16:9
		const uint32_t SCR_WIDTH = 2560;
		const uint32_t SCR_HEIGHT = 1440;
		const uint32_t BUFFER_WIDTH = SCR_WIDTH / 4;
		const uint32_t BUFFER_HEIGHT = SCR_HEIGHT / 4;
		const uint32_t SHADOW_WIDTH = 2560, SHADOW_HEIGHT = 2560;


		const glm::vec3 MAIN_CAMERA_POS = glm::vec3(0.0f, 2.0f, 5.0f);



	};


}