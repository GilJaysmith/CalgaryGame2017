#pragma once

#include "Engine/Entities/EntityHandle.h"
#include "Engine/GameStates/GameState.h"
#include "Engine/GameStates/Time.h"


class Camera;

class RaceGameState : public GameState
{
public:
	RaceGameState();
	~RaceGameState();

	virtual void OnEnter() override;
	virtual bool OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	EntityHandle m_PlayerCar;
	std::vector<EntityHandle> m_Cars;
	std::vector<EntityHandle> m_Checkpoints;
	std::vector<EntityHandle> m_CarsAsTheyFinish;

	enum RACESTATE
	{
		NONE,
		COUNTDOWN,
		RACE,
		PLAYER_FINISHED,
		RESULTS,
		EXIT
	};

	RACESTATE m_RaceState;
	Time m_TimeInCurrentState;

	int m_CurrentCountdownThreshold;

	void OnChangeState(RACESTATE new_state);
	void UpdateCountdownState();
	void UpdateRaceState();
	void UpdatePlayerFinishedState();
	void UpdateResultsState();
};
