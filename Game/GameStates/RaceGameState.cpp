#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Audio/Music.h"
#include "Engine/Cameras/CameraManager.h"
#include "Engine/Cameras/DebugCamera.h"
#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/DebugPanels/imgui/imgui.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityHandle.h"
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

	m_PlayerCar = Entity::CreateEntity("player_0_car");
	m_PlayerCar->SetName("Player");
	RaceManager::RegisterCar(m_PlayerCar);
	Camera* camera = MemNew(MemoryPool::Rendering, VehicleFollowCamera)(m_PlayerCar, 0);
	CameraManager::RegisterCamera(camera);
	Renderer::SetActiveCamera(camera);

	m_RaceState = NONE;

	Music::LoadPlaylist("Data/Audio/Music/Playlist.yaml");
}


bool RaceGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);

	std::string race_status_description = "Race Monitor";
	switch (m_RaceState)
	{
		case COUNTDOWN: race_status_description = "Countdown!"; break;
		case RACE: race_status_description = "Race!"; break;
		case PLAYER_FINISHED: race_status_description = "Player Finished!"; break;
		case RESULTS: race_status_description = "Results!"; break;
	}
	ImGui::Begin(race_status_description.c_str());
	ImGui::SetWindowFontScale(1.5f);

	std::vector<RaceManager::CarRaceStatus> race_status = RaceManager::GetRaceStatus();
	for (auto position = 0; position < race_status.size(); ++position)
	{
		const auto& rs = race_status[position];
		std::string status;
		if (rs.m_NextCheckpoint.IsValid())
		{
			status = "RACING";
		}
		else
		{
			if (std::find(m_CarsAsTheyFinish.begin(), m_CarsAsTheyFinish.end(), rs.m_Car) == m_CarsAsTheyFinish.end())
			{
				m_CarsAsTheyFinish.push_back(rs.m_Car);
				if (rs.m_Car == m_PlayerCar)
				{
					OnChangeState(RACESTATE::PLAYER_FINISHED);
				}
			}
			status = "FINISHED!";
		}
		ImGui::Text("%d: %s %s", position + 1, rs.m_Car->GetName().c_str(), status.c_str());
	}
	ImGui::End();

	// Let's do actions specific to this state here.
	m_TimeInCurrentState += frame_time;

	switch (m_RaceState)
	{
	case NONE:
		OnChangeState(RACESTATE::COUNTDOWN);
		break;
	case COUNTDOWN:
		UpdateCountdownState();
		break;
	case RACE:
		UpdateRaceState();
		break;
	case PLAYER_FINISHED:
		UpdatePlayerFinishedState();
		break;
	case RESULTS:
		UpdateResultsState();
		break;
	}

	return (m_RaceState != RACESTATE::EXIT);
}

void RaceGameState::OnExit()
{
	Music::StopPlaying();
}

void RaceGameState::OnChangeState(RACESTATE new_state)
{
	m_RaceState = new_state;
	m_TimeInCurrentState = Time::fromMilliseconds(0.0f);

	switch (new_state)
	{
	case COUNTDOWN:
		// TODO: Message all cars to disable input.
		m_CurrentCountdownThreshold = 3;
		break;
	case RACE:
		// TODO: Message all cars to enable input.
		Music::StartPlaying();
		break;
	case PLAYER_FINISHED:
		// Start a two-second countdown.
		Audio::PlaySound("commentary\\you_finished.wav");
		break;
	case RESULTS:
		// Show the final race order.
		Music::StopPlaying();
		break;
	}
}

void RaceGameState::UpdateCountdownState()
{
	float seconds_remaining = 5.0f - m_TimeInCurrentState.toSeconds();
	if (seconds_remaining <= static_cast<float>(m_CurrentCountdownThreshold))
	{
		switch (m_CurrentCountdownThreshold)
		{
		case 3:
			Audio::PlaySound("commentary\\countdown_three.wav");
			break;
		case 2:
			Audio::PlaySound("commentary\\countdown_two.wav");
			break;
		case 1:
			Audio::PlaySound("commentary\\countdown_one.wav");
			break;
		case 0:
			Audio::PlaySound("commentary\\countdown_go.wav");
			OnChangeState(RACESTATE::RACE);
			break;
		}
		--m_CurrentCountdownThreshold;
	}
}

void RaceGameState::UpdateRaceState()
{

}

void RaceGameState::UpdatePlayerFinishedState()
{
	if (m_TimeInCurrentState.toSeconds() >= 5.0f)
	{
		OnChangeState(RACESTATE::RESULTS);
	}
}

void RaceGameState::UpdateResultsState()
{
	if (m_TimeInCurrentState.toSeconds() >= 5.0f)
	{
		OnChangeState(RACESTATE::EXIT);
	}
}
