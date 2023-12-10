 
#pragma once 
#include "RudyPCH.h"
#include "Object.h"  

//the light objects are mostly structural data; 
//
//LightType type = LightType::None;
//glm::vec3 position = glm::vec3(0.0f);
//glm::vec3 direction = glm::vec3(0.0f);
//glm::vec3 color = glm::vec3(1.0f);
//float intensity = 1.0f;
//float constant = 1.0f;
//float linear = 0.09f;
//float quadratic = 0.032f;
//float cutOff = glm::cos(glm::radians(12.5f));
//float outerCutOff = glm::cos(glm::radians(15.0f));


namespace Rudy {

	//me: relating high level enums;
	enum class LightType
	{
		None = 0,
		Directional,
		Point,
		Spot,
	};

	//me:  the light object
	class Light
	{
	public:  
		~Light() = default;

	    Transform transform; //position, rotation, scale

	};


	class PointLight : public Light
	{
	public:
		glm::vec3 Color = glm::vec3(1.0f, 1.0f, 1.0f);
		float Intensity = 1.0f; 

	};


	class DirectionalLight : public Light
	{
	public:
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		float intensity = 2.0f;
		glm::vec3 direction = - glm::vec3(1.0f, 0.5f, 1.0f); 


		static Ref<DirectionalLight> Create()
		{
			Ref<DirectionalLight> light = CreateRef<DirectionalLight>();
			return light;
		}
 

	};



}