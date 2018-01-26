#pragma once

class Time;

namespace physx
{
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
};
