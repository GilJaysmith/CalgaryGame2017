#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsUtils.h"
#include "Engine/Rendering/RenderMessages.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleFilterShader.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleTireFriction.h"
#include "Engine/Vehicles/VehicleComponent.h"
#include "Engine/Vehicles/VehicleMessages.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

//#pragma optimize ("", off)

Component* VehicleComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Vehicles, VehicleComponent)(owner, properties);
}

VehicleComponent::VehicleComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
	m_WheelNames = properties["wheels"].as<std::vector<std::string>>();
	CreateVehicle();
}

VehicleComponent::~VehicleComponent()
{
	DestroyVehicle();
}

bool VehicleComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Vehicle)
	{
		switch (message->GetMessageSubtype())
		{
			case VehicleMessageSubtype::SetInputs:
			{
				Message_VehicleSetInputs* mvsi = static_cast<Message_VehicleSetInputs*>(message);

				physx::PxRigidDynamic* actor = m_Vehicle4W->getRigidDynamicActor();
				float vehicle_vel = actor->getLinearVelocity().magnitude();

				mVehicleInputData.setAnalogHandbrake(mvsi->m_Handbrake);
				mVehicleInputData.setAnalogSteer(mvsi->m_Steer);
				float accel = mvsi->m_Acceleration;
				float brake = mvsi->m_Brake;

				physx::PxU32 gear = m_Vehicle4W->mDriveDynData.getCurrentGear();
				if (accel > brake)
				{
					if (gear == physx::PxVehicleGearsData::eREVERSE)
					{
						m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
					}
				}
				if (brake > accel)
				{
					if (vehicle_vel < 1.0f && gear >= physx::PxVehicleGearsData::eNEUTRAL)
					{
						m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
					}
					if (m_Vehicle4W->mDriveDynData.getCurrentGear() == physx::PxVehicleGearsData::eREVERSE)
					{
						std::swap(accel, brake);
					}
				}

				ImGui::SetNextWindowSizeConstraints(ImVec2(400, 100), ImVec2(800, 600));
				ImGui::Begin("Car intentions debug");
				ImGui::Text("Car gear: %d", m_Vehicle4W->mDriveDynData.getCurrentGear());
				ImGui::Text("Car speed: %f", vehicle_vel);
				ImGui::Text("Intention accel: %f", accel);
				ImGui::Text("Intention brake: %f", brake);
				ImGui::End();

				mVehicleInputData.setAnalogAccel(accel);
				mVehicleInputData.setAnalogBrake(brake);

				if (mvsi->m_Jump && !m_IsVehicleInAir)
				{
					// Apply an upward bump.
					actor->addForce(physx::PxVec3(0.0f, 20.0f, 0.0f), physx::PxForceMode::eVELOCITY_CHANGE);
				}

				if (mvsi->m_ResetOrientation)
				{
					glm::mat4 transform = m_Entity->GetTransform();
					glm::vec3 position = transform[3];
					physx::PxTransform startTransform(physx::PxVec3(position.x, position.y + 3.0f, position.z), physx::PxQuat(physx::PxIdentity));
					actor->setGlobalPose(startTransform);
					actor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f));
				}
			}
			break;

			case VehicleMessageSubtype::GetEngineInfo:
			{
				Message_VehicleGetEngineInfo* vegi = static_cast<Message_VehicleGetEngineInfo*>(message);
				vegi->m_EngineRotationSpeed = m_Vehicle4W->mDriveDynData.getEngineRotationSpeed();
			}
			break;
		}
	}
	return false;
}


physx::PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f,		0.75f,
	5.0f,		0.75f,
	30.0f,		0.125f,
	120.0f,		0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
physx::PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

physx::PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

physx::PxVehiclePadSmoothingData gPadSmoothingData =
{
	{
		6.0f,	//rise rate eANALOG_INPUT_ACCEL
		6.0f,	//rise rate eANALOG_INPUT_BRAKE		
		6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
		2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,	//fall rate eANALOG_INPUT_ACCEL
		10.0f,	//fall rate eANALOG_INPUT_BRAKE		
		10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
		5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
		5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
	}
};


snippetvehicle::VehicleDesc VehicleComponent::initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const physx::PxF32 chassisMass = 1500.0f;
	const physx::PxVec3 chassisDims(2.0f, 1.2f, 3.0f);	// HACK! These numbers roughly correspond to the Mini Cooper's chassis.
	const physx::PxVec3 chassisMOI
	((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
	const physx::PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const physx::PxF32 wheelMass = 20.0f;
	const physx::PxF32 wheelRadius = 0.5f;
	const physx::PxF32 wheelWidth = 0.4f;
	const physx::PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	const physx::PxU32 nbWheels = 4;

	snippetvehicle::VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = m_Material;
	vehicleDesc.chassisSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, (physx::PxU32)physx::PxPairFlag::eNOTIFY_TOUCH_FOUND, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = m_Material;
	vehicleDesc.wheelSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	Message_RenderGetLocalPoses rglp(m_WheelNames);
	m_Entity->OnMessage(&rglp);
	for (auto wheel_name : m_WheelNames)
	{
		vehicleDesc.wheelOffsets.push_back(glm_to_physx(rglp.m_LocalPoses[wheel_name]));
	}

	Message_RenderGetLocalAABBs rgla(m_WheelNames);
	m_Entity->OnMessage(&rgla);
	vehicleDesc.wheelRadius = (rgla.m_LocalAABBs[m_WheelNames[0]].rtf.z - rgla.m_LocalAABBs[m_WheelNames[0]].lbb.z) / 2.0f;
	vehicleDesc.wheelWidth = (rgla.m_LocalAABBs[m_WheelNames[0]].rtf.x - rgla.m_LocalAABBs[m_WheelNames[0]].lbb.x);

	return vehicleDesc;
}

void VehicleComponent::CreateVehicle()
{
	m_Material = NULL;

	m_VehicleSceneQueryData = NULL;
	m_BatchQuery = NULL;

	m_FrictionPairs = NULL;

	m_Vehicle4W = NULL;

	m_IsVehicleInAir = true;

	m_MimicKeyInputs = false;

	physx::PxPhysics* gPhysics = Physics::GetPhysics();
	physx::PxScene* gScene = Physics::GetScene();
	physx::PxCooking* gCooking = Physics::GetCooking();

	m_Material = gPhysics->createMaterial(0.1f, 0.1f, 0.1f);

	//Create the batched scene queries for the suspension raycasts.
	m_VehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, m_Allocator);
	m_BatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *m_VehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	m_FrictionPairs = snippetvehicle::createFrictionPairs(m_Material);

	//Create a vehicle that will drive on the plane.
	snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc();

	m_Vehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);

	const glm::mat4& transform = m_Entity->GetTransform();
	glm::vec4 position = transform[3];

	physx::PxTransform startTransform(physx::PxVec3(position.x, position.y + vehicleDesc.wheelRadius + 1.0f, position.z), physx::PxQuat(physx::PxIdentity));
	m_Vehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	m_Vehicle4W->getRigidDynamicActor()->userData = static_cast<void*>(m_Entity);
	gScene->addActor(*m_Vehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	m_Vehicle4W->setToRestState();
	m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
	m_Vehicle4W->mDriveDynData.setUseAutoGears(true);
}

void VehicleComponent::DestroyVehicle()
{
	m_Vehicle4W->getRigidDynamicActor()->release();
	m_Vehicle4W->free();

	m_BatchQuery->release();
	m_VehicleSceneQueryData->free(m_Allocator);
	m_FrictionPairs->release();
}


void VehicleComponent::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	switch (update_pass)
	{
		case UpdatePass::BeforePhysics:
		{
			physx::PxScene* gScene = Physics::GetScene();

			const physx::PxF32 timestep = elapsed_time.toSeconds();

			//Update the control inputs for the vehicle.
			if (m_MimicKeyInputs)
			{
				PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(gKeySmoothingData, gSteerVsForwardSpeedTable, mVehicleInputData, timestep, m_IsVehicleInAir, *m_Vehicle4W);
			}
			else
			{
				PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(gPadSmoothingData, gSteerVsForwardSpeedTable, mVehicleInputData, timestep, m_IsVehicleInAir, *m_Vehicle4W);
			}

			//Raycasts.
			physx::PxVehicleWheels* vehicles[1] = { m_Vehicle4W };
			physx::PxRaycastQueryResult* raycastResults = m_VehicleSceneQueryData->getRaycastQueryResultBuffer(0);
			const physx::PxU32 raycastResultsSize = m_VehicleSceneQueryData->getQueryResultBufferSize();
			PxVehicleSuspensionRaycasts(m_BatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

			//Vehicle update.
			const physx::PxVec3 grav = gScene->getGravity();
			physx::PxVehicleWheelQueryResult vehicleQueryResults[1] = { { m_WheelQueryResults, m_Vehicle4W->mWheelsSimData.getNbWheels() } };
			PxVehicleUpdates(timestep, grav, *m_FrictionPairs, 1, vehicles, vehicleQueryResults);

			//Work out if the vehicle is in the air.
			m_IsVehicleInAir = m_Vehicle4W->getRigidDynamicActor()->isSleeping() ? false : physx::PxVehicleIsInAir(vehicleQueryResults[0]);

			break;
		}

		case UpdatePass::AfterPhysics:
		{
			physx::PxMat44 matrix_transform(m_Vehicle4W->getRigidDynamicActor()->getGlobalPose());
			glm::mat4 new_world_transform = physx_to_glm(matrix_transform);
			m_Entity->SetTransform(new_world_transform);

			// We're also going to get the wheel local poses.
			std::map<std::string, glm::mat4> wheel_local_poses;
			for (auto shape_idx = 0; shape_idx < 4; ++shape_idx)
			{
				glm::mat4 local_pose = physx_to_glm(physx::PxMat44(m_WheelQueryResults[shape_idx].localPose));
				wheel_local_poses[m_WheelNames[shape_idx]] = local_pose;
			}
			Message_RenderSetLocalPoses mrslp(wheel_local_poses);
			m_Entity->OnMessage(&mrslp);

			ImGui::SetNextWindowSizeConstraints(ImVec2(400, 100), ImVec2(800, 600));
			ImGui::Begin("Car debug");
			ImGui::Text("Car gear: %d", m_Vehicle4W->mDriveDynData.getCurrentGear());
			ImGui::Text("Revs: %f", m_Vehicle4W->mDriveDynData.getEngineRotationSpeed());
			physx::PxRigidDynamic* actor = m_Vehicle4W->getRigidDynamicActor();
			ImGui::Text("Car speed: %f", actor->getLinearVelocity().magnitude());
			ImGui::End();

			break;
		}
	}
}
