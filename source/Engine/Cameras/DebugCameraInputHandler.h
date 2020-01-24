#pragma once

#include "Engine/Input/InputHandler.h"

class DebugCameraInputHandler : public InputHandler
{
public:
	DebugCameraInputHandler();
	virtual ~DebugCameraInputHandler() override;

	virtual void Update() override;

	bool Fast() const { return m_Fast; }
	bool SuperFast() const { return m_SuperFast; }
	bool Forward() const { return m_Forward; }
	bool Backward() const { return m_Backward; }
	bool Left() const { return m_Left; }
	bool Right() const { return m_Right; }
	bool Up() const { return m_Up; }
	bool Down() const { return m_Down; }
	bool YawLeft() const { return m_YawLeft; }
	bool YawRight() const { return m_YawRight; }
	bool PitchUp() const { return m_PitchUp; }
	bool PitchDown() const { return m_PitchDown; }

protected:
	bool m_Fast;
	bool m_SuperFast;
	bool m_Forward;
	bool m_Backward;
	bool m_Left;
	bool m_Right;
	bool m_Up;
	bool m_Down;
	bool m_YawLeft;
	bool m_YawRight;
	bool m_PitchUp;
	bool m_PitchDown;
};