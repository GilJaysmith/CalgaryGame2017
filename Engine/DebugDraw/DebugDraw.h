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

	//IDebugObject* DebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour);
	//IDebugObject* DebugLines(const std::vector<glm::vec3>& points, const glm::vec3& colour);
	//IDebugObject* DebugGrid(const glm::mat4& transform, float grid_square_size, unsigned int squares_across, unsigned int squares_deep, const glm::vec3& colour);
	//IDebugObject* DebugSphere(const glm::vec3& centre, float radius, const glm::vec3& colour);
	//IDebugObject* DebugBox(const glm::mat4& transform, const glm::vec3& dims, const glm::vec3& colour);
	//IDebugObject* DebugCrosshair(const glm::vec3& centre, float scale, const glm::vec3& colour);
	//IDebugObject* DebugArrow(const glm::mat4& transform, float scale, const glm::vec3& colour);

	// OK, I want to be able to create an object which is:
	// - persistent or nonpersistent
	// - in a named layer
	// - at a given transform
	// - maybe connected to an external transform
	// - of a given colour
	// - and here is the type and geometry
	// So, it seems like the base class should store its colour and external transform
	// and we can assign the layer once
	// and likewise persistent / nonpersistent
	// and the incoming transform is part of the parameters but can be defaulted, as can the external transform
	// So:
	// DebugLine(string, glm::vec3, transform = glm::mat4(), transform* = nullptr)
	// DebugLines(string, array of glm::vec3, transform = glm::mat4(), transform* = nullptr)
	// DebugGrid(string, float, uint, uint, transform = glm::mat4(), transform* = nullptr)
	// DebugSphere(string, float, transform = glm::mat4(), transform* = nullptr)
	// DebugBox(string, glm::vec3, transform = glm::mat4(), transform* = nullptr)
	// DebugCrosshair(string, float, transform = glm::mat4(), transform* = nullptr)
	// DebugArrow(string, float, transform = glm::mat4(), transform* = nullptr)
	// And then IDebugObject supports:
	// - SetColour(glm::vec3)
	// - SetTransform(glm::mat4)
	// - SetExternalTransform(glm::mat4*)
	// - SetPersistent(bool)

	IDebugObject* DebugLine(const std::string& layer, const glm::vec3& end);
	IDebugObject* DebugLines(const std::string& layer, const std::vector<glm::vec3>& points);
	IDebugObject* DebugGrid(const std::string& layer, float grid_square_size, unsigned int squares_across, unsigned int squares_deep);
	IDebugObject* DebugSphere(const std::string& layer, float radius);
	IDebugObject* DebugBox(const std::string& layer, const glm::vec3& dims);
	IDebugObject* DebugCrosshair(const std::string& layer);
	IDebugObject* DebugArrow(const std::string& layer);
}