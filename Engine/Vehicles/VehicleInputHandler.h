#pragma once

#include "Engine/Input/InputHandler.h"

class VehicleInputHandler : public InputHandler
{
public:
	VehicleInputHandler(int player_idx);
	virtual ~VehicleInputHandler();

public:
	virtual void Update() override;

public:
	float Acceleration() const { return m_Acceleration; }
	float Brake() const { return m_Brake; }
	bool Handbrake() const { return m_Handbrake; }
	float Steer() const { return m_Steer; }
	bool Jump() const { return m_Jump; }
	bool ResetOrientation() const { return m_ResetOrientation; }

protected:
	int m_PlayerIdx;
	float m_Acceleration;
	float m_Brake;
	bool m_Handbrake;
	float m_Steer;
	bool m_Jump;
	bool m_ResetOrientation;
};
