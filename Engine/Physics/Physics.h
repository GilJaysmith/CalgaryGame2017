#pragma once

class Time;

namespace physx
{
	class PxMaterial;
	class PxPhysics;
	class PxScene;
}

namespace Physics
{
	void Initialize();
	void Terminate();

	void Simulate(const Time& time);

	physx::PxPhysics* GetPhysics();
	physx::PxScene* GetScene();

	physx::PxMaterial* GetMaterial(const std::string& material_name);

	bool IsPaused();
	void Pause(bool pause);
};
