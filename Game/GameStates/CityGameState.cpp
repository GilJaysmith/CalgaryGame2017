#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Cameras/DebugCamera.h"
#include "Engine/Entities/Entity.h"
#include "Engine/DebugPanels/imgui/imgui.h"
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
	//		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x * 3, 40.0f, z * 6));
	//		Entity::CreateEntity("Porsche_911_GT2", transform);
	//	}
	//}

	if (false)
	{
//		if (!GIS::LoadCityFromCooked("Vancouver"))
		{
			GIS::LoadCityFromSource("Vancouver");
		}
	}

//	Entity* car = Entity::CreateEntity("player_0_car", glm::translate(glm::mat4(), glm::vec3(0.0f, 10.0f, 0.0f)));
	//m_Camera = MemNew(MemoryPool::Vehicles, VehicleFollowCamera)(car, 0);
	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);
}

char buffer[60];

bool CityGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);
	
	if (Input::JustPressed(GLFW_KEY_B))
	{
		Audio::PlaySound("a2002011001-e02.wav", glm::vec3(0.0f, 0.0f, 0.0f));
	}

	return true;
}

void CityGameState::OnExit()
{
	MemDelete(m_Camera);

	GIS::UnloadCity();
}
