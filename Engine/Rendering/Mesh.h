#pragma once

#include <map>

#include "Engine/Math/AABB.h"

struct MeshNode;
struct SubMesh;

struct MeshRenderParams
{
	MeshRenderParams(const glm::mat4& world_transform, const glm::vec4& tint, const std::map<std::string, glm::mat4>& poses)
		: m_WorldTransform(world_transform), m_Tint(tint), m_Poses(poses)
	{}
	glm::vec4 m_Tint;
	glm::mat4 m_WorldTransform;
	std::map<std::string, glm::mat4> m_Poses;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadFromYaml(const std::string& filename);
	void Render(const MeshRenderParams& params);
	std::map<std::string, glm::mat4> GetLocalPoses();
	std::map<std::string, math::AABB> GetLocalAABBs(const std::vector<std::string>& node_names);

	unsigned int GetNumVerts() const;

protected:

	std::string m_MeshName;
	MeshNode* m_RootNode;
	std::map<std::string, MeshNode*> m_NodesByName;
	std::vector<SubMesh*> m_SubMeshes;
	unsigned int m_NumVerts;
};