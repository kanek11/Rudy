#pragma once

#include <glm/glm.hpp>

//convention for now:
//use Unity convention for camera
//right handed,  y up, x right,  looking at -z: important,  this is the default for glm::lookAt
//be careful with convention when using third-party,



namespace Hazel {
 
	class MyMath
	{
	public:
		static glm::mat4 lookAt(glm::vec3 eye, glm::vec3 focus, glm::vec3 up) {

			glm::vec3 translate = eye - focus;

			glm::vec3 forward_Z = glm::normalize(translate);
			glm::vec3 right_X = glm::normalize(glm::cross(up, forward_Z));
			glm::vec3 up_Y = glm::normalize(glm::cross(forward_Z, right_X));

			glm::mat4 view(
				right_X.x, right_X.y, right_X.z, -translate.x,
				up_Y.x, up_Y.y, up_Y.z, -translate.y,
				forward_Z.x, forward_Z.y, forward_Z.z, -translate.z,
				0, 0, 0, 1);

			return view;

		}

		static glm::mat4 perspective(float eye_fov, float aspect_ratio,
			float zNear, float zFar)
		{
			// Students will implement this function

			glm::mat4 projection = glm::mat4(1.0f);

			// TODO: Implement this function
			// Create the projection matrix for the given parameters.

			double n = zNear;
			double f = zFar;

			double rad = eye_fov * 3.1415926 / 180;
			double t = tan(rad / 2.0f) * abs(n);
			double r = aspect_ratio * t;

			glm::mat4 squishMatrix(
				n, 0, 0, 0,
				0, n, 0, 0,
				0, 0, (n + f), -n * f,
				0, 0, 1, 0);

			glm::mat4 Ot(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, -(n + f) / 2,
				0, 0, 0, 1);

			glm::mat4 Os(
				1 / r, 0, 0, 0,
				0, 1 / t, 0, 0,
				0, 0, 2 / (n - f), 0,
				0, 0, 0, 1);

			projection = Os * Ot * squishMatrix * projection;


			return projection;
		}


	};



	 
	class Camera
	{
	public:
		Camera() = default;  
		~Camera() = default;

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

			 //return glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
		} 




	protected:

		glm::vec3 m_Position = { 0.0f, 1.0f, 3.0f };

		//view parameters
        glm::vec3 m_Front = { 0.0f, 0.0f, 1.0f };  //z
        glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };   //y
        glm::vec3 m_Right = { 1.0f, 0.0f, 0.0f };  //x 

		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };  //look at origin.

		//perspective parameters
		float m_FOV = 45.0f, m_AspectRatio = 16.0f / 9.0f, m_Near = 0.1f, m_Far = 1000.0f;


		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);


	};


}
