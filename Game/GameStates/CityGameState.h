#pragma once

#include "Engine/Entities/EntityHandle.h"
#include "Engine/GameStates/GameState.h"

class Camera;
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
	EntityHandle m_Car;
};
