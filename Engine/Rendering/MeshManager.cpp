#include "Engine/Pch.h"

#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/MeshManager.h"

namespace MeshManager
{
	std::map<std::string, Mesh*> s_Meshes;

	void Initialize()
	{

	}

	void Terminate()
	{
		for (auto it : s_Meshes)
		{
			MemDelete(it.second);
		}
		s_Meshes.clear();
	}

	Mesh* LoadMeshFromFile(const std::string& mesh_name)
	{
		auto it = s_Meshes.find(mesh_name);
		if (it != s_Meshes.end())
		{
			return it->second;
		}
		Mesh* mesh = MemNew(MemoryPool::Rendering, Mesh);
		mesh->LoadFromYaml(mesh_name);
		s_Meshes[mesh_name] = mesh;
		return mesh;
	}
}