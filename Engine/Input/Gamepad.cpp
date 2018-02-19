#include "Engine/Pch.h"

#include "Engine/Input/Gamepad.h"
#include "Engine/Input/Input.h"


std::map<int, GamepadButton::TYPE> button_lookup = {
	{ 0, GamepadButton::A },
	{ 1, GamepadButton::B },
	{ 2, GamepadButton::X },
	{ 3, GamepadButton::Y },
	{ 4, GamepadButton::L1 },
	{ 5, GamepadButton::R1 },
	{ 6, GamepadButton::BACK },
	{ 7, GamepadButton::START },
	{ 8, GamepadButton::L3 },
	{ 9, GamepadButton::R3 },
	{ 10, GamepadButton::DPAD_UP },
	{ 11, GamepadButton::DPAD_RIGHT },
	{ 12, GamepadButton::DPAD_DOWN },
	{ 13, GamepadButton::DPAD_LEFT }
};


std::map<int, GamepadAxis::TYPE> axis_lookup = {
	{ 0, GamepadAxis::LEFT_STICK_HORIZ },
	{ 1, GamepadAxis::LEFT_STICK_VERT },
	{ 2, GamepadAxis::RIGHT_STICK_HORIZ },
	{ 3, GamepadAxis::RIGHT_STICK_VERT },
	{ 4, GamepadAxis::LEFT_TRIGGER },
	{ 5, GamepadAxis::RIGHT_TRIGGER }
};


GamepadHandler::GamepadHandler(unsigned int joystick)
	: m_Joystick(joystick)
	, m_Connected(false)
	, m_ButtonPressed(GamepadButton::COUNT)
	, m_Axis(GamepadAxis::COUNT)
{
}

GamepadHandler::~GamepadHandler()
{

}

void GamepadHandler::Connect()
{
	m_Connected = true;
	Update();
}

void GamepadHandler::Disconnect()
{
	m_Connected = false;
}

void GamepadHandler::Update()
{
	int button_count;
	const unsigned char* buttons = glfwGetJoystickButtons(m_Joystick, &button_count);

	for (auto button : button_lookup)
	{
		if (button.first < button_count)
		{
			m_ButtonPressed[button.second] = buttons[button.first] == GLFW_PRESS;
		}
	}

	const float DEADZONE = 0.05f;

	int axis_count;
	const float* axes = glfwGetJoystickAxes(m_Joystick, &axis_count);
	for (auto axis : axis_lookup)
	{
		if (axis.first < axis_count)
		{
			float axis_val = axes[axis.first];
			axis_val += 1.0f;
			axis_val /= 2.0f;
			m_Axis[axis.second] = axis_val;
		}
	}
}

float GamepadHandler::Axis(GamepadAxis::TYPE axis) const
{
	return m_Connected ? m_Axis[axis] : 0.0f;
}

bool GamepadHandler::ButtonPressed(GamepadButton::TYPE button) const
{
	return m_Connected ? m_ButtonPressed[button] : false;
}

bool GamepadHandler::AnyButtonPressed() const
{
	return std::any_of(m_ButtonPressed.begin(), m_ButtonPressed.end(), [](bool x) { return x; });
}


namespace GamepadManager
{
	std::map<int, int> s_PlayerToGamepad;
	unsigned int s_NextPlayerToConnect = 0;

	void Update()
	{
		// Check all controllers assigned to a player to see if they've disconnected.
		for (auto controller : s_PlayerToGamepad)
		{
			GamepadHandler* gamepad = Input::GetGamepadHandler(controller.second);
			if (!gamepad->Connected())
			{
				// In-use gamepad has disconnected.
				// TODO: handle this...
			}
		}

		// Check all controllers which are not currently assigned to a player to see if any button has been pressed.
		// If so then allocate that controller to the next player.
		for (unsigned int controller_idx = 0; controller_idx < Input::GetNumGamepads(); ++controller_idx)
		{
			if (!std::any_of(s_PlayerToGamepad.begin(), s_PlayerToGamepad.end(), [controller_idx](auto it) {return it.second == controller_idx; }))
			{
				// Controller not assigned to a player.
				GamepadHandler* gamepad = Input::GetGamepadHandler(controller_idx);
				if (gamepad->AnyButtonPressed())
				{
					// Button was pressed - next player gets this controller.
					s_PlayerToGamepad[s_NextPlayerToConnect++] = controller_idx;
				}
			}
		}
	}

	bool PlayerIsConnected(int player_idx)
	{
		return s_PlayerToGamepad.find(player_idx) != s_PlayerToGamepad.end();
	}

	GamepadHandler* GamepadForPlayer(int player_idx)
	{
		if (PlayerIsConnected(player_idx))
		{
			int gamepad_idx = s_PlayerToGamepad.at(player_idx);
			return Input::GetGamepadHandler(gamepad_idx);
		}
		return nullptr;
	}
}
