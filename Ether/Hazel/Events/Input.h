#pragma once

#include "./KeyCodes.h"
#include "./MouseCodes.h"

#include <glm/glm.hpp>

namespace Hazel {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
		//static float GetMouseScroll();  //only support callback. TODO: make it work .

		static void SetWindowContext(void * windowContext) { s_WindowContext = windowContext; }


		//me: add a context window to decouple from application.
		static void * s_WindowContext;

	};
}
