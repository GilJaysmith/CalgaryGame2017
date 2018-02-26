#include "Game/Pch.h"

#include "Engine/Cameras/DebugCamera.h"
#include "Engine/Entities/Entity.h"
#include "Engine/DebugPanels/imgui/imgui.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Vehicles/VehicleFollowCamera.h"

#include "Game/GameStates/CityGameState.h"
#include "Game/GIS/GISManager.h"

CityGameState::CityGameState()
{
}

CityGameState::~CityGameState()
{

}

void CityGameState::OnEnter()
{
	Entity* m_GroundPlane = Entity::CreateEntity("floor", glm::mat4());

	//for (unsigned int x = 0; x < 10; x++)
	//{
	//	for (unsigned int z = 0; z < 10; z++)
	//	{
	//		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x * 3, 40.0f, z * 6));
	//		Entity::CreateEntity("Porsche_911_GT2", transform);
	//	}
	//}

	if (!GIS::LoadCityFromCooked("Vancouver"))
	{
		GIS::LoadCityFromSource("Vancouver");
	}

	Entity* car = Entity::CreateEntity("player_0_car", glm::translate(glm::mat4(), glm::vec3(0.0f, 120.0f, 0.0f)));
	m_Camera = MemNew(MemoryPool::Vehicles, VehicleFollowCamera)(car, 0);

	//m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);
}

char buffer[60];

bool CityGameState::OnUpdate(const Time& frame_time)
{
//	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSizeConstraints(ImVec2(400, 100), ImVec2(800, 600));
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("Fun debug");
//	ImGui::Text("Car altitude is: %fm and here's a random number: %d", car->GetTransform()[3][1], rand());
	if (ImGui::Button("Click me to toggle physics"))
	{
		Physics::Pause(!Physics::IsPaused());
	}
	ImGui::InputText("<- Type here", buffer, sizeof(buffer));
	ImGui::LabelText("<- What you typed", buffer);
	ImGui::End();

	ImGui::Begin("Another window");
	ImGui::Text("More text!");
	ImGui::Text("Every piece of text you'd want!");
	ImGui::End();

	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

//	GIS::Render();
	
	return true;
}

void CityGameState::OnExit()
{
	MemDelete(m_Camera);

	GIS::UnloadCity();
}
