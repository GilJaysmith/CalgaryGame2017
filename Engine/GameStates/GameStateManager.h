#pragma once

class GameState;

#include "Engine/GameStates/Time.h"

namespace GameStateManager
{
	void Initialize();
	void Terminate();

	void RequestGameState(GameState* new_game_state);

	bool Update(const Time& frame_time);
}
