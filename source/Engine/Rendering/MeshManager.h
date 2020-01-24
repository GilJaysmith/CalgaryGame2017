#pragma once

class Mesh;

namespace MeshManager
{
	void Initialize();
	void Terminate();

	Mesh* LoadMeshFromFile(const std::string& mesh_name);
}