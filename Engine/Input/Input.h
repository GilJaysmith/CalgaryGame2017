#pragma once

#include "sdks/glfw/include/GLFW/glfw3.h"

class InputHandler;

namespace Input
{
	enum TYPE
	{
		NONE,
		PRESSED,
		HELD,
		RELEASED,
	};

	void Initialize();

	void Terminate();;

	void Update(GLFWwindow* window);

	TYPE GetKeyEvent(int key);

	void RegisterInputHandler(InputHandler* handler);
	void UnregisterInputHandler(InputHandler* handler);
}