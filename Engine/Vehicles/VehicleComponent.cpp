#include "Engine/Pch.h"

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"
#include "Engine/Vehicles/VehicleComponent.h"

#include <sdks/libyaml/include/yaml-cpp/yaml.h>


Component* VehicleComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Vehicles, VehicleComponent)(owner, properties);
}

VehicleComponent::VehicleComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{

}

VehicleComponent::~VehicleComponent()
{

}

bool VehicleComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Vehicle)
	{
		return true;
	}
	return false;
}

void VehicleComponent::OnUpdate(const Time& elapsed_time)
{

}