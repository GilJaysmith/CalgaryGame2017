#pragma once


namespace DebugDraw
{
	void Initialize();
	void Terminate();

	void Render();

	class IDebugObject
	{
	public:
		virtual void Release() = 0;
	};

	IDebugObject* DebugLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& colour);
	IDebugObject* DebugLines(const std::vector<glm::vec3>& points, const glm::vec3& colour);
	IDebugObject* DebugGrid(const glm::mat4& transform, float grid_square_size, unsigned int squares_across, unsigned int squares_deep, const glm::vec3& colour);
	IDebugObject* DebugSphere(const glm::vec3& centre, float radius, const glm::vec3& colour);
	IDebugObject* DebugBox(const glm::mat4& transform, const glm::vec3& dims, const glm::vec3& colour);
	IDebugObject* DebugCrosshair(const glm::vec3& centre, float scale, const glm::vec3& colour);
	IDebugObject* DebugArrow(const glm::mat4& transform, float scale, const glm::vec3& colour);
}