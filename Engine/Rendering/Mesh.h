#pragma once

#include <map>

#include "Engine/Math/AABB.h"

struct MeshNode;
struct SubMesh;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadFromYaml(const std::string& filename);
	void Render(const glm::mat4& world_transform, const glm::vec4& tint);
	void SetLocalPoses(const std::map<std::string, glm::mat4>& local_poses);
	void GetLocalPoses(std::map<std::string, glm::mat4>& local_poses);

	unsigned int GetNumVerts() const;

protected:

	std::string m_MeshName;
	MeshNode* m_RootNode;
	std::map<std::string, MeshNode*> m_NodesByName;
	std::vector<SubMesh*> m_SubMeshes;
	unsigned int m_NumVerts;
};