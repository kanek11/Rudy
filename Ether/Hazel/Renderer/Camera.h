#pragma once

#include <glm/glm.hpp>

//convention for now:
//use Unity convention for camera
//right handed,  y up, x right,  +z looking forward


namespace Hazel {





	class Camera
	{
	public:
		Camera() = default;  
		virtual ~Camera() = default;

		//
		//void OnUpdate(Timestep ts);
		//void OnEvent(Event& e);


		const glm::mat4 GetViewProjectionMatrix() const{
			return GetProjectionMatrix() * GetViewMatrix();
		} 

		const glm::mat4& GetViewMatrix() const {   
			//set to always look to orignial for now 
			return glm::lookAt(m_Position, m_FocalPoint, m_Up); 
		}
		 

		const glm::mat4& GetProjectionMatrix() const { 
			return glm::perspective(m_FOV, m_AspectRatio, m_Near, m_Far);
		} 


	protected:

		glm::vec3 m_Position = { 0.0f, 1.0f, -10.0f };


		//view parameters
        glm::vec3 m_Front = { 0.0f, 0.0f, 1.0f };  //z
        glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };   //y
        glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };  //x 

		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };


		//perspective parameters
		float m_FOV = 45.0f, m_AspectRatio = 16.0f / 9.0f, m_Near = 0.1f, m_Far = 1000.0f;


		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);


	};


}
