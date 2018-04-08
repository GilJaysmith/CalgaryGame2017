#include "Engine/Pch.h"

#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Health/DamageMessages.h"
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
	for (auto shape : m_ShapeToDebugObject)
	{
		shape.second->Release();
	}
}

bool TriggerComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Collision)
	{
		switch (message->GetMessageSubtype())
		{
		case CollisionMessageSubtype::Trigger:
		{
			Message_CollisionTrigger* mct = static_cast<Message_CollisionTrigger*>(message);
			physx::PxActor* my_actor = mct->m_MyActor;
			physx::PxActor* other_actor = mct->m_OtherActor;

			DebugDraw::IDebugObject* debug_object = m_ShapeToDebugObject[mct->m_MyShape];
			if (debug_object)
			{
				if (mct->m_Touch)
				{
					debug_object->SetColour(glm::vec3(1.0f, 0.0f, 0.0f));
				}
				else
				{
					debug_object->SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}

			if (other_actor)
			{
				Entity* other_entity = static_cast<Entity*>(other_actor->userData);
				if (other_entity)
				{
					std::stringstream str;
					str << "Trigger " << m_Entity->GetType() << " shape " << mct->m_MyShape << (mct->m_Touch ? " touch " : " untouch ") << other_entity->GetType() << " shape " << mct->m_OtherShape;
					Logging::Log("Trigger", str.str());
				}

				if (mct->m_Touch)
				{
					Message_DamageTakeDamage dtd(100);
					other_entity->OnMessage(&dtd);
				}
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

		// Make debug shape accordingly.
		DebugDraw::IDebugObject* debug_object = nullptr;

		switch (shape->getGeometryType())
		{
		case physx::PxGeometryType::eSPHERE:
		{
			physx::PxSphereGeometry geo;
			if (shape->getSphereGeometry(geo))
			{
				debug_object = DebugDraw::DebugSphere("TEST", geo.radius);
				debug_object->SetExternalTransform(m_Entity->GetTransformPtr());
			}
		}
		break;

		case physx::PxGeometryType::eBOX:
		{
			physx::PxBoxGeometry geo;
			if (shape->getBoxGeometry(geo))
			{
				debug_object = DebugDraw::DebugBox("TEST", physx_to_glm(geo.halfExtents) * 2.0f);
				debug_object->SetExternalTransform(m_Entity->GetTransformPtr());
			}
		}

		case physx::PxGeometryType::eCAPSULE:
		{
			physx::PxCapsuleGeometry geo;
			if (shape->getCapsuleGeometry(geo))
			{
				debug_object = DebugDraw::DebugCapsule("TEST", geo.radius, geo.halfHeight);
				debug_object->SetExternalTransform(m_Entity->GetTransformPtr());
			}
		}

		break;
		}
		if (debug_object)
		{
			debug_object->SetColour(glm::vec3(0.0f, 1.0f, 0.0f));
			m_ShapeToDebugObject[shape] = debug_object;
		}
	}

	std::vector<unsigned int> filter = properties["simfilter"].as<std::vector<unsigned int>>();
	assert(filter.size() == 4);
	physx::PxFilterData filter_data(filter[0], filter[1], filter[2] | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST, filter[3]);
	for (auto shape : shapes)
	{
		shape->setSimulationFilterData(filter_data);
	}

	m_Actor->userData = static_cast<void*>(m_Entity);
	m_Actor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, true);
	Physics::GetScene()->addActor(*m_Actor);
}
