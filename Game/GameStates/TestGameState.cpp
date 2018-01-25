#include "Game/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/Entities/Entity.h"
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
		m_Entities[i] = Entity::CreateEntity("cube");
		glm::mat4 world_transform;
		world_transform = glm::scale(world_transform, glm::vec3(0.8f, 0.8f, 0.8f));
		world_transform = glm::translate(world_transform, glm::vec3(-5.0 + i % 10, 0.0f, 5.0f - i / 10));
		m_Entities[i]->SetTransform(world_transform);
		m_Axis[i] = glm::normalize(glm::vec3(rand()- rand(), rand() - rand(), rand()- rand()));
		m_Speed[i] = (rand() / (float)RAND_MAX) / 10.0f;
	}

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	ShaderManager::SetUniform4fv("proj", proj);

	m_Camera = MemNew(MemoryPool::Rendering, Camera);
	Renderer::SetActiveCamera(m_Camera);

	m_Time = 0.0f;
	m_UpdateTime = false;

	m_CameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
	m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_CameraYaw = 0.0f;
	m_CameraPitch = 0.0f;
}

void TestGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);

	if (Input::GetKeyEvent(GLFW_KEY_T) == Input::PRESSED)
	{
		m_UpdateTime = !m_UpdateTime;
	}
	if (m_UpdateTime)
	{
		m_Time += 0.01f;
	}

	float camera_speed = 1.0f;
	if (Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_LEFT_SHIFT) == Input::HELD)
	{
		camera_speed *= 5.0f;
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
		m_CameraYaw -= camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_RIGHT) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_RIGHT) == Input::HELD)
	{
		m_CameraYaw += camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_UP) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_UP) == Input::HELD)
	{
		m_CameraPitch += camera_speed * frame_time.toSeconds();
	}
	if (Input::GetKeyEvent(GLFW_KEY_DOWN) == Input::PRESSED || Input::GetKeyEvent(GLFW_KEY_DOWN) == Input::HELD)
	{
		m_CameraPitch -= camera_speed * frame_time.toSeconds();
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

	for (int i = 0; i < _countof(m_Entities); ++i)
	{
		glm::mat4 world_transform = m_Entities[i]->GetTransform();
		world_transform = glm::rotate(world_transform, m_Speed[i], m_Axis[i]);
		m_Entities[i]->SetTransform(world_transform);
	}
}

void TestGameState::OnExit()
{

}
