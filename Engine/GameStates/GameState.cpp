#include "Engine/Pch.h"

#include "Engine/GameStates/GameState.h"


GameState::GameState()
{

}

GameState::~GameState()
{

}

void GameState::OnEnter()
{

}

bool GameState::OnUpdate(const Time& frame_time)
{
	m_TotalTimeInState += frame_time;
	return true;
}

void GameState::OnExit()
{

}