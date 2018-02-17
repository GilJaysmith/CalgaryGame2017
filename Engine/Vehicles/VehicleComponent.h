#pragma once

#include "Engine/Entities/Component.h"

#include <sdks/PhysX/PhysX/Include/PxPhysicsAPI.h>
#include <sdks/PhysX/PhysX/Include/vehicle/PxVehicleUtil.h>

#include "Nvidia/SnippetVehicleCreate.h"
#include "Nvidia/SnippetVehicleSceneQuery.h"

class VehicleComponent : public Component
{
public:
	static Component* CreateComponent(Entity* owner, const YAML::Node& properties);

public:
	virtual ~VehicleComponent();

	virtual bool OnMessage(Message*);
	virtual void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass);

protected:
	VehicleComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) {}

	void CreateVehicle();
	void DestroyVehicle();

private:
	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;

	physx::PxMaterial*				gMaterial;

	snippetvehicle::VehicleSceneQueryData*	gVehicleSceneQueryData;
	physx::PxBatchQuery*			gBatchQuery;

	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs;

	physx::PxVehicleDrive4W*		gVehicle4W;

	bool					gIsVehicleInAir;

	physx::PxF32					gVehicleModeLifetime;
	physx::PxF32					gVehicleModeTimer;
	physx::PxU32					gVehicleOrderProgress;
	bool					gVehicleOrderComplete;
	bool					gMimicKeyInputs;

	snippetvehicle::VehicleDesc initVehicleDesc();
	void startAccelerateForwardsMode();
	void startAccelerateReverseMode();
	void startBrakeMode();
	void startTurnHardLeftMode();
	void startTurnHardRightMode();
	void startHandbrakeTurnLeftMode();
	void startHandbrakeTurnRightMode();
	void releaseAllControls();
	void incrementDrivingMode(const physx::PxF32 timestep);

};