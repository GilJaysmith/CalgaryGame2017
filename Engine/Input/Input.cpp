#include "Engine/Pch.h"

#include "Engine/Input/Input.h"
#include "Engine/Logging/Logging.h"

namespace Input
{
	std::map<unsigned int, TYPE> s_KeyEvents;

	void Initialize()
	{
		for (int i = GLFW_KEY_UNKNOWN; i <= GLFW_KEY_LAST; ++i)
		{
			s_KeyEvents[i] = TYPE::NONE;
		}
	}

	void Terminate()
	{

	}

	void Update(GLFWwindow* window)
	{
		for (int i = GLFW_KEY_UNKNOWN; i <= GLFW_KEY_LAST; ++i)
		{
			switch (glfwGetKey(window, i))
			{ 
				case GLFW_PRESS:
					if (s_KeyEvents[i] == TYPE::NONE)
					{
						s_KeyEvents[i] = TYPE::PRESSED;
					}
					else if (s_KeyEvents[i] == TYPE::PRESSED)
					{
						s_KeyEvents[i] = TYPE::HELD;
					}
					break;
				case GLFW_RELEASE:
					if (s_KeyEvents[i] == TYPE::PRESSED || s_KeyEvents[i] == HELD)
					{
						s_KeyEvents[i] = TYPE::RELEASED;
					}
					else if (s_KeyEvents[i] == TYPE::RELEASED)
					{
						s_KeyEvents[i] = TYPE::NONE;
					}
					break;
			}
		}
	}

	Input::TYPE GetKeyEvent(int key)
	{
		auto it = s_KeyEvents.find(key);
		if (it == s_KeyEvents.end())
		{
			Logging::Log("Logging", "Unknown key in GetKeyEvent");
			return TYPE::NONE;
		}
		return s_KeyEvents[key];
	}
}