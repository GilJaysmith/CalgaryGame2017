#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Physics/PhysicsMessages.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"
#include "sdks/PhysX/PhysX/Include/PxPhysicsAPI.h"
#include "sdks/PhysX/PxShared/include/foundation/PxFoundation.h"


Component* PhysicsComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Rendering, PhysicsComponent)(owner, properties);
}

PhysicsComponent::~PhysicsComponent()
{
	if (m_Actor)
	{
		m_Actor->release();
	}
	if (m_StaticActor)
	{
		m_StaticActor->release();
	}
}

bool PhysicsComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Physics)
	{
		switch (message->GetMessageSubtype())
		{
			case PhysicsMessageSubtype::SetLinearVelocity:
			{
				Message_PhysicsSetLinearVelocity* pslv = dynamic_cast<Message_PhysicsSetLinearVelocity*>(message);
				if (m_Actor)
				{
					m_Actor->setLinearVelocity(physx::PxVec3(pslv->m_LinearVelocity.x, pslv->m_LinearVelocity.y, pslv->m_LinearVelocity.z));
				}
				return true;
			}
			
			case PhysicsMessageSubtype::SetAngularVelocity:
			{
				Message_PhysicsSetAngularVelocity* psav = dynamic_cast<Message_PhysicsSetAngularVelocity*>(message);
				if (m_Actor)
				{
					m_Actor->setAngularVelocity(physx::PxVec3(psav->m_AngularVelocity.x, psav->m_AngularVelocity.y, psav->m_AngularVelocity.z));
				}
				return true;
			}

			case PhysicsMessageSubtype::DisableGravity:
			{
				Message_PhysicsDisableGravity* pdg = dynamic_cast<Message_PhysicsDisableGravity*>(message);
				if (m_Actor)
				{
					m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, pdg->m_DisableGravity);
				}
				return true;
			}
		}
	}
	return false;
}

void PhysicsComponent::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	switch (update_pass)
	{
		case UpdatePass::AfterPhysics:
		{
			physx::PxMat44 matrix_transform(m_Actor->getGlobalPose());
			glm::mat4 new_world_transform;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					new_world_transform[i][j] = matrix_transform[i][j];
				}
			}
			m_Entity->SetTransform(new_world_transform);
		}
	}
}

PhysicsComponent::PhysicsComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
	, m_Actor(nullptr)
	, m_StaticActor(nullptr)
{
	const glm::mat4& transform = owner->GetTransform();
	glm::vec3 position = transform[3];
	m_Actor = Physics::GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z)));

	std::string material_name = properties["material"].as<std::string>();
	std::string geometry_type = properties["shape"].as<std::string>();
	if (geometry_type == "box")
	{
		m_Actor->createShape(physx::PxBoxGeometry(properties["dimensions"][0].as<float>(), properties["dimensions"][1].as<float>(), properties["dimensions"][2].as<float>()), *Physics::GetMaterial(material_name));
		Physics::GetScene()->addActor(*m_Actor);
	}
	else if (geometry_type == "plane")
	{
		m_StaticActor = physx::PxCreatePlane(*Physics::GetPhysics(), physx::PxPlane(properties["vector"][0].as<float>(), properties["vector"][1].as<float>(), properties["vector"][2].as<float>(), properties["vector"][3].as<float>()), *Physics::GetMaterial(material_name));
		Physics::GetScene()->addActor(*m_StaticActor);
	}
}

void PhysicsComponent::OnSetActive(bool active)
{
	m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);
}
