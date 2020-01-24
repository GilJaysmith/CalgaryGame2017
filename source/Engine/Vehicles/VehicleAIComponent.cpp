#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Vehicles/VehicleAIComponent.h"


Component* VehicleAIComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Entities, VehicleAIComponent)(owner, properties);
}

VehicleAIComponent::~VehicleAIComponent()
{
}

bool VehicleAIComponent::OnMessage(Message* message)
{
	return false;
}

void VehicleAIComponent::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	if (update_pass == UpdatePass::AfterPhysics)
	{
	}
}


VehicleAIComponent::VehicleAIComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
}