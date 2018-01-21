#pragma once

#include "sdks/glfw/include/GLFW/glfw3.h"

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
}