#pragma once

#include "Engine/GameStates/GameState.h"

class Camera;
class Entity;
class Time;

class CityGameState : public GameState
{
public:
	CityGameState();
	~CityGameState();

	virtual void OnEnter() override;
	virtual bool OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	Camera* m_Camera;
	Entity* m_Car;
};
