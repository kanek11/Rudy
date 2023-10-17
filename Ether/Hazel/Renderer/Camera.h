#pragma once

#include <glm/glm.hpp>

#include "Hazel/Events/Input.h"

//convention for now:
//use Unity convention for camera
//right handed,  y up, looking at -z of world: important,  this is the default for glm::lookAt
//be careful with convention when using third-party,

 //recommend using Euler angle to avoid mess with axes.


//navigation:  use the same convention similar to Unreal5
//raw input = world navigation around the camera
//alt = lookat navigation around the focal point
// 
// raw:
//left button +  left/right mouse.  rotate yaw.  up/down mouse = move in horizontal plane
//right button =  yaw & pitch
//middle button = pan  in vertical plane
// 
//implement:
//Alt +
//left button = rotate around focal point
//middle button = pan ;
// 
// 
//right button = zoom in/out  , 
 



namespace Hazel {
 
	//warn: use Eigen instead of glm for custom math functions.
	//glm is technically a math library for graphics, not a general math library.  all its functions are routined and not to be used for general math.
	//glm somehow handles affine matrix etc differently, eg: the translation part is not the last column but the last row.
	//class MyMath {};



	 
	class Camera
	{
	public:

		~Camera() = default;
		Camera() = default; 

	 
		//void OnEvent(Event& e);


		const glm::mat4 GetProjectionViewMatrix() const{
			return GetProjectionMatrix() * GetViewMatrix();
		} 

		const glm::mat4& GetViewMatrix() const {   
			//set to always look to orignial for now 
			//avoid return object directly. it cause undefined behavior.
			//glm::mat4 view = MyMath::lookAt(m_Position, m_FocalPoint, m_Up);
			return m_ViewMatrix;
		}

		const glm::mat4& GetProjectionMatrix() const { 
			 
			//glm ::mat4 projection = MyMath::perspective(m_FOV, m_AspectRatio, m_Near, m_Far); 
			return  m_ProjectionMatrix;
			 
		} 



		//navigation onupdate;
		//here the event functions update the paramters, and reflect when call getview/getprojectionmatrix.
		void OnUpdate(float ts) {


			m_ViewMatrix = glm::lookAt(m_Position, m_FocalPoint, m_Up);
			m_ProjectionMatrix = glm::perspective(m_FOV, m_AspectRatio, m_Near, m_Far);

			glm::vec2 CurrentMousePos =  Input::GetMousePosition();
			//HZ_CORE_INFO("camera: the mouse cursor is {0}, {1}", m_LastMousePos.x, m_LastMousePos.y);

			if (Input::IsKeyPressed(Key::LeftAlt))
			{ 
				if (!m_ButtonPressedLastFrame)
				{
					HZ_CORE_INFO("Alt pressed");
					m_ButtonPressedLastFrame = true;  //set for next frame, donothing in this frame
					return;
				}
				 
				//update necessary info
				glm::vec2 delta = (CurrentMousePos - m_LastMousePos) * m_Sensitivity;
				m_Distance =  glm::length(m_Position - m_FocalPoint);

				//HZ_CORE_INFO("camera: the delta is {0}, {1}", delta.x, delta.y);
				

				if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
					MouseRotate(delta);
				else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
					MouseZoom(delta);
				else if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
					MousePan(delta); 
			}
			else
			{
				m_ButtonPressedLastFrame = false;  
			}

			//finally update the last mouse position  after all the calculations
			m_LastMousePos = CurrentMousePos;
			 

		}

		//tears of kingdom: allow only x or y axis in the same time

		//easier to use Euler angle .  rotate around 1 axis at a time, 
		//make sure , the up vector is always up by locking the roll angle to 0.
		//update orientation,  camera position.

		//negate delta, so when drag down, the camera move up
		void MouseRotate(const glm::vec2& delta) {

			//HZ_CORE_INFO("camera rotating");

			glm::mat4 yawMat = glm::mat4(1.0f);
			glm::mat4 pitchMat = glm::mat4(1.0f);
			//if (delta.x > delta.y)
			//{//yaw
				float yaw = -delta.x;
				glm::vec3 yawAxis = m_Up;
				 yawMat = glm::rotate(yawMat, glm::radians(yaw), yawAxis); 
			//}
			//else
			//{//pitch
			 float pitch = -delta.y;
			 glm::vec3 pitchAxis = m_Right;
			 pitchMat = glm::rotate(pitchMat, glm::radians(pitch), pitchAxis);
			
			//} 
			// 
			//rotate
			glm::vec4 newFront = yawMat * pitchMat * glm::vec4(m_Front, 0.0f);
			m_Front = glm::normalize(glm::vec3(newFront));

			//update right and up
			m_Right = glm::normalize(glm::cross(m_Front, m_Up));
			m_Up = glm::normalize(glm::cross(m_Right, m_Front));

			//update camera position
			m_Position = m_FocalPoint - m_Front * m_Distance;

		}


		//update distance and camera position
		void MouseZoom(const glm::vec2& delta) {

			//HZ_CORE_INFO("camera zooming");

			//only use y axis,  positive delta.y means zoom in 

			float zoom =  (float)0.1* delta.y;   
			m_Distance -= zoom;

			//stop zooming in if too close
			if (m_Distance < 1.0f) m_Distance = 1.0f;

			//update camera position
			m_Position = m_FocalPoint - m_Front * m_Distance;
		}

		//move on the plane perpendicular to the view direction -z, which is current right and up
		//move both position and focal point,  the orientation is not changed
		//the moving speed is proportional to the distance to the focal point,  tan= move/ distance
		void MousePan(const glm::vec2& delta) {

			//HZ_CORE_INFO("camera panning ");
			float panSpeed = 0.005f * m_Distance;
			glm::vec3 pan = -delta.x * panSpeed * m_Right + delta.y * panSpeed * m_Up;   
			m_FocalPoint += pan;
			m_Position += pan;
		}



		glm::vec3 GetPosition() const { return m_Position; }


		 

		//make sure initial values are compatible
		//align with opengl default values,  from +z position look to -z direction; 
		glm::vec3 m_Position = { 0.0f, 0.0f, 5.0f };

		//view parameters
        glm::vec3 m_Front = { 0.0f, 0.0f, -1.0f };  //-z
        glm::vec3 m_Up = { 0.0f, 1.0f, 0.0f };   //y
        glm::vec3 m_Right = { -1.0f, 0.0f, 0.0f };  //-x 

	    glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };  //look at origin. 

		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		
		//perspective parameters
		float m_FOV = 45.0f, m_AspectRatio = 16.0f / 9.0f, m_Near = 0.1f, m_Far = 1000.0f;

		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);  


		//navigation related  

		glm::vec2 m_LastMousePos = { 0.0f, 0.0f };   //update every frame

		bool m_ButtonPressedLastFrame = false;
		float m_Sensitivity = 0.3f;    //scale the mouse movement. on screen space.

		float m_Distance;   //= length(position - focalpoint)




	};


}
