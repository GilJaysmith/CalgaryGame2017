#include "Engine/Pch.h"

#include "Engine/GameStates/GameStateManager.h"
#include "Engine/GameStates/GameState.h"
#include "Engine/GameStates/Time.h"


namespace GameStateManager
{
	GameState* s_CurrentState = nullptr;
	GameState* s_ForceNextState = nullptr;
	bool s_ForceQuit = false;
	std::vector<GameState*> s_StateQueue;

	void Initialize()
	{
		s_CurrentState = s_ForceNextState = nullptr;
	}

	void Terminate()
	{
	}

	void RequestGameState(GameState* new_game_state)
	{
		s_ForceNextState = new_game_state;
	}

	void QueueGameState(GameState* game_state)
	{
		s_StateQueue.push_back(game_state);
	}

	void ForceQuit()
	{
		s_ForceQuit = true;
		s_StateQueue.clear();
	}

	bool Update(const Time& frame_time)
	{
		bool go_to_next_state = true;
		if (s_CurrentState)
		{
			go_to_next_state = !s_CurrentState->OnUpdate(frame_time) || s_ForceNextState != nullptr || s_ForceQuit;
		}
		if (go_to_next_state)
		{
			GameState* next_state = nullptr;
			if (s_ForceNextState)
			{
				next_state = s_ForceNextState;
				s_ForceNextState = nullptr;
			}
			else if (s_StateQueue.size())
			{
				next_state = s_StateQueue[0];
				s_StateQueue.erase(s_StateQueue.begin());
			}
			if (s_CurrentState)
			{
				s_CurrentState->OnExit();
			}
			s_CurrentState = next_state;
			if (s_CurrentState)
			{
				s_CurrentState->OnEnter();
			}
		}
		return s_CurrentState != nullptr;
	}
}
