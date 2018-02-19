#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleFilterShader.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleTireFriction.h"
#include "Engine/Vehicles/VehicleComponent.h"
#include "Engine/Vehicles/VehicleMessages.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"


Component* VehicleComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Vehicles, VehicleComponent)(owner, properties);
}

VehicleComponent::VehicleComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
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
				mVehicleInputData.setAnalogAccel(mvsi->m_Acceleration);
				mVehicleInputData.setAnalogBrake(mvsi->m_Brake);
				mVehicleInputData.setAnalogHandbrake(mvsi->m_Handbrake);
				mVehicleInputData.setAnalogSteer(mvsi->m_Steer);
				break;
			}
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

enum DriveMode
{
	eDRIVE_MODE_ACCEL_FORWARDS = 0,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_NONE
};

DriveMode gDriveModeOrder[] =
{
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_ACCEL_REVERSE,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_LEFT,
	eDRIVE_MODE_BRAKE,
	eDRIVE_MODE_HARD_TURN_RIGHT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
	eDRIVE_MODE_ACCEL_FORWARDS,
	eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
	eDRIVE_MODE_NONE
};


snippetvehicle::VehicleDesc VehicleComponent::initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const physx::PxF32 chassisMass = 1500.0f;
	const physx::PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
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
	const physx::PxU32 nbWheels = 6;

	snippetvehicle::VehicleDesc vehicleDesc;

	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = m_Material;
	vehicleDesc.chassisSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_CHASSIS, snippetvehicle::COLLISION_FLAG_CHASSIS_AGAINST, 0, 0);

	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = m_Material;
	vehicleDesc.chassisSimFilterData = physx::PxFilterData(snippetvehicle::COLLISION_FLAG_WHEEL, snippetvehicle::COLLISION_FLAG_WHEEL_AGAINST, 0, 0);

	return vehicleDesc;
}

void VehicleComponent::startAccelerateForwardsMode()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		mVehicleInputData.setAnalogAccel(1.0f);
	}
}

void VehicleComponent::startAccelerateReverseMode()
{
	m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);

	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalAccel(true);
	}
	else
	{
		mVehicleInputData.setAnalogAccel(1.0f);
	}
}

void VehicleComponent::startBrakeMode()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalBrake(true);
	}
	else
	{
		mVehicleInputData.setAnalogBrake(1.0f);
	}
}

void VehicleComponent::startTurnHardLeftMode()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalAccel(true);
		mVehicleInputData.setDigitalSteerLeft(true);
	}
	else
	{
		mVehicleInputData.setAnalogAccel(true);
		mVehicleInputData.setAnalogSteer(-1.0f);
	}
}

void VehicleComponent::startTurnHardRightMode()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalAccel(true);
		mVehicleInputData.setDigitalSteerRight(true);
	}
	else
	{
		mVehicleInputData.setAnalogAccel(1.0f);
		mVehicleInputData.setAnalogSteer(1.0f);
	}
}

void VehicleComponent::startHandbrakeTurnLeftMode()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalSteerLeft(true);
		mVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		mVehicleInputData.setAnalogSteer(-1.0f);
		mVehicleInputData.setAnalogHandbrake(1.0f);
	}
}

void VehicleComponent::startHandbrakeTurnRightMode()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalSteerRight(true);
		mVehicleInputData.setDigitalHandbrake(true);
	}
	else
	{
		mVehicleInputData.setAnalogSteer(1.0f);
		mVehicleInputData.setAnalogHandbrake(1.0f);
	}
}


void VehicleComponent::releaseAllControls()
{
	if (m_MimicKeyInputs)
	{
		mVehicleInputData.setDigitalAccel(false);
		mVehicleInputData.setDigitalSteerLeft(false);
		mVehicleInputData.setDigitalSteerRight(false);
		mVehicleInputData.setDigitalBrake(false);
		mVehicleInputData.setDigitalHandbrake(false);
	}
	else
	{
		mVehicleInputData.setAnalogAccel(0.0f);
		mVehicleInputData.setAnalogSteer(0.0f);
		mVehicleInputData.setAnalogBrake(0.0f);
		mVehicleInputData.setAnalogHandbrake(0.0f);
	}
}


void VehicleComponent::incrementDrivingMode(const physx::PxF32 timestep)
{
	m_VehicleModeTimer += timestep;
	if (m_VehicleModeTimer > m_VehicleModeLifetime)
	{
		//If the mode just completed was eDRIVE_MODE_ACCEL_REVERSE then switch back to forward gears.
		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[m_VehicleOrderProgress])
		{
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
		}

		//Increment to next driving mode.
		m_VehicleModeTimer = 0.0f;
		m_VehicleOrderProgress++;
		releaseAllControls();

		//If we are at the end of the list of driving modes then start again.
		if (eDRIVE_MODE_NONE == gDriveModeOrder[m_VehicleOrderProgress])
		{
			m_VehicleOrderProgress = 0;
			m_VehicleOrderComplete = true;
		}

		//Start driving in the selected mode.
		DriveMode eDriveMode = gDriveModeOrder[m_VehicleOrderProgress];
		switch (eDriveMode)
		{
		case eDRIVE_MODE_ACCEL_FORWARDS:
			startAccelerateForwardsMode();
			break;
		case eDRIVE_MODE_ACCEL_REVERSE:
			startAccelerateReverseMode();
			break;
		case eDRIVE_MODE_HARD_TURN_LEFT:
			startTurnHardLeftMode();
			break;
		case eDRIVE_MODE_HANDBRAKE_TURN_LEFT:
			startHandbrakeTurnLeftMode();
			break;
		case eDRIVE_MODE_HARD_TURN_RIGHT:
			startTurnHardRightMode();
			break;
		case eDRIVE_MODE_HANDBRAKE_TURN_RIGHT:
			startHandbrakeTurnRightMode();
			break;
		case eDRIVE_MODE_BRAKE:
			startBrakeMode();
			break;
		case eDRIVE_MODE_NONE:
			break;
		};

		//If the mode about to start is eDRIVE_MODE_ACCEL_REVERSE then switch to reverse gears.
		if (eDRIVE_MODE_ACCEL_REVERSE == gDriveModeOrder[m_VehicleOrderProgress])
		{
			m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
		}
	}
}


void VehicleComponent::CreateVehicle()
{
	m_Material = NULL;

	m_VehicleSceneQueryData = NULL;
	m_BatchQuery = NULL;

	m_FrictionPairs = NULL;

	m_Vehicle4W = NULL;

	m_IsVehicleInAir = true;

	m_VehicleModeLifetime = 4.0f;
	m_VehicleModeTimer = 0.0f;
	m_VehicleOrderProgress = 0;
	m_VehicleOrderComplete = false;
	m_MimicKeyInputs = false;


	physx::PxPhysics* gPhysics = Physics::GetPhysics();
	physx::PxScene* gScene = Physics::GetScene();
	physx::PxCooking* gCooking = Physics::GetCooking();

	m_Material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	//Create the batched scene queries for the suspension raycasts.
	m_VehicleSceneQueryData = snippetvehicle::VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, 1, snippetvehicle::WheelSceneQueryPreFilterBlocking, NULL, m_Allocator);
	m_BatchQuery = snippetvehicle::VehicleSceneQueryData::setUpBatchedSceneQuery(0, *m_VehicleSceneQueryData, gScene);

	//Create the friction table for each combination of tire and surface type.
	m_FrictionPairs = snippetvehicle::createFrictionPairs(m_Material);

	////Create a plane to drive on.
	physx::PxFilterData groundPlaneSimFilterData(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
	physx::PxRigidStatic* gGroundPlane = snippetvehicle::createDrivablePlane(groundPlaneSimFilterData, m_Material, gPhysics);
	gScene->addActor(*gGroundPlane);

	//Create a vehicle that will drive on the plane.
	snippetvehicle::VehicleDesc vehicleDesc = initVehicleDesc();
	m_Vehicle4W = createVehicle4W(vehicleDesc, gPhysics, gCooking);

	const glm::mat4& transform = m_Entity->GetTransform();
	glm::vec4 position = transform[3];

	physx::PxTransform startTransform(physx::PxVec3(position.x, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), position.z), physx::PxQuat(physx::PxIdentity));
	m_Vehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	gScene->addActor(*m_Vehicle4W->getRigidDynamicActor());

	//Set the vehicle to rest in first gear.
	//Set the vehicle to use auto-gears.
	m_Vehicle4W->setToRestState();
	m_Vehicle4W->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eFIRST);
	m_Vehicle4W->mDriveDynData.setUseAutoGears(true);

	m_VehicleModeTimer = 0.0f;
	m_VehicleOrderProgress = 0;
//	gVehicleOrderProgress = (physx::PxU32)((rand() / float(RAND_MAX)) * (sizeof(gDriveModeOrder) / sizeof(gDriveModeOrder[0])));
	startBrakeMode();
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

			//Cycle through the driving modes to demonstrate how to accelerate/reverse/brake/turn etc.
//			incrementDrivingMode(timestep);

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
			physx::PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
			physx::PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults, m_Vehicle4W->mWheelsSimData.getNbWheels() } };
			PxVehicleUpdates(timestep, grav, *m_FrictionPairs, 1, vehicles, vehicleQueryResults);

			//Work out if the vehicle is in the air.
			m_IsVehicleInAir = m_Vehicle4W->getRigidDynamicActor()->isSleeping() ? false : physx::PxVehicleIsInAir(vehicleQueryResults[0]);
		}

		case UpdatePass::AfterPhysics:
		{
			physx::PxMat44 matrix_transform(m_Vehicle4W->getRigidDynamicActor()->getGlobalPose());
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
