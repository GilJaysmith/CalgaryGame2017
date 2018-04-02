#pragma once

#include "Engine/Entities/EntityHandle.h"
#include "Engine/GameStates/GameState.h"

class Camera;
class Time;

class RaceGameState : public GameState
{
public:
	RaceGameState();
	~RaceGameState();

	virtual void OnEnter() override;
	virtual bool OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	Camera* m_Camera;
	std::vector<EntityHandle> m_Cars;
	std::vector<EntityHandle> m_Checkpoints;
};
