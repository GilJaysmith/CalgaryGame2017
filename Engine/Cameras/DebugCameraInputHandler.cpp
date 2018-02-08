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

	if (Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::HELD)
	{
		m_Fast = true;
		if (Input::GetKeyEvent(GLFW_KEY_LEFT_CONTROL) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT_CONTROL) == Input::HELD)
		{
			m_SuperFast = true;
		}
	}

	if (Input::GetKeyEvent(GLFW_KEY_W) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_W) == Input::HELD)
	{
		m_Forward = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_S) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_S) == Input::HELD)
	{
		m_Backward = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_A) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_A) == Input::HELD)
	{
		m_Left = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_D) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_D) == Input::HELD)
	{
		m_Right = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_Q) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_Q) == Input::HELD)
	{
		m_Up = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_E) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_E) == Input::HELD)
	{
		m_Down = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_LEFT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT) == Input::HELD)
	{
		m_YawLeft = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_RIGHT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_RIGHT) == Input::HELD)
	{
		m_YawRight = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_UP) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_UP) == Input::HELD)
	{
		m_PitchUp = true;
	}
	if (Input::GetKeyEvent(GLFW_KEY_DOWN) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_DOWN) == Input::HELD)
	{
		m_PitchDown = true;
	}
}