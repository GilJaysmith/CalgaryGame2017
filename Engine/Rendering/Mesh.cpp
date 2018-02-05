#include "Engine/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

#include "sdks/assimp/include/Importer.hpp"
#include "sdks/assimp/include/scene.h"
#include "sdks/assimp/include/postprocess.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

#include <vector>


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::LoadFromYaml(const std::string& filename)
{
	m_MeshName = filename;

	std::string mesh_filename = "data/meshes/" + filename + ".yaml";
	YAML::Node node = YAML::LoadFile(mesh_filename);

	// Load attribute bindings
	struct AttributeBinding
	{
		std::string name;
		int num_floats;
	};
	std::vector<AttributeBinding> attribute_bindings;
	int total_floats = 0;
	for (unsigned int i = 0; i < node["attributes"].size(); ++i)
	{
		auto binding = node["attributes"][i];
		AttributeBinding ab = { binding["name"].as<std::string>(), binding["floats"].as<int>() };
		attribute_bindings.push_back(ab);
		total_floats += ab.num_floats;
	}

	// Vertices
	std::vector<std::vector<float>> final_vertices;
	if (node["vertices"])
	{
		for (auto vert : node["vertices"])
		{
			final_vertices.push_back(vert.as<std::vector<float>>());
		}
	}
	else if (node["obj"])
	{
		Assimp::Importer importer;
		std::string obj_filename = "data/meshes/" + node["obj"].as<std::string>();
		const aiScene* scene = importer.ReadFile(obj_filename, aiProcess_Triangulate);
		for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; ++mesh_idx)
		{
			const aiMesh* mesh = scene->mMeshes[mesh_idx];
			aiVector3D* vertices = mesh->mVertices;
			for (unsigned int face_idx = 0; face_idx < mesh->mNumFaces; ++face_idx)
			{
				aiFace& face = mesh->mFaces[face_idx];
				unsigned int* vert_indices = face.mIndices;
				float c = rand() / (float)RAND_MAX;
				for (unsigned int vert_idx_loop = 0; vert_idx_loop < face.mNumIndices; ++vert_idx_loop)
				{
					unsigned int vert_idx = vert_indices[vert_idx_loop];
					aiVector3D vert = vertices[vert_idx];
					std::vector<float> this_vert = { vert.x, vert.y, vert.z, c, c, c };
					final_vertices.push_back(this_vert);
				}
			}
		}
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

	// Shaders
	m_ShaderProgram = ShaderManager::LoadProgram(node["shader"].as<std::string>());

	// Textures
	unsigned int texture_index = 0;
	for (auto texture : node["textures"])
	{
		auto texture_id = TextureManager::LoadTexture(texture.as<std::string>());
		SetTexture(texture_index, texture_id);
		++texture_index;
	}

	// Apply attribute bindings;
	int offset = 0;
	for (auto ab : attribute_bindings)
	{
		GLint posAttrib = glGetAttribLocation(m_ShaderProgram, ab.name.c_str());
		if (posAttrib != -1)
		{
			glVertexAttribPointer(posAttrib, ab.num_floats, GL_FLOAT, GL_FALSE, total_floats * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
			glEnableVertexAttribArray(posAttrib);
		}
		offset += ab.num_floats;
	}

	MemDelete(vert_data);
}

void Mesh::SetTexture(unsigned int texture_index, GLuint texture_id)
{
	m_Textures[texture_index] = texture_id;
}

void Mesh::Render(const glm::mat4& world_transform, const glm::vec4& tint)
{
	ShaderManager::SetActiveShader(m_ShaderProgram);

	for (auto it : m_Textures)
	{
		glActiveTexture(GL_TEXTURE0 + it.first);
		glBindTexture(GL_TEXTURE_2D, it.second);
	}

	GLint uniModel = glGetUniformLocation(m_ShaderProgram, "model");
	if (uniModel > -1)
	{
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(world_transform));
	}

	GLint uniTint = glGetUniformLocation(m_ShaderProgram, "tint");
	if (uniTint > -1)
	{
		glUniform4fv(uniTint, 1, glm::value_ptr(tint));
	}

	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, m_NumVerts);


	ShaderManager::SetActiveShader(0);
}