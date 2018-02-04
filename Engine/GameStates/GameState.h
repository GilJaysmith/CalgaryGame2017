#pragma once

#include "Engine/GameStates/Time.h"

class Time;

class GameState
{
public:
	GameState();
	~GameState();

	virtual void OnEnter() = 0;
	virtual bool OnUpdate(const Time& frame_time) = 0;
	virtual void OnExit() = 0;

protected:
	Time m_TotalTimeInState;
};
