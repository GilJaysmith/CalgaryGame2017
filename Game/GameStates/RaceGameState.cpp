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

#include "Game/GameStates/RaceGameState.h"
#include "Game/Race/RaceManager.h"


RaceGameState::RaceGameState()
{
}

RaceGameState::~RaceGameState()
{

}

void RaceGameState::OnEnter()
{
	Entity* m_GroundPlane = Entity::CreateEntity("floor", glm::mat4());

	RaceManager::InitializeRace("race1");

	for (unsigned int i = 0; i < RaceManager::GetNumRacers() - 1; ++i)
	{
		EntityHandle car = Entity::CreateEntity("Porsche 911 GT2");
		std::stringstream str;
		str << "AI car " << i;
		car->SetName(str.str());
		RaceManager::RegisterCar(car);
	}

	EntityHandle player_car = Entity::CreateEntity("player_0_car");
	player_car->SetName("Player");
	RaceManager::RegisterCar(player_car);
	m_Camera = MemNew(MemoryPool::Rendering, VehicleFollowCamera)(player_car, 0);
	Renderer::SetActiveCamera(m_Camera);

	Music::LoadPlaylist("Data/Audio/Music/Playlist.yaml");
	Music::StartPlaying();
}


bool RaceGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	std::vector<RaceManager::CarRaceStatus> race_status = RaceManager::GetRaceStatus();
	ImGui::Begin("Race positions");
	ImGui::SetWindowFontScale(1.5f);
	for (auto position = 0; position < race_status.size(); ++position)
	{
		const auto& rs = race_status[position];
		std::string status = rs.m_NextCheckpoint.IsValid() ? "RACING" : "FINISHED!";
		ImGui::Text("%d: %s %s", position + 1, rs.m_Car->GetName().c_str(), status.c_str());
	}
	ImGui::End();

	return true;
}

void RaceGameState::OnExit()
{
	MemDelete(m_Camera);

	Music::StopPlaying();
}
