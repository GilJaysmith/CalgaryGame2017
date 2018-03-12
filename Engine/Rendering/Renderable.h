#pragma once

#include "Engine/Math/AABB.h"

class Mesh;

class Renderable
{
public:
	virtual void Render() const = 0;
	virtual bool IsActive() const = 0;
	virtual unsigned int GetNumMeshes() const = 0;
	virtual unsigned int GetNumVerts() const = 0;
};

class RenderableMesh : public Renderable
{
public:
	RenderableMesh(Mesh* mesh);
	~RenderableMesh();

	void SetTransform(const glm::mat4& world_transform);
	void SetLocalPoses(const std::map<std::string, glm::mat4>& local_poses);
	std::map<std::string, glm::mat4> GetLocalPoses(const std::vector<std::string>& node_names);
	std::map<std::string, math::AABB> GetLocalAABBs(const std::vector<std::string>& node_names);

	virtual void Render() const override;

	void SetActive(bool active) { m_Active = active;  }
	virtual bool IsActive() const override { return m_Active;  }

	virtual unsigned int GetNumMeshes() const override;
	virtual unsigned int GetNumVerts() const override;

protected:
	Mesh* m_Mesh;
	glm::vec4 m_Tint;
	glm::mat4 m_WorldTransform;
	bool m_Active;
};