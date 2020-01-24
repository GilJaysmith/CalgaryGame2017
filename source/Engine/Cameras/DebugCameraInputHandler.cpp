#include "Engine/Pch.h"

#include "Engine/Cameras/DebugCameraInputHandler.h"
#include "Engine/Input/Input.h"


DebugCameraInputHandler::DebugCameraInputHandler()
{

}

DebugCameraInputHandler::~DebugCameraInputHandler()
{

}

void DebugCameraInputHandler::Update()
{
	m_Fast = m_SuperFast = false;
	m_Forward = m_Backward = m_Left = m_Right = false;
	m_Up = m_Down = false;
	m_YawLeft = m_YawRight = false;
	m_PitchUp = m_PitchDown = false;

	if (Input::IsPressed(GLFW_KEY_LEFT_SHIFT))
	{
		m_Fast = true;
		if (Input::IsPressed(GLFW_KEY_LEFT_CONTROL))
		{
			m_SuperFast = true;
		}
	}

	if (Input::IsPressed(GLFW_KEY_W))
	{
		m_Forward = true;
	}
	if (Input::IsPressed(GLFW_KEY_S))
	{
		m_Backward = true;
	}
	if (Input::IsPressed(GLFW_KEY_A))
	{
		m_Left = true;
	}
	if (Input::IsPressed(GLFW_KEY_D))
	{
		m_Right = true;
	}
	if (Input::IsPressed(GLFW_KEY_Q))
	{
		m_Up = true;
	}
	if (Input::IsPressed(GLFW_KEY_E))
	{
		m_Down = true;
	}
	if (Input::IsPressed(GLFW_KEY_LEFT))
	{
		m_YawLeft = true;
	}
	if (Input::IsPressed(GLFW_KEY_RIGHT))
	{
		m_YawRight = true;
	}
	if (Input::IsPressed(GLFW_KEY_UP))
	{
		m_PitchUp = true;
	}
	if (Input::IsPressed(GLFW_KEY_DOWN))
	{
		m_PitchDown = true;
	}
}