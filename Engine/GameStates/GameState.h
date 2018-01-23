#pragma once

#include "Engine/GameStates/Time.h"

class GameState
{
public:
	GameState();
	~GameState();

	virtual void OnEnter();
	virtual void OnUpdate(const Time& frame_time);
	virtual void OnExit();

protected:
	Time m_TotalTimeInState;
};
