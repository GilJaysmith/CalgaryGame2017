#include "Engine/Pch.h"

#include "Engine/Cameras/DebugCamera.h"
#include "Engine/Cameras/DebugCameraInputHandler.h"
#include "Engine/Entities/Entity.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/PhysicsMessages.h"


DebugCamera::DebugCamera()
	: m_InputHandler(MemNew(MemoryPool::Rendering, DebugCameraInputHandler))
{
	m_CameraPos = glm::vec3(0.0f, 20.0f, 15.0f);
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraYaw = -glm::pi<float>() / 2.0f;
	m_CameraPitch = -glm::pi<float>() / 5.0f;
}

DebugCamera::~DebugCamera()
{
	MemDelete(m_InputHandler);
}

void DebugCamera::Update(const Time& frame_time)
{
	float camera_speed = 1.0f;
	float camera_yaw_speed = 0.2f;
	float camera_pitch_speed = 0.2f;

	if (m_InputHandler->Fast())
	{
		camera_speed *= 20.0f;
		camera_yaw_speed *= 10.0f;
		camera_pitch_speed *= 10.0f;
	}

	if (m_InputHandler->SuperFast())
	{
		camera_speed *= 20.0f;
		camera_yaw_speed *= 10.0f;
		camera_pitch_speed *= 10.0f;
	}

	if (m_InputHandler->Forward())
	{
		m_CameraPos += m_CameraFront * camera_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->Backward())
	{
		m_CameraPos -= m_CameraFront * camera_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->Left())
	{
		m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * camera_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->Right())
	{
		m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * camera_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->Up())
	{
		m_CameraPos += m_CameraUp * camera_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->Down())
	{
		m_CameraPos -= m_CameraUp * camera_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->YawLeft())
	{
		m_CameraYaw -= camera_yaw_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->YawRight())
	{
		m_CameraYaw += camera_yaw_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->PitchUp())
	{
		m_CameraPitch += camera_pitch_speed * frame_time.toSeconds();
	}
	if (m_InputHandler->PitchDown())
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

	if (Input::JustPressed(GLFW_KEY_SPACE))
	{
		// Fire a pandacube.
		glm::mat4 transform;
		transform = glm::translate(transform, m_CameraPos);
		Entity* pandacube = Entity::CreateEntity("Cube", transform);
		
		const float LV_SCALE = 20.0f;
		glm::vec3 linear_velocity = glm::normalize(m_CameraFront) * LV_SCALE;
		Message_PhysicsSetLinearVelocity pslv(linear_velocity);
		pandacube->OnMessage(&pslv);

		const float AV_SCALE = 10.0f;
		glm::vec3 angular_velocity(
			((rand() - rand()) / (float)RAND_MAX) * AV_SCALE,
			((rand() - rand()) / (float)RAND_MAX) * AV_SCALE,
			((rand() - rand()) / (float)RAND_MAX) * AV_SCALE
			);
		Message_PhysicsSetAngularVelocity psav(angular_velocity);
		pandacube->OnMessage(&psav);

		if (Input::IsPressed(GLFW_KEY_LEFT_CONTROL))
		{
			Message_PhysicsDisableGravity pdg(true);
			pandacube->OnMessage(&pdg);
		}

	}
}

void DebugCamera::OnSetActive(bool active)
{
	if (active)
	{
		Input::RegisterInputHandler(m_InputHandler);
	}
	else
	{
		Input::UnregisterInputHandler(m_InputHandler);
	}
}
