#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Cameras/Camera.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"

#include "Game/GameStates/TestGameState.h"

TestGameState::TestGameState()
{
}

TestGameState::~TestGameState()
{

}


void TestGameState::OnEnter()
{
	for (int i = 0; i < _countof(m_Entities); ++i)
	{
		glm::mat4 world_transform;
		float x = -20.0f + i % 40;
		float y = 5.0f + 5.f * (rand() / (float)RAND_MAX);
		float z = 5.0f - i / 40;
		world_transform = glm::translate(world_transform, glm::vec3(x, y, z));
		m_Entities[i] = Entity::CreateEntity("cube", world_transform);
		m_Entities[i]->SetTransform(world_transform);
	}

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 1000.0f);
	ShaderManager::SetUniform4fv("proj", proj);
	CheckGLError();

	m_Camera = MemNew(MemoryPool::Rendering, Camera);
	Renderer::SetActiveCamera(m_Camera);

	m_Time = 0.0f;
	m_UpdateTime = false;

	m_CameraPos = glm::vec3(0.0f, 20.0f, 15.0f);
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraYaw = -glm::pi<float>() / 2.0f;
	m_CameraPitch = -glm::pi<float>() / 5.0f;
}


void TestGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);

	if (Input::GetKeyEvent(GLFW_KEY_M) == Input::PRESSED)
	{
		Audio::PlaySound("a2002011001-e02.wav");
	}

	if (Input::GetKeyEvent(GLFW_KEY_T) == Input::PRESSED)
	{
		m_UpdateTime = !m_UpdateTime;
	}
	if (m_UpdateTime)
	{
		m_Time += 0.01f;
	}

	float camera_speed = 1.0f;
	float camera_yaw_speed = 0.2f;
	float camera_pitch_speed = 0.2f;

	if (Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::HELD)
	{
		camera_speed *= 10.0f;
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

	m_CameraPitch = glm::clamp(m_CameraPitch, -glm::pi<float>() / 3.0f , glm::pi<float>() / 3.0f);

	glm::vec3 front;
	front.x = cos(m_CameraPitch) * cos(m_CameraYaw);
	front.y = sin(m_CameraPitch);
	front.z = cos(m_CameraPitch) * sin(m_CameraYaw);
	m_CameraFront = glm::normalize(front);

	glm::mat4 view_matrix = glm::lookAt(
		m_CameraPos,
		m_CameraPos + m_CameraFront,
		m_CameraUp);
	m_Camera->SetViewMatrix(view_matrix);
}

void TestGameState::OnExit()
{
	MemDelete(m_Camera);
	for (auto it : m_Entities)
	{
		EntityManager::DestroyEntity(it);
	}
}
