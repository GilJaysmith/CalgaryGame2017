#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Audio/Music.h"
#include "Engine/Cameras/DebugCamera.h"
#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/DebugPanels/imgui/imgui.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Health/DamageMessages.h"
#include "Engine/Input/Input.h"
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
	Entity* m_GroundPlane = Entity::CreateEntity("sea", glm::mat4());

	//for (unsigned int x = 0; x < 10; x++)
	//{
	//	for (unsigned int z = 0; z < 10; z++)
	//	{
	//		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x * 3, 2.0f, z * 6));
	//		Entity::CreateEntity("Porsche 911 GT2", transform);
	//	}
	//}

	if (false)
	{
//		if (!GIS::LoadCityFromCooked("Vancouver"))
		{
			GIS::LoadCityFromSource("Vancouver");
		}
	}

	m_Car = Entity::CreateEntity("player_0_car", glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 0.0f)));
	m_Camera = MemNew(MemoryPool::Vehicles, VehicleFollowCamera)(m_Car.GetEntity(), 0);
//	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);

	Entity::CreateEntity("Porsche 911 GT2", glm::translate(glm::mat4(), glm::vec3(10.0f, 1.0f, 0.0f)));
	Entity::CreateEntity("Porsche 911 GT2", glm::translate(glm::mat4(), glm::vec3(20.0f, 1.0f, 0.0f)));
	Entity::CreateEntity("Porsche 911 GT2", glm::translate(glm::mat4(), glm::vec3(30.0f, 1.0f, 0.0f)));
	Entity::CreateEntity("Porsche 911 GT2", glm::translate(glm::mat4(), glm::vec3(40.0f, 1.0f, 0.0f)));
	Entity::CreateEntity("Porsche 911 GT2", glm::translate(glm::mat4(), glm::vec3(50.0f, 1.0f, 0.0f)));

	Entity::CreateEntity("trigger_sphere", glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 10.0f)));
	Entity::CreateEntity("trigger_box", glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 30.0f)));
	Entity::CreateEntity("trigger_sphere", glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 50.0f)));

	Music::LoadPlaylist("Data/Audio/Music/Playlist.yaml");
//	Music::StartPlaying();

	//std::vector<glm::vec3> points = { 
	//	glm::vec3(10.0f, 10.0f, 10.0f), 
	//	glm::vec3(-10.0f, 10.0f, 10.0f), 
	//	glm::vec3(-10.0f, 10.0f, -10.0f),
	//	glm::vec3(10.0f, 10.0f, -10.0f), 
	//	glm::vec3(10.0f, 10.0f, 10.0f) };
	//DebugDraw::IDebugObject* lines = DebugDraw::DebugLines("TEST", points);
	//lines->SetColour(glm::vec3(0.0f, 1.0f, 0.0f));

	//glm::mat4 grid_transform;
	//grid_transform = glm::translate(grid_transform, glm::vec3(5.0f, 5.0f, 5.0f));
	//DebugDraw::IDebugObject* grid = DebugDraw::DebugGrid("TEST", 1.0f, 10, 10);
	//grid->SetExternalTransform(&m_DebugDrawTransform);
	//grid->SetColour(glm::vec3(1.0f, 1.0f, 0.0f));
	//grid->SetTransform(grid_transform);

	//DebugDraw::IDebugObject* sphere = DebugDraw::DebugSphere("TEST", 1.0f);
	//sphere->SetExternalTransform(&m_DebugDrawTransform);
	//sphere->SetColour(glm::vec3(0.0f, 0.0f, 1.0f));
	//glm::mat4 sphere_transform;
	//sphere_transform = glm::translate(sphere_transform, glm::vec3(5.0f, 5.0f, 5.0f));
	//sphere->SetTransform(sphere_transform);
	
	//glm::mat4 box_transform;
	//box_transform = glm::translate(box_transform, glm::vec3(5.0f, 5.0f, 5.0f));
	//box_transform = glm::rotate(box_transform, glm::pi<float>() / 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//DebugDraw::DebugBox("TEST", glm::vec3(2.0f, 3.0f, 4.0f));

	//DebugDraw::IDebugObject* line = DebugDraw::DebugLine("TEST", glm::vec3(0.0f, 10.0f, 0.0f));
	//line->SetExternalTransform(&m_DebugDrawTransform);
	//line->SetColour(glm::vec3(1.0f, 0.0f, 0.0f));

	//DebugDraw::DebugCrosshair("TEST");

	//glm::mat4 arrow_transform;
	//arrow_transform = glm::translate(arrow_transform, glm::vec3(-2.0f, 10.0f, -2.0f));
	//arrow_transform = glm::rotate(arrow_transform, glm::pi<float>() / 4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//DebugDraw::IDebugObject* arrow = DebugDraw::DebugArrow("TEST");
	//arrow->SetTransform(arrow_transform);
}

char buffer[60];

bool CityGameState::OnUpdate(const Time& frame_time)
{
	float ms = m_TotalTimeInState.toSeconds();
	m_DebugDrawTransform = glm::translate(glm::mat4(), glm::vec3(0.0f, sin(ms), 0.0f));

	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	if (m_Car.IsValid())
	{
		unsigned int car_health = 0;
		Message_DamageGetHealth dgh;
		m_Car->OnMessage(&dgh);
		car_health = dgh.m_Health;

		ImGui::Begin("Health test");
		ImGui::SetWindowFontScale(2.0f);
		ImGui::Text("Car health: %d", car_health);
		if (ImGui::Button("Damage car"))
		{
			Message_DamageTakeDamage dtd(10);
			m_Car->OnMessage(&dtd);
		}
		ImGui::End();
	}

	return true;
}

void CityGameState::OnExit()
{
	MemDelete(m_Camera);

	GIS::UnloadCity();

	Music::StopPlaying();
}
