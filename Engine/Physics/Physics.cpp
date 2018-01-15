#include "Pch.h"

#include "Physics/Physics.h"
#include "PxPhysicsAPI.h"
#include "common/PxTolerancesScale.h"

#include "Logging/Logging.h"
#include "Memory/Memory.h"


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

	void Initialize()
	{
		Logging::Log("Physics", "Physics initializing...");

		pa = MemNew(MemoryPool::Physics, PhysicsAllocator);
		pec = MemNew(MemoryPool::Physics, PhysicsErrorCallback);

		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *pa, *pec);
		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), false);
		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));

		Logging::Log("Physics", "Physics initialized");
	}

	void Terminate()
	{
		Logging::Log("Physics", "Physics terminating...");
		cooking->release();
		physics->release();
		foundation->release();

		MemDelete(pec);
		MemDelete(pa);
		Logging::Log("Physics", "Physics terminated");
	}
}
