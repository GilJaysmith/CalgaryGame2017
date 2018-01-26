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
	float m_Time;
	bool m_UpdateTime;

	Camera* m_Camera;
	glm::vec3 m_CameraPos;
	glm::vec3 m_CameraFront;
	glm::vec3 m_CameraUp;
	float m_CameraYaw;
	float m_CameraPitch;

	Entity* m_Entities[1000];
};
