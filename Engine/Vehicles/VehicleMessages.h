#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"


namespace VehicleMessageSubtype
{
	enum TYPE
	{
		SetInputs,
		COUNT
	};
}


struct Message_VehicleSetInputs : public Message
{
	Message_VehicleSetInputs(float steer, float acceleration, float brake, bool handbrake)
		: Message(MessageType::Vehicle, VehicleMessageSubtype::SetInputs)
		, m_Steer(steer)
		, m_Acceleration(acceleration)
		, m_Brake(brake)
		, m_Handbrake(handbrake)
	{}

	float m_Steer;
	float m_Acceleration;
	float m_Brake;
	bool m_Handbrake;
};