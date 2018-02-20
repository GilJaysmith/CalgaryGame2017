#pragma once

#include "Engine/GameStates/GameState.h"

class DebugCamera;
class Entity;
class Time;

class TestGameState : public GameState
{
public:
	TestGameState();
	~TestGameState();

	virtual void OnEnter() override;
	virtual bool OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	Camera* m_Camera;
	float m_Time;
};
