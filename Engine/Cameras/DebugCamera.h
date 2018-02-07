#pragma once

#include "Engine/Cameras/Camera.h"

class Time;

class DebugCamera : public Camera
{
public:
	DebugCamera();
	~DebugCamera();

	virtual void Update(const Time& frame_time) override;

	virtual glm::vec3 GetPosition() const override { return m_CameraPos; }

protected:
	glm::vec3 m_CameraPos;
	glm::vec3 m_CameraFront;
	glm::vec3 m_CameraUp;
	float m_CameraYaw;
	float m_CameraPitch;
};
