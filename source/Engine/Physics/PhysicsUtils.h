#pragma once

#include "sdks/glm/glm.hpp"
#include "foundation/PxMat44.h"
#include "PxShape.h"



inline glm::mat4 physx_to_glm(const physx::PxMat44& physx)
{
	glm::mat4 glm;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			glm[i][j] = physx[i][j];
		}
	}
	return glm;
}

inline physx::PxMat44 glm_to_physx(const glm::mat4& glm)
{
	physx::PxMat44 physx;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			physx[i][j] = glm[i][j];
		}
	}
	return physx;
}

inline glm::vec3 physx_to_glm(const physx::PxVec3& physx)
{
	glm::vec3 glm;
	glm.x = physx.x;
	glm.y = physx.y;
	glm.z = physx.z;
	return glm;
}

physx::PxShape* CreatePhysicsShape(const YAML::Node& node, const physx::PxMaterial& material, physx::PxShapeFlags flags);
