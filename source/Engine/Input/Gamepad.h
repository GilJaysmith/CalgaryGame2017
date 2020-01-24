#pragma once


namespace GamepadButton
{
	enum TYPE
	{
		A,
		B,
		X,
		Y,
		L1,
		R1,
		BACK,
		START,
		L3,
		R3,
		DPAD_UP,
		DPAD_RIGHT,
		DPAD_DOWN,
		DPAD_LEFT,
		COUNT
	};
}

namespace GamepadAxis
{
	enum TYPE
	{
		LEFT_STICK_HORIZ,
		LEFT_STICK_VERT,
		RIGHT_STICK_HORIZ,
		RIGHT_STICK_VERT,
		LEFT_TRIGGER,
		RIGHT_TRIGGER,
		COUNT
	};
}

class GamepadHandler
{
public:
	GamepadHandler(unsigned int joystick);
	~GamepadHandler();

	void Connect();
	void Disconnect();
	bool Connected() const { return m_Connected;  }

	void Update();

	unsigned int Joystick() const { return m_Joystick; }
	float Axis(GamepadAxis::TYPE axis) const;
	bool ButtonPressed(GamepadButton::TYPE button) const;
	bool AnyButtonPressed() const;

protected:
	unsigned int m_Joystick;
	bool m_Connected;

	std::vector<bool> m_ButtonPressed;
	std::vector<float> m_Axis;
};

namespace GamepadManager
{
	void Update();

	bool PlayerIsConnected(int player_idx);
	GamepadHandler* GamepadForPlayer(int player_idx);
}
