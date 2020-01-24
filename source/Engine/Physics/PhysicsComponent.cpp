#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Physics/PhysicsMessages.h"
#include "Engine/Physics/PhysicsUtils.h"

#include "yaml-cpp/yaml.h"

#include "PxPhysicsAPI.h"
#include "foundation/PxFoundationConfig.h"


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
					physx::PxRigidDynamic* dynamic_actor = m_Actor->is<physx::PxRigidDynamic>();
					if (dynamic_actor)
					{
						dynamic_actor->setLinearVelocity(physx::PxVec3(pslv->m_LinearVelocity.x, pslv->m_LinearVelocity.y, pslv->m_LinearVelocity.z));
					}
				}
				return true;
			}
			
			case PhysicsMessageSubtype::SetAngularVelocity:
			{
				Message_PhysicsSetAngularVelocity* psav = dynamic_cast<Message_PhysicsSetAngularVelocity*>(message);
				if (m_Actor)
				{
					physx::PxRigidDynamic* dynamic_actor = m_Actor->is<physx::PxRigidDynamic>();
					if (dynamic_actor)
					{
						dynamic_actor->setAngularVelocity(physx::PxVec3(psav->m_AngularVelocity.x, psav->m_AngularVelocity.y, psav->m_AngularVelocity.z));
					}
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
			if (m_Actor->is<physx::PxRigidDynamic>())
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
			break;
		}
	}
}

void PhysicsComponent::OnTransformUpdated()
{
	if (m_Actor)
	{
		m_Actor->setGlobalPose(physx::PxTransform(glm_to_physx(m_Entity->GetTransform())));
	}
}

PhysicsComponent::PhysicsComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
	, m_Actor(nullptr)
{
	const glm::mat4& transform = owner->GetTransform();
	glm::vec3 position = transform[3];

	std::string material_name = properties["material"].as<std::string>();
	std::string geometry_type = properties["shape"].as<std::string>();
	if (geometry_type == "box")
	{
		m_Actor = Physics::GetPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z)));
		physx::PxShape* shape = Physics::GetPhysics()->createShape(physx::PxBoxGeometry(properties["dimensions"][0].as<float>(), properties["dimensions"][1].as<float>(), properties["dimensions"][2].as<float>()), *Physics::GetMaterial(material_name));
		m_Actor->attachShape(*shape);
	}
	else if (geometry_type == "plane")
	{
		m_Actor = physx::PxCreatePlane(*Physics::GetPhysics(), physx::PxPlane(properties["vector"][0].as<float>(), properties["vector"][1].as<float>(), properties["vector"][2].as<float>(), properties["vector"][3].as<float>()), *Physics::GetMaterial(material_name));
	}
	if (m_Actor)
	{
		physx::PxU32 num_shapes = m_Actor->getNbShapes();
		physx::PxShape** shapes = (physx::PxShape**)(MemNewBytes(MemoryPool::Physics, num_shapes * sizeof(physx::PxShape*)));
		m_Actor->getShapes(shapes, num_shapes);
		
		if (properties["simfilter"])
		{
			std::vector<unsigned int> filter = properties["simfilter"].as<std::vector<unsigned int>>();
			physx::PxFilterData filter_data(filter[0], filter[1], filter[2], filter[3]);
			for (physx::PxU32 shape_idx = 0; shape_idx < num_shapes; ++shape_idx)
			{
				shapes[shape_idx]->setSimulationFilterData(filter_data);
			}
		}

		if (properties["queryfilter"])
		{
			std::vector<unsigned int> filter = properties["queryfilter"].as<std::vector<unsigned int>>();
			physx::PxFilterData filter_data(filter[0], filter[1], filter[2], filter[3]);
			for (physx::PxU32 shape_idx = 0; shape_idx < num_shapes; ++shape_idx)
			{
				shapes[shape_idx]->setQueryFilterData(filter_data);
			}
		}

		MemDelete(shapes);

		m_Actor->userData = static_cast<void*>(m_Entity);
		Physics::GetScene()->addActor(*m_Actor);
	}
}

void PhysicsComponent::OnSetActive(bool active)
{
	m_Actor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, !active);
}
