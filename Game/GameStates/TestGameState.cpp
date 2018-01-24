#include "Game/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Input/Input.h"
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
		world_transform = glm::scale(world_transform, glm::vec3(0.5f, 0.5f, 0.5f));
		world_transform = glm::translate(world_transform, glm::vec3(-5.0 + i / 100, -15.0f + i % 100, 0.0f));
		m_Entities[i]->SetTransform(world_transform);
	}

	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.5f, 10.0f);

	ShaderManager::SetUniform4fv("view", view);
	ShaderManager::SetUniform4fv("proj", proj);

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

	for (int i = 0; i < _countof(m_Entities); ++i)
	{
		glm::mat4 world_transform = m_Entities[i]->GetTransform();
		world_transform = glm::translate(world_transform, glm::vec3(0.0f, 0.01f, 0.0f));
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
