#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Vehicles/VehicleInputComponent.h"
#include "Engine/Vehicles/VehicleInputHandler.h"
#include "Engine/Vehicles/VehicleMessages.h"


Component* VehicleInputComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Entities, VehicleInputComponent)(owner, properties);
}

VehicleInputComponent::~VehicleInputComponent()
{
	MemDelete(m_InputHandler);
}

bool VehicleInputComponent::OnMessage(Message* message)
{
	return false;
}

void VehicleInputComponent::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	if (update_pass == UpdatePass::BeforePhysics)
	{
		// Transfer input handler details to vehicle component.
		if (m_InputHandler)
		{
			Message_VehicleSetInputs mvsi(m_InputHandler->Steer(), m_InputHandler->Acceleration(), m_InputHandler->Brake(), m_InputHandler->Handbrake(), m_InputHandler->Jump());
			m_Entity->OnMessage(&mvsi);
		}
	}
}

VehicleInputComponent::VehicleInputComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
	int player_idx = properties["player"].as<int>();
	m_InputHandler = MemNew(MemoryPool::Vehicles, VehicleInputHandler)(player_idx);
}