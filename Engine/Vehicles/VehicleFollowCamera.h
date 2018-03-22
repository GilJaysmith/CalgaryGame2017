#pragma once

#include "Engine/Cameras/Camera.h"
#include "Engine/Entities/EntityHandle.h"

class VehicleFollowCameraInputHandler;

class VehicleFollowCamera : public Camera
{
public:
	VehicleFollowCamera(Entity* followed, int player_idx);
	~VehicleFollowCamera();

	virtual void Update(const Time& frame_time) override;

	virtual glm::vec3 GetPosition() const override { return m_CameraPos; }
	virtual glm::vec3 GetForwardVector() const override { return m_CameraFront; }

protected:
	EntityHandle m_FollowedEntity;
	VehicleFollowCameraInputHandler* m_InputHandler;

	glm::vec3 m_CameraPos;
	glm::vec3 m_CameraFront;
	glm::vec3 m_CameraUp;
	float m_CameraYaw;
	float m_CameraPitch;
};
