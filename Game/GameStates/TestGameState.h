#pragma once

#include "Engine/GameStates/GameState.h"

class Renderable;

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

	Renderable* renderable;
	Renderable* renderable2;

	unsigned int shader_program;
};
