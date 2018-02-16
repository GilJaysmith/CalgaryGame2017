#include "Engine/Pch.h"

#include "Engine/Physics/Physics.h"
#include "PxPhysicsAPI.h"
#include "common/PxTolerancesScale.h"
#include "PxScene.h"
#include "PxSceneDesc.h"

#include "Engine/GameStates/Time.h"
#include "sdks/libyaml/include/yaml-cpp/yaml.h"

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
		dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		scene_desc.cpuDispatcher = dispatcher;
		scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
//		scene_desc.filterShader = FilterShader;
		scene = physics->createScene(scene_desc);

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

		dispatcher->release();
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
		if (!s_IsPaused)
		{
			float seconds = time.toSeconds();
			scene->simulate(seconds);
			scene->fetchResults(true);
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
