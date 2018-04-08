#pragma once

#include "sdks/glm/glm.hpp"


namespace DebugDraw
{
	void Initialize();
	void Terminate();

	void Render();

	class IDebugObject
	{
	public:
		virtual void SetColour(const glm::vec3& colour) = 0;
		virtual void SetTransform(const glm::mat4& transform) = 0;
		virtual void SetExternalTransform(const glm::mat4* transform) = 0;
		virtual void SetPersistent(bool persistent) = 0;
		virtual void Release() = 0;
	};


	IDebugObject* DebugLine(const std::string& layer, const glm::vec3& end);
	IDebugObject* DebugLines(const std::string& layer, const std::vector<glm::vec3>& points);
	IDebugObject* DebugGrid(const std::string& layer, float grid_square_size, unsigned int squares_across, unsigned int squares_deep);
	IDebugObject* DebugSphere(const std::string& layer, float radius);
	IDebugObject* DebugBox(const std::string& layer, const glm::vec3& dims);
	IDebugObject* DebugCrosshair(const std::string& layer);
	IDebugObject* DebugArrow(const std::string& layer);
	IDebugObject* DebugCapsule(const std::string& layer, float radius, float half_height);
}