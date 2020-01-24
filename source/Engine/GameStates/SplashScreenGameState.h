#pragma once

#include "Engine/GameStates/GameState.h"
#include "Engine/GameStates/Time.h"

class SplashScreenGameState : public GameState
{
public:
	SplashScreenGameState(const std::string& texture, const Time& duration);
	~SplashScreenGameState();

	virtual void OnEnter() override;
	virtual bool OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:

	GLuint m_Texture;
	Time m_Duration;
	Time m_TimeInState;
	GLuint m_Sprite;
};