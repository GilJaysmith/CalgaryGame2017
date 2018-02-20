#include "Engine/Pch.h"

#include "Engine/Input/Gamepad.h"
#include "Engine/Input/Input.h"
#include "Engine/Vehicles/VehicleFollowCameraInputHandler.h"


VehicleFollowCameraInputHandler::VehicleFollowCameraInputHandler(int player_idx)
	: m_PlayerIdx(player_idx)
{
	Input::RegisterInputHandler(this);
}

VehicleFollowCameraInputHandler::~VehicleFollowCameraInputHandler()
{
	Input::UnregisterInputHandler(this);
}

void VehicleFollowCameraInputHandler::Update()
{
	// Keyboard.
	m_Yaw = Input::IsPressed(GLFW_KEY_LEFT) ? -1.0f : Input::IsPressed(GLFW_KEY_RIGHT) ? 1.0f : 0.0f;
	m_Pitch = Input::IsPressed(GLFW_KEY_DOWN) ? -1.0f : Input::IsPressed(GLFW_KEY_UP) ? 1.0f : 0.0f;

	// Gamepad.
	GamepadHandler* gamepad = GamepadManager::GamepadForPlayer(m_PlayerIdx);
	if (gamepad)
	{
		m_Yaw = gamepad->Axis(GamepadAxis::RIGHT_STICK_HORIZ);
		m_Pitch = gamepad->Axis(GamepadAxis::RIGHT_STICK_VERT);
	}
}
