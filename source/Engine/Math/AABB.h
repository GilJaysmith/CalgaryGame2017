#pragma once

#include "sdks/glm/glm.hpp"

namespace math
{
	struct AABB
	{
		glm::vec3 lbb;	// left-bottom-back corner
		glm::vec3 rtf;	// right-top-front corner

		AABB()
			: lbb(FLT_MAX, FLT_MAX, FLT_MAX)
			, rtf(-FLT_MAX, -FLT_MAX, -FLT_MAX)
		{}

		void Expand(const glm::vec3& point)
		{
			lbb.x = glm::min(lbb.x, point.x);
			lbb.y = glm::min(lbb.y, point.y);
			lbb.z = glm::min(lbb.z, point.z);

			rtf.x = glm::max(rtf.x, point.x);
			rtf.y = glm::max(rtf.y, point.y);
			rtf.z = glm::max(rtf.z, point.z);
		}

		void Expand(const AABB& aabb)
		{
			if (aabb.lbb.x != FLT_MAX)
			{
				Expand(aabb.lbb);
			}
			if (aabb.rtf.x != -FLT_MAX)
			{
				Expand(aabb.rtf);
			}
		}

		bool Contains(const glm::vec3& point)
		{
			return lbb.x <= point.x && point.x <= rtf.x
				&& lbb.y <= point.y && point.y <= rtf.y
				&& lbb.z <= point.z && point.z <= rtf.z;
		}

		void Transform(const glm::mat4& mat)
		{
			lbb = glm::vec3(mat * glm::vec4(lbb, 1.0f));
			rtf = glm::vec3(mat * glm::vec4(rtf, 1.0f));
		}
	};
}