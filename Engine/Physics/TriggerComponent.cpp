#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Physics/CollisionMessages.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsUtils.h"
#include "Engine/Physics/TriggerComponent.h"

#include "sdks/PhysX/PhysX/Include/PxPhysicsAPI.h"

Component* TriggerComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Physics, TriggerComponent)(owner, properties);
}

TriggerComponent::~TriggerComponent()
{
	if (m_Actor)
	{
		m_Actor->release();
	}
}

bool TriggerComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Collision)
	{
		switch (message->GetMessageSubtype())
		{
		case CollisionMessageSubtype::Touch:
		{
			Message_CollisionTouch* mct = static_cast<Message_CollisionTouch*>(message);
			physx::PxActor* my_actor = mct->m_MyActor;
			physx::PxActor* other_actor = mct->m_OtherActor;

			Entity* other_entity = static_cast<Entity*>(other_actor->userData);
			if (other_entity)
			{
			}

			return true;
		}
		break;
		}
	}
	return false;
}

TriggerComponent::TriggerComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
	, m_Actor(nullptr)
{
	const glm::mat4& transform = owner->GetTransform();
	glm::vec3 position = transform[3];
	m_Actor = Physics::GetPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z)));

	std::vector<physx::PxShape*> shapes;
	const YAML::Node& shapes_node = properties["shapes"];
	physx::PxMaterial* trigger_material = Physics::GetMaterial("trigger");
	for (const auto& node : shapes_node)
	{
		physx::PxShape* shape = CreatePhysicsShape(node, *trigger_material, physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE | physx::PxShapeFlag::eVISUALIZATION));
		shapes.push_back(shape);
		m_Actor->attachShape(*shape);
	}

	std::vector<unsigned int> filter = properties["simfilter"].as<std::vector<unsigned int>>();
	assert(filter.size() == 4);
	physx::PxFilterData filter_data(filter[0], filter[1], filter[2], filter[3]);
	for (auto shape : shapes)
	{
		shape->setSimulationFilterData(filter_data);
	}

	m_Actor->userData = static_cast<void*>(m_Entity);
	m_Actor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
	Physics::GetScene()->addActor(*m_Actor);
}
