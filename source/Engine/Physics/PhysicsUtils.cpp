#include "Engine/Pch.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsUtils.h"

#include "sdks/PhysX/Include/PxPhysics.h"
#include "sdks/PhysX/Include/PxShape.h"

physx::PxShape* CreatePhysicsShape(const YAML::Node& node, const physx::PxMaterial& material, physx::PxShapeFlags flags)
{
	physx::PxShape* shape = nullptr;
	if (node["type"].as<std::string>() == "sphere")
	{
		physx::PxSphereGeometry sphere_geometry(node["radius"].as<float>());
		shape = Physics::GetPhysics()->createShape(sphere_geometry, material, true, flags);
	}
	else if (node["type"].as<std::string>() == "box")
	{
		std::vector<float> half_extents = node["halfExtents"].as<std::vector<float>>();
		assert(half_extents.size() == 3);
		physx::PxBoxGeometry box_geometry(half_extents[0], half_extents[1], half_extents[2]);
		shape = Physics::GetPhysics()->createShape(box_geometry, material, true, flags);
	}
	else if (node["type"].as<std::string>() == "capsule")
	{
		physx::PxCapsuleGeometry capsule_geometry(node["radius"].as<float>(), node["halfHeight"].as<float>());
		shape = Physics::GetPhysics()->createShape(capsule_geometry, material, true, flags);
	}
	else
	{
		assert(false);
	}
	return shape;
}
