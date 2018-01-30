#pragma once

#include "Engine/GameStates/GameState.h"

class SplashScreenGameState : public GameState
{
public:
	SplashScreenGameState();
	~SplashScreenGameState();

	virtual void OnEnter() override;
	virtual void OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:

};