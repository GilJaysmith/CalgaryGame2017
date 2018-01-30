#include "Engine/Pch.h"

#include "Engine/Cameras/DebugCamera.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Input/Input.h"


DebugCamera::DebugCamera()
{
	m_CameraPos = glm::vec3(0.0f, 20.0f, 15.0f);
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraYaw = -glm::pi<float>() / 2.0f;
	m_CameraPitch = -glm::pi<float>() / 5.0f;
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(const Time& frame_time)
{
	float camera_speed = 1.0f;
	float camera_yaw_speed = 0.2f;
	float camera_pitch_speed = 0.2f;

	if (Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::HELD)
	{
		camera_speed *= 20.0f;
		camera_yaw_speed *= 10.0f;
		camera_pitch_speed *= 10.0f;
	}

	if (Input::GetKeyEvent(GLFW_KEY_LEFT_CONTROL) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT_CONTROL) == Input::HELD)
	{
		camera_speed *= 20.0f;
		camera_yaw_speed *= 10.0f;
		camera_pitch_speed *= 10.0f;
	}

	if (Input::GetKeyEvent(GLFW_KEY_W) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_W) == Input::HELD)
	{
		m_CameraPos += m_CameraFront * camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_S) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_S) == Input::HELD)
	{
		m_CameraPos -= m_CameraFront * camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_A) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_A) == Input::HELD)
	{
		m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_D) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_D) == Input::HELD)
	{
		m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_Q) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_Q) == Input::HELD)
	{
		m_CameraPos += m_CameraUp * camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_E) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_E) == Input::HELD)
	{
		m_CameraPos -= m_CameraUp * camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_LEFT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT) == Input::HELD)
	{
		m_CameraYaw -= camera_yaw_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_RIGHT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_RIGHT) == Input::HELD)
	{
		m_CameraYaw += camera_yaw_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_UP) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_UP) == Input::HELD)
	{
		m_CameraPitch += camera_pitch_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_DOWN) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_DOWN) == Input::HELD)
	{
		m_CameraPitch -= camera_pitch_speed * frame_time.toSeconds();
	}

	m_CameraPitch = glm::clamp(m_CameraPitch, -glm::pi<float>() / 2.1f, glm::pi<float>() / 2.1f);

	glm::vec3 front;
	front.x = cos(m_CameraPitch) * cos(m_CameraYaw);
	front.y = sin(m_CameraPitch);
	front.z = cos(m_CameraPitch) * sin(m_CameraYaw);
	m_CameraFront = glm::normalize(front);

	m_ViewMatrix = glm::lookAt(
		m_CameraPos,
		m_CameraPos + m_CameraFront,
		m_CameraUp);
}
