#pragma once

#include "Engine/Input/InputHandler.h"

class VehicleFollowCameraInputHandler : public InputHandler
{
public:
	VehicleFollowCameraInputHandler(int player_idx);
	virtual ~VehicleFollowCameraInputHandler() override;

	virtual void Update();

public:
	float Yaw() const { return m_Yaw; }
	float Pitch() const { return m_Pitch; }

protected:
	int m_PlayerIdx;
	float m_Yaw;
	float m_Pitch;
};
