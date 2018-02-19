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

	virtual bool OnMessage(Message*) override;
	virtual void OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass) override;

protected:
	VehicleComponent(Entity* owner, const YAML::Node& properties);

	virtual void OnSetActive(bool active) {}

	void CreateVehicle();
	void DestroyVehicle();

private:
	physx::PxDefaultAllocator m_Allocator;
	physx::PxDefaultErrorCallback m_ErrorCallback;
	physx::PxMaterial* m_Material;
	snippetvehicle::VehicleSceneQueryData* m_VehicleSceneQueryData;
	physx::PxBatchQuery* m_BatchQuery;
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* m_FrictionPairs;
	physx::PxVehicleDrive4W*  m_Vehicle4W;
	bool m_IsVehicleInAir;
	physx::PxF32 m_VehicleModeLifetime;
	physx::PxF32 m_VehicleModeTimer;
	physx::PxU32 m_VehicleOrderProgress;
	bool m_VehicleOrderComplete;
	bool m_MimicKeyInputs;
	physx::PxVehicleDrive4WRawInputData mVehicleInputData;

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