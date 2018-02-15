#include "Engine/Pch.h"

#include "Engine/Input/Input.h"
#include "Engine/Input/InputHandler.h"
#include "Engine/Logging/Logging.h"

namespace Input
{
	std::map<unsigned int, TYPE> s_KeyEvents;
	std::vector<InputHandler*> s_Handlers;

	void Initialize()
	{
		s_KeyEvents[GLFW_KEY_ESCAPE] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_SPACE] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_LEFT_CONTROL] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_LEFT_SHIFT] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_LEFT] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_RIGHT] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_UP] = TYPE::NONE;
		s_KeyEvents[GLFW_KEY_DOWN] = TYPE::NONE;
		for (int i = GLFW_KEY_A; i <= GLFW_KEY_Z; ++i)
		{
			s_KeyEvents[i] = TYPE::NONE;
		}
	}

	void Terminate()
	{

	}

	void Update(GLFWwindow* window)
	{
		for (auto key : s_KeyEvents)
		{
			unsigned int key_code = key.first;
			switch (glfwGetKey(window, key_code))
			{ 
				case GLFW_PRESS:
					if (s_KeyEvents[key_code] == TYPE::NONE)
					{
						s_KeyEvents[key_code] = TYPE::PRESSED;
					}
					else if (s_KeyEvents[key_code] == TYPE::PRESSED)
					{
						s_KeyEvents[key_code] = TYPE::HELD;
					}
					break;
				case GLFW_RELEASE:
					if (s_KeyEvents[key_code] == TYPE::PRESSED || s_KeyEvents[key_code] == HELD)
					{
						s_KeyEvents[key_code] = TYPE::RELEASED;
					}
					else if (s_KeyEvents[key_code] == TYPE::RELEASED)
					{
						s_KeyEvents[key_code] = TYPE::NONE;
					}
					break;
			}
		}

		for (auto handler : s_Handlers)
		{
			handler->Update();
		}
	}

	Input::TYPE GetKeyEvent(int key)
	{
		auto it = s_KeyEvents.find(key);
		if (it == s_KeyEvents.end())
		{
			Logging::Log("Input", "Unknown key in GetKeyEvent");
			return TYPE::NONE;
		}
		return s_KeyEvents[key];
	}

	void RegisterInputHandler(InputHandler* handler)
	{
		s_Handlers.push_back(handler);
	}

	void UnregisterInputHandler(InputHandler* handler)
	{
		for (auto it = s_Handlers.begin(); it != s_Handlers.end(); ++it)
		{
			if (*it == handler)
			{
				s_Handlers.erase(it);
				break;
			}
		}
	}
}