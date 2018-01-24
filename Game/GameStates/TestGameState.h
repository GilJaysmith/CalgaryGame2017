#pragma once

#include "Engine/GameStates/GameState.h"

class Entity;

class TestGameState : public GameState
{
public:
	TestGameState();
	~TestGameState();

	virtual void OnEnter() override;
	virtual void OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	float time;
	int rot;
	bool update_rotation;
	bool update_time;

	Entity* m_Entities[1000];
};
