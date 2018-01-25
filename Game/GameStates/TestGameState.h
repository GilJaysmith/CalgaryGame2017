#pragma once

#include "Engine/GameStates/GameState.h"

class Camera;
class Entity;
class Time;

class TestGameState : public GameState
{
public:
	TestGameState();
	~TestGameState();

	virtual void OnEnter() override;
	virtual void OnUpdate(const Time& frame_time) override;
	virtual void OnExit() override;

protected:
	float time;
	int rot;
	bool update_rotation;
	bool update_time;

	Camera* m_Camera;
	Entity* m_Entities[1000];
	glm::vec3 m_Axis[1000];
	float m_Speed[1000];
};
