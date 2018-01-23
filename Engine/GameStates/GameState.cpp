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

void GameState::OnUpdate(const Time& frame_time)
{
	m_TotalTimeInState += frame_time;
}

void GameState::OnExit()
{

}