#include "Game/Pch.h"

#include "Engine/Cameras/DebugCamera.h"
#include "Engine/Entities/Entity.h"
#include "Engine/imGUI/imgui.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"

#include "Game/GameStates/CityGameState.h"
#include "Game/GIS/GISManager.h"

CityGameState::CityGameState()
{
}

CityGameState::~CityGameState()
{

}

Entity* car = nullptr;

void CityGameState::OnEnter()
{
	Entity* m_GroundPlane = Entity::CreateEntity("sea", glm::mat4());
	for (unsigned int x = 0; x < 10; x++)
	{
		for (unsigned int z = 0; z < 10; z++)
		{
			glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x * 3, 40.0f, z * 6));
			car = Entity::CreateEntity("Porsche_911_GT2", transform);
		}
	}

//	GIS::LoadCity("Vancouver");

	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);
}

bool CityGameState::OnUpdate(const Time& frame_time)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(400, 100));
	ImGui::Begin("Car debug");
	ImGui::Text("Car altitude is: %f and here's a rand %d", car->GetTransform()[3][1], rand());
	ImGui::End();

	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	GIS::Render();
	
	return true;
}

void CityGameState::OnExit()
{
	MemDelete(m_Camera);

	GIS::UnloadCity();
}
