#include "Engine/Pch.h"

#include "Engine/Physics/Physics.h"
#include "PxPhysicsAPI.h"
#include "common/PxTolerancesScale.h"
#include "PxScene.h"
#include "PxSceneDesc.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"
#include "Engine/GameStates/Time.h"

namespace Physics
{

	class PhysicsAllocator : public physx::PxAllocatorCallback
	{
	public:
		virtual ~PhysicsAllocator() {}
		virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override
		{
			return operator new(size, MemoryPool::Physics);
		}
		virtual void deallocate(void* ptr) override
		{
			MemFree(ptr);
		}
	};


	class PhysicsErrorCallback : public physx::PxErrorCallback
	{
	public:
		virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			Logging::Log("Physics", message);
		}
	};

	PhysicsAllocator* pa = nullptr;
	PhysicsErrorCallback* pec = nullptr;

	physx::PxFoundation* foundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxCooking* cooking = nullptr;

	physx::PxScene* scene = nullptr;

	void Initialize()
	{
		Logging::Log("Physics", "Physics initializing...");

		pa = MemNew(MemoryPool::Physics, PhysicsAllocator);
		pec = MemNew(MemoryPool::Physics, PhysicsErrorCallback);

		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *pa, *pec);
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), false);
		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));

		physx::PxSceneDesc scene_desc(physics->getTolerancesScale());
		scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		scene_desc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
		scene = physics->createScene(scene_desc);

		physx::PxMaterial* gMaterial = physics->createMaterial(0.5f, 0.5f, 0.5f);
		physx::PxRigidStatic* groundPlane = PxCreatePlane(*physics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
		scene->addActor(*groundPlane);

		Logging::Log("Physics", "Physics initialized");
	}

	void Terminate()
	{
		Logging::Log("Physics", "Physics terminating...");
		scene->release();
		cooking->release();
		physics->release();
		foundation->release();

		MemDelete(pec);
		MemDelete(pa);
		Logging::Log("Physics", "Physics terminated");
	}

	void Simulate(const Time& time)
	{
		float seconds = time.toSeconds();
		scene->simulate(seconds);
		scene->fetchResults(true);
	}

	physx::PxPhysics* GetPhysics()
	{
		return physics;
	}

	physx::PxScene* GetScene()
	{
		return scene;
	}
}
