#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/Time.h"
#include "Engine/Physics/CollisionMessages.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleCreate.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleFilterShader.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"
#include "sdks/PhysX/PhysX/Include/common/PxTolerancesScale.h"
#include "sdks/PhysX/PhysX/Include/PxPhysicsAPI.h"
#include "sdks/PhysX/PxShared/include/pvd/PxPvd.h"
#include "sdks/PhysX/PxShared/include/pvd/PxPvdTransport.h"
#include "sdks/PhysX/PhysX/Include/PxScene.h"
#include "sdks/PhysX/PhysX/Include/PxSceneDesc.h"


namespace Physics
{

	class PhysicsAllocator : public physx::PxAllocatorCallback
	{
	public:
		virtual ~PhysicsAllocator() {}
		virtual void* allocate(size_t size, const char* typeName, const char* filename, int line) override
		{
			return MemAlloc(MemoryPool::Physics, size, filename, line);
		}
		virtual void deallocate(void* ptr) override
		{
			MemDelete(ptr);
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

	physx::PxPvd* pvd = nullptr;
	const char* pvd_host = "localhost";

	physx::PxScene* scene = nullptr;
	physx::PxDefaultCpuDispatcher* dispatcher = nullptr;

	std::map<std::string, physx::PxMaterial*> s_Materials;

	bool s_IsPaused = false;

	physx::PxMaterial* GetMaterial(const std::string& material_name)
	{
		if (s_Materials.find(material_name) == s_Materials.end())
		{
			Logging::Log("Physics", "Couldn't find material!");
			exit(1);
		}
		return s_Materials[material_name];
	}

	physx::PxFilterFlags FilterShader
	(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		PX_UNUSED(attributes0);
		PX_UNUSED(attributes1);
		PX_UNUSED(constantBlock);
		PX_UNUSED(constantBlockSize);

		if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
			return physx::PxFilterFlag::eSUPPRESS;

		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
		pairFlags |= physx::PxPairFlags(physx::PxU16(filterData0.word2 | filterData1.word2));

		return physx::PxFilterFlags();
	}

	class SimulationCallback : public physx::PxSimulationEventCallback
	{
	public:
		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {}
		virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override {}
		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override {}
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override
		{
			// Thurr's been a contact.
			physx::PxRigidActor* actor0 = pairHeader.actors[0];
			physx::PxRigidActor* actor1 = pairHeader.actors[1];

			Entity* entity0 = static_cast<Entity*>(actor0->userData);
			Entity* entity1 = static_cast<Entity*>(actor1->userData);
			if (entity0)
			{
				Message_CollisionTouch mct(actor0, actor1);
				entity0->OnMessage(&mct);
			}
			if (entity1)
			{
				Message_CollisionTouch mct(actor1, actor0);
				entity1->OnMessage(&mct);
			}
		}
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {}
		virtual void onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {}
	};

	SimulationCallback sSimulationCallback;

	class BroadPhaseCallback : public physx::PxBroadPhaseCallback
	{
	public:
		virtual	void onObjectOutOfBounds(physx::PxShape& shape, physx::PxActor& actor) override {}
		virtual	void onObjectOutOfBounds(physx::PxAggregate& aggregate) override {}
	};

	BroadPhaseCallback sBroadPhaseCallback;

	void Initialize()
	{
		Logging::Log("Physics", "Physics initializing...");

		pa = MemNew(MemoryPool::Physics, PhysicsAllocator);
		pec = MemNew(MemoryPool::Physics, PhysicsErrorCallback);

		foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, *pa, *pec);

		pvd = physx::PxCreatePvd(*foundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(pvd_host, 5425, 10);
		pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), false, pvd);
		cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));

		physx::PxSceneDesc scene_desc(physics->getTolerancesScale());
		scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		scene_desc.broadPhaseCallback = &sBroadPhaseCallback;
		dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		scene_desc.cpuDispatcher = dispatcher;
		scene_desc.filterShader = FilterShader;
		scene = physics->createScene(scene_desc);
		scene->setSimulationEventCallback(&sSimulationCallback);

		physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		physx::PxInitVehicleSDK(*physics);
		physx::PxVehicleSetBasisVectors(physx::PxVec3(0, 1, 0), physx::PxVec3(0, 0, 1));
		physx::PxVehicleSetUpdateMode(physx::PxVehicleUpdateMode::eVELOCITY_CHANGE);

		YAML::Node yaml = YAML::LoadFile("Data/Physics/materials.yaml");
		for (auto material_desc : yaml["materials"])
		{
			std::string material_name = material_desc.first.as<std::string>();
			std::vector<float> material_properties = material_desc.second.as<std::vector<float>>();
			physx::PxMaterial* physx_material = physics->createMaterial(material_properties[0], material_properties[1], material_properties[2]);
			s_Materials[material_name] = physx_material;
		}

		Logging::Log("Physics", "Physics initialized");
	}

	void Terminate()
	{
		Logging::Log("Physics", "Physics terminating...");

		for (auto material : s_Materials)
		{
			material.second->release();
		}

		physx::PxCloseVehicleSDK();

		dispatcher->release();
		scene->release();

		cooking->release();
		physics->release();
		physx::PxPvdTransport* transport = pvd->getTransport();
		pvd->release();
		transport->release();
		foundation->release();

		MemDelete(pec);
		MemDelete(pa);
		Logging::Log("Physics", "Physics terminated");
	}

	void Simulate(const Time& time)
	{
		if (time.toMilliseconds() == 0.0f)
		{
			return;
		}
		if (!s_IsPaused)
		{
			float seconds = time.toSeconds();
			while (true)
			{
				if (seconds <= 0.0f)
				{
					break;
				}
				float time_to_simulate = glm::max(0.0f, glm::min(1.0f / 60.0f, seconds));
				scene->simulate(seconds);
				scene->fetchResults(true);
				seconds -= time_to_simulate;
			}
		}
	}

	physx::PxPhysics* GetPhysics()
	{
		return physics;
	}

	physx::PxScene* GetScene()
	{
		return scene;
	}

	physx::PxCooking* GetCooking()
	{
		return cooking;
	}

	bool IsPaused()
	{
		return s_IsPaused;
	}

	void Pause(bool pause)
	{
		s_IsPaused = pause;
	}
}
