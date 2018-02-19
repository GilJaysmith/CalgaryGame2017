#pragma once

#include "sdks/glfw/include/GLFW/glfw3.h"

class GamepadHandler;
class InputHandler;

namespace Input
{
	enum TYPE
	{
		NONE,
		PRESSED,
		HELD,
		RELEASED,
		COUNT
	};

	void Initialize();

	void Terminate();;

	void Update(GLFWwindow* window);

	bool IsPressed(int key);
	bool JustPressed(int key);
	bool JustReleased(int key);

	void RegisterInputHandler(InputHandler* handler);
	void UnregisterInputHandler(InputHandler* handler);

	unsigned int GetNumGamepads();
	GamepadHandler* GetGamepadHandler(int);
}