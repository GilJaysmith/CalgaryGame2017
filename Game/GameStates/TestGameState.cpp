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

	time = 0.0f;
	rot = 0;

	update_time = false;
	update_rotation = false;
}

void TestGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);

	if (update_time)
	{
		time += 0.01f;
	}

	if (update_rotation)
	{
		rot = (rot + 1) % 360;
	}

	glm::vec3 camera_pos(cos(time), 2.0f, 4.0f + sin(time));
	glm::mat4 view_matrix = glm::lookAt(
		camera_pos,
		glm::vec3(cos(time), 0.0f, sin(time)),
		glm::vec3(0.0f, 1.0f, 0.0f));
	m_Camera->SetViewMatrix(view_matrix);

	for (int i = 0; i < _countof(m_Entities); ++i)
	{
		glm::mat4 world_transform = m_Entities[i]->GetTransform();
		world_transform = glm::rotate(world_transform, m_Speed[i], m_Axis[i]);
		m_Entities[i]->SetTransform(world_transform);
	}

	if (Input::GetKeyEvent(GLFW_KEY_T) == Input::PRESSED)
	{
		update_time = !update_time;
	}

	if (Input::GetKeyEvent(GLFW_KEY_R) == Input::PRESSED)
	{
		update_rotation = !update_rotation;
	}
}

void TestGameState::OnExit()
{

}
