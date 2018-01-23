#include "Engine/Pch.h"

#include "Engine/GameStates/GameStateManager.h"
#include "Engine/GameStates/GameState.h"
#include "Engine/GameStates/Time.h"


namespace GameStateManager
{
	GameState* s_CurrentState = nullptr;
	GameState* s_NextState = nullptr;

	void Initialize()
	{
		s_CurrentState = s_NextState = nullptr;
	}

	void Terminate()
	{
	}

	void RequestGameState(GameState* new_game_state)
	{
		s_NextState = new_game_state;
	}

	bool Update(const Time& frame_time)
	{
		if (s_CurrentState)
		{
			s_CurrentState->OnUpdate(frame_time);
		}
		if (s_NextState != s_CurrentState)
		{
			if (s_CurrentState)
			{
				s_CurrentState->OnExit();
			}
			s_CurrentState = s_NextState;
			if (s_CurrentState)
			{
				s_CurrentState->OnEnter();
			}
		}
		return s_CurrentState != nullptr;
	}
}
