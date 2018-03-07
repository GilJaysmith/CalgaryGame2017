#pragma once

#include "sdks/glm/glm.hpp"
#include "sdks/PhysX/PxShared/include/foundation/PxMat44.h"


glm::mat4 physx_to_glm(const physx::PxMat44& physx)
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

physx::PxMat44 glm_to_physx(const glm::mat4& glm)
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
