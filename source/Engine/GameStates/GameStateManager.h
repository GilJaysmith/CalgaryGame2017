#pragma once

class GameState;
class Time;

#include "Engine/GameStates/Time.h"

namespace GameStateManager
{
	void Initialize();
	void Terminate();

	void RequestGameState(GameState* new_game_state);
	void QueueGameState(GameState* game_state);
	void ForceQuit();

	bool Update(const Time& frame_time);
}
