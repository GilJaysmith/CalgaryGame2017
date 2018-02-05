#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Cameras/DebugCamera.h"
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
	m_GroundPlane = Entity::CreateEntity("sea", glm::mat4());

	for (int i = 0; i < _countof(m_Entities); ++i)
	{
		glm::mat4 world_transform;
		float x = -20.0f + i % 40;
		float y = 5.0f + 5.f * (rand() / (float)RAND_MAX);
		float z = 5.0f - i / 40;
		world_transform = glm::translate(world_transform, glm::vec3(x, y, z));
		m_Entities[i] = Entity::CreateEntity("cube", world_transform);
		//m_Entities[i]->SetTransform(world_transform);
	}

	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);
}

bool TestGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	if (Input::GetKeyEvent(GLFW_KEY_M) == Input::PRESSED)
	{
		Audio::PlaySound("a2002011001-e02.wav");
	}

	return true;
}

void TestGameState::OnExit()
{
	MemDelete(m_Camera);
	for (auto it : m_Entities)
	{
		EntityManager::DestroyEntity(it);
	}

	EntityManager::DestroyEntity(m_GroundPlane);
}
