#pragma once

#include "Engine/GameStates/GameState.h"

class DebugCamera;
class Entity;
class Time;

class CityGameState : public GameState
{
public:
	CityGameState();
	~CityGameState();

	virtual void OnEnter() override;
	virtual void OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	DebugCamera* m_Camera;
};
