#include "Engine/Pch.h"

#include "Engine/Input/Gamepad.h"
#include "Engine/Input/Input.h"
#include "Engine/Input/InputHandler.h"

namespace Input
{
	std::map<unsigned int, TYPE> s_KeyEvents;
	std::vector<InputHandler*> s_Handlers;
	std::map<int, GamepadHandler*> s_GamepadHandlers;

	void joystick_callback(int joystick, int event)
	{
		switch (event)
		{
		case GLFW_CONNECTED:
			{
				std::cout << "joystick_callback: joystick " << joystick << " connected!";
				const char* name = glfwGetJoystickName(joystick);
				std::cout << "This joystick is called " << name << std::endl;
				s_GamepadHandlers[joystick]->Connect();
				break;
			}
		case GLFW_DISCONNECTED:
			{
				std::cout << "joystick_callback: joystick " << joystick << " disconnected!" << std::endl;
				s_GamepadHandlers[joystick]->Disconnect();
				break;
			}
		}
	}

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

		for (int joystick = GLFW_JOYSTICK_1; joystick <= GLFW_JOYSTICK_LAST; ++joystick)
		{
			s_GamepadHandlers[joystick] = MemNew(MemoryPool::System, GamepadHandler)(joystick);
			if (glfwJoystickPresent(joystick))
			{
				s_GamepadHandlers[joystick]->Connect();
			}
		}

		glfwSetJoystickCallback(joystick_callback);
	}

	void Terminate()
	{
		for (auto it : s_GamepadHandlers)
		{
			MemDelete(it.second);
		}
		s_GamepadHandlers.clear();
	}

	void UpdateJoysticks()
	{
		for (auto it : s_GamepadHandlers)
		{
			it.second->Update();
		}
	}

	void UpdateKeys(GLFWwindow* window)
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
	}

	void Update(GLFWwindow* window)
	{
		UpdateJoysticks();
		UpdateKeys(window);
		GamepadManager::Update();
		
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

	bool IsPressed(int key)
	{
		Input::TYPE key_input = s_KeyEvents[key];
		return key_input == Input::PRESSED || key_input == Input::HELD;
	}

	bool JustPressed(int key)
	{
		return s_KeyEvents[key] == Input::PRESSED;
	}

	bool JustReleased(int key)
	{
		return s_KeyEvents[key] == Input::RELEASED;
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

	unsigned int GetNumGamepads()
	{
		return static_cast<unsigned int>(s_GamepadHandlers.size());
	}

	GamepadHandler* GetGamepadHandler(int gamepad_idx)
	{
		return s_GamepadHandlers.at(gamepad_idx);
	}
}