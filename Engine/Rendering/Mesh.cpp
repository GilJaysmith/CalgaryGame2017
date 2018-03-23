#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

#include "sdks/assimp/include/assimp/Exporter.hpp"
#include "sdks/assimp/include/assimp/Importer.hpp"
#include "sdks/assimp/include/assimp/scene.h"
#include "sdks/assimp/include/assimp/postprocess.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

#include <vector>

//#pragma optimize ("", off)

struct AttributeBinding
{
	std::string name;
	int num_floats;
};

void CopyMat(const aiMatrix4x4& from, glm::mat4& to)
{
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;	to[2][3] = from.d3; to[3][3] = from.d4;
}


struct SubMesh
{
	GLuint m_Vao;
	unsigned int m_NumVerts;
	GLuint m_Program;
	std::map<unsigned int, GLuint> m_Textures;
	math::AABB m_AABB;

	SubMesh(const YAML::Node& node, GLuint program, const std::vector<AttributeBinding>& attribute_bindings)
		: m_Program(program)
	{
		unsigned int total_floats = 0;
		for (auto ab : attribute_bindings)
		{
			total_floats += ab.num_floats;
		}

		std::vector<std::vector<float>> final_vertices;

		for (auto vert : node["vertices"])
		{
			final_vertices.push_back(vert.as<std::vector<float>>());
		}

		m_NumVerts = (unsigned int)final_vertices.size();
		if (!m_NumVerts)
		{
			return;
		}

		float* vert_data = (float*)MemNewBytes(MemoryPool::Rendering, sizeof(float) * final_vertices.size() * final_vertices[0].size());
		float* v = vert_data;
		for (auto vert : final_vertices)
		{
			if (vert.size() != total_floats)
			{
				Logging::Log("Mesh", "Mesh vertex data has incorrect length");
				exit(1);
			}
			m_AABB.Expand(glm::vec3(vert[0], vert[1], vert[2]));
			for (auto f : vert)
			{
				*v++ = f;
			}
		}

		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		int vert_data_size = (int)(final_vertices.size() * final_vertices[0].size() * sizeof(float));
		glBufferData(GL_ARRAY_BUFFER, vert_data_size, vert_data, GL_STATIC_DRAW);

		// Textures
		unsigned int texture_index = 0;
		for (auto texture : node["textures"])
		{
			auto texture_id = TextureManager::LoadTexture(texture.as<std::string>());
			m_Textures[texture_index] = texture_id;
			++texture_index;
		}

		// Apply attribute bindings;
		int offset = 0;
		for (auto ab : attribute_bindings)
		{
			GLint posAttrib = glGetAttribLocation(m_Program, ab.name.c_str());
			if (posAttrib != -1)
			{
				glVertexAttribPointer(posAttrib, ab.num_floats, GL_FLOAT, GL_FALSE, total_floats * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
				glEnableVertexAttribArray(posAttrib);
			}
			offset += ab.num_floats;
		}

		MemDelete(vert_data);
	}

	SubMesh(aiMesh* mesh, GLuint program, const std::vector<AttributeBinding>& attribute_bindings)
		: m_Program(program)
	{
		unsigned int total_floats = 0;
		for (auto ab : attribute_bindings)
		{
			total_floats += ab.num_floats;
		}

		std::vector<std::vector<float>> final_vertices;

		aiVector3D* vertices = mesh->mVertices;
		aiVector3D* normals = mesh->mNormals;

		for (unsigned int face_idx = 0; face_idx < mesh->mNumFaces; ++face_idx)
		{
			aiFace& face = mesh->mFaces[face_idx];
			unsigned int* vert_indices = face.mIndices;
			glm::vec3 tint(1.0f, 1.0f, 1.0f);
			for (unsigned int vert_idx_loop = 0; vert_idx_loop < face.mNumIndices; ++vert_idx_loop)
			{
				unsigned int vert_idx = vert_indices[vert_idx_loop];
				aiVector3D vert = vertices[vert_idx];
				aiVector3D normal = normals[vert_idx];
				std::vector<float> this_vert = { vert.x, vert.y, vert.z, normal.x, normal.y, normal.z, tint.x, tint.y, tint.z };
				final_vertices.push_back(this_vert);
				m_AABB.Expand(glm::vec3(vert.x, vert.y, vert.z));
			}
		}

		std::stringstream str;
		str << "Mesh " << mesh->mName.C_Str() << " AABB (" << m_AABB.lbb.x << ", " << m_AABB.lbb.y << ", " << m_AABB.lbb.z
			<< ") -> (" << m_AABB.lbb.x << ", " << m_AABB.lbb.y << ", " << m_AABB.lbb.z
			<< ")";
		Logging::Log("Mesh", str.str());

		m_NumVerts = (unsigned int)final_vertices.size();

		if (m_NumVerts == 0)
		{
			return;
		}

		float* vert_data = (float*)MemNewBytes(MemoryPool::Rendering, sizeof(float) * final_vertices.size() * final_vertices[0].size());
		float* v = vert_data;
		for (auto vert : final_vertices)
		{
			if (vert.size() != total_floats)
			{
				Logging::Log("Mesh", "Mesh vertex data has incorrect length");
				exit(1);
			}
			for (auto f : vert)
			{
				*v++ = f;
			}
		}

		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		int vert_data_size = (int)(final_vertices.size() * final_vertices[0].size() * sizeof(float));
		glBufferData(GL_ARRAY_BUFFER, vert_data_size, vert_data, GL_STATIC_DRAW);

		// Apply attribute bindings;
		int offset = 0;
		for (auto ab : attribute_bindings)
		{
			GLint posAttrib = glGetAttribLocation(m_Program, ab.name.c_str());
			if (posAttrib != -1)
			{
				glVertexAttribPointer(posAttrib, ab.num_floats, GL_FLOAT, GL_FALSE, total_floats * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
				glEnableVertexAttribArray(posAttrib);
			}
			offset += ab.num_floats;
		}

		MemDelete(vert_data);
	}

	void Render(const glm::mat4& transform, const glm::vec4& tint)
	{
		if (m_NumVerts)
		{
			ShaderManager::SetActiveShader(m_Program);

			for (auto it : m_Textures)
			{
				glActiveTexture(GL_TEXTURE0 + it.first);
				glBindTexture(GL_TEXTURE_2D, it.second);
			}

			GLint uniModel = glGetUniformLocation(m_Program, "model_transform");
			if (uniModel > -1)
			{
				glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(transform));
			}

			glm::mat4 normal_transform = glm::transpose(glm::inverse(transform));
			GLint uniNormal = glGetUniformLocation(m_Program, "model_normal_transform");
			if (uniNormal > -1)
			{
				glUniformMatrix4fv(uniNormal, 1, GL_FALSE, glm::value_ptr(normal_transform));
			}

			GLint uniTint = glGetUniformLocation(m_Program, "model_tint");
			if (uniTint > -1)
			{
				glUniform4fv(uniTint, 1, glm::value_ptr(tint));
			}

			glBindVertexArray(m_Vao);
			glDrawArrays(GL_TRIANGLES, 0, m_NumVerts);
		}
	}
};

struct MeshNodeVisitor
{
	virtual void operator()(MeshNode*) {}
};

struct MeshNode
{
	std::string m_Name;
	std::vector<SubMesh*> m_SubMeshes;
	glm::mat4 m_Transform;
	std::vector<MeshNode*> m_Children;
	unsigned int m_NumVerts;
	math::AABB m_AABB;

	MeshNode(SubMesh* sub_mesh)
	{
		m_SubMeshes.push_back(sub_mesh);
		m_NumVerts = sub_mesh->m_NumVerts;
		m_AABB.Expand(sub_mesh->m_AABB);
	}

	MeshNode(aiNode* node, const std::vector<SubMesh*>& meshes)
	{
		Logging::Log("Mesh", std::string("Node ") + node->mName.C_Str());
		m_Name = node->mName.C_Str();
		CopyMat(node->mTransformation, m_Transform);
		std::stringstream str;
		str << "Meshnode " << node->mName.C_Str() << " transform:\n";
		str << "[" << m_Transform[0][0] << ", " << m_Transform[1][0] << ", " << m_Transform[2][0] << ", " << m_Transform[3][0] << "\n";
		str << "[" << m_Transform[0][1] << ", " << m_Transform[1][1] << ", " << m_Transform[2][1] << ", " << m_Transform[3][1] << "\n";
		str << "[" << m_Transform[0][2] << ", " << m_Transform[1][2] << ", " << m_Transform[2][2] << ", " << m_Transform[3][2] << "\n";
		str << "[" << m_Transform[0][3] << ", " << m_Transform[1][3] << ", " << m_Transform[2][3] << ", " << m_Transform[3][3] << "\n";
		Logging::Log("MeshNode", str.str());
		m_NumVerts = 0;
		for (unsigned int mesh_idx = 0; mesh_idx < node->mNumMeshes; ++mesh_idx)
		{
			SubMesh* submesh = meshes[*(node->mMeshes + mesh_idx)];
			m_SubMeshes.push_back(submesh);
			m_NumVerts += submesh->m_NumVerts;
			m_AABB.Expand(submesh->m_AABB);
		}
		for (unsigned int child_idx = 0; child_idx < node->mNumChildren; ++child_idx)
		{
			MeshNode* child_node = MemNew(MemoryPool::Rendering, MeshNode)(*(node->mChildren + child_idx), meshes);
			m_Children.push_back(child_node);
			m_NumVerts += child_node->m_NumVerts;
			m_AABB.Expand(child_node->m_AABB);
		}

		str.str("");
		str << "Node " << m_Name << " loaded, " << m_NumVerts << " verts in " << m_SubMeshes.size() << " meshes";
		Logging::Log("Mesh", str.str());
		str.str("");
		str << "AABB (" << m_AABB.lbb.x << ", " << m_AABB.lbb.y << ", " << m_AABB.lbb.z << ") -> (" << m_AABB.rtf.x << ", " << m_AABB.rtf.y << ", " << m_AABB.rtf.z << ")";
		Logging::Log("Mesh", str.str());
		str.str("");
		glm::vec3 centre = glm::vec3(m_AABB.lbb + m_AABB.rtf) / 2.0f;
		str << "Centre: " << centre.x << ", " << centre.y << ", " << centre.z;
		Logging::Log("Mesh", str.str());
		str.str("");
		glm::vec3 half_dims(m_AABB.rtf.x - centre.x, m_AABB.rtf.y - centre.y, m_AABB.rtf.z - centre.z);
		str << "Half dims: " << half_dims.x << ", " << half_dims.y << ", " << half_dims.z;
		Logging::Log("Mesh", str.str());
	}

	~MeshNode()
	{
		for (auto child : m_Children)
		{
			MemDelete(child);
		}
	}

	void Render(glm::mat4 transform, const glm::vec4& tint)
	{
		transform = transform * m_Transform;
		
		for (auto sub_mesh : m_SubMeshes)
		{
			sub_mesh->Render(transform, tint);
		}

		for (auto child_node : m_Children)
		{
			child_node->Render(transform, tint);
		}
	}

	void Visit(MeshNodeVisitor& visitor)
	{
		visitor(this);
		for (auto child_node : m_Children)
		{
			child_node->Visit(visitor);
		}
	}
};


Mesh::Mesh()
	: m_RootNode(nullptr)
{
}

Mesh::~Mesh()
{
	if (m_RootNode)
	{
		MemDelete(m_RootNode);
	}
	for (auto submesh : m_SubMeshes)
	{
		MemDelete(submesh);
	}
}


void Mesh::LoadFromYaml(const std::string& filename)
{
	m_MeshName = filename;

	std::string mesh_filename = "data/meshes/" + filename + ".yaml";
	YAML::Node node = YAML::LoadFile(mesh_filename);

	// Load attribute bindings
	std::vector<AttributeBinding> attribute_bindings;
	int total_floats = 0;
	for (unsigned int i = 0; i < node["attributes"].size(); ++i)
	{
		auto binding = node["attributes"][i];
		AttributeBinding ab = { binding["name"].as<std::string>(), binding["floats"].as<int>() };
		attribute_bindings.push_back(ab);
		total_floats += ab.num_floats;
	}

	// Shaders
	GLuint program = ShaderManager::LoadProgram(node["shader"].as<std::string>());

	// Vertices
	unsigned int num_verts = 0;
	unsigned int num_meshes = 0;
	if (node["vertices"])
	{
		SubMesh* sub_mesh = MemNew(MemoryPool::Rendering, SubMesh)(node, program, attribute_bindings);
		m_SubMeshes.push_back(sub_mesh);
		m_RootNode = MemNew(MemoryPool::Rendering, MeshNode)(sub_mesh);
		num_verts = sub_mesh->m_NumVerts;
		num_meshes = 1;
	}
	else if (node["obj"])
	{
		Assimp::Importer importer;
		std::string obj_filename = "data/meshes/" + node["obj"].as<std::string>();
		const aiScene* scene = importer.ReadFile(obj_filename, aiProcess_Triangulate + aiProcess_GenNormals);

		// Make VAOs for all the meshes.
		for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx)
		{
			aiMesh* mesh = scene->mMeshes[mesh_idx];
			SubMesh* sub_mesh = MemNew(MemoryPool::Rendering, SubMesh)(mesh, program, attribute_bindings);
			m_SubMeshes.push_back(sub_mesh);
			num_verts += sub_mesh->m_NumVerts;
		}
		num_meshes = scene->mNumMeshes;

		m_RootNode = MemNew(MemoryPool::Rendering, MeshNode)(scene->mRootNode, m_SubMeshes);
	}

	struct NodeGatherVisitor : public MeshNodeVisitor
	{
		std::map<std::string, MeshNode*> m_NodesByName;
		virtual void operator()(MeshNode* mesh_node) override
		{
			m_NodesByName[mesh_node->m_Name] = mesh_node;
		}
	};

	NodeGatherVisitor ngv;
	m_RootNode->Visit(ngv);
	m_NodesByName = ngv.m_NodesByName;
}

void Mesh::Render(const glm::mat4& world_transform, const glm::vec4& tint)
{
	m_RootNode->Render(world_transform, tint);
}

void Mesh::SetLocalPoses(const std::map<std::string, glm::mat4>& local_poses)
{
	for (auto local_pose : local_poses)
	{
		MeshNode* node = m_NodesByName[local_pose.first];
		glm::mat4 node_transform = node->m_Transform;
		glm::mat4 new_node_transform = local_pose.second;
		node->m_Transform = new_node_transform;
	}
}

std::map<std::string, glm::mat4> Mesh::GetLocalPoses(const std::vector<std::string>& node_names)
{
	std::map<std::string, glm::mat4> local_poses;
	for (auto node_name : node_names)
	{
		MeshNode* node = m_NodesByName[node_name];
		glm::mat4 node_transform = node->m_Transform;
		local_poses[node_name] = node_transform;
	}
	return local_poses;
}

std::map<std::string, math::AABB> Mesh::GetLocalAABBs(const std::vector<std::string>& node_names)
{
	std::map<std::string, math::AABB> local_aabbs;
	for (auto node_name : node_names)
	{
		MeshNode* node = m_NodesByName[node_name];
		math::AABB node_aabb = node->m_AABB;
		local_aabbs[node_name] = node_aabb;
	}
	return local_aabbs;
}

unsigned int Mesh::GetNumVerts() const
{
	return m_RootNode->m_NumVerts;
}
