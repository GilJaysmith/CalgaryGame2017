#include "Engine/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

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
	std::string mesh_filename = "data/meshes/" + filename + ".yaml";
	YAML::Node node = YAML::LoadFile(mesh_filename);

	// Shaders
	m_ShaderProgram = ShaderManager::LoadProgram(node["shader"].as<std::string>());

	// Load attribute bindings
	struct AttributeBinding
	{
		std::string name;
		int num_floats;
	};
	std::vector<AttributeBinding> attribute_bindings;
	for (unsigned int i = 0; i < node["attributes"].size(); ++i)
	{
		auto binding = node["attributes"][i];
		AttributeBinding ab = { binding["name"].as<std::string>(), binding["floats"].as<int>() };
		attribute_bindings.push_back(ab);
	}
	int total_floats = 0;
	for (auto ab : attribute_bindings)
	{
		total_floats += ab.num_floats;
	}

	// Textures
	unsigned int texture_index = 0;
	for (auto texture : node["textures"])
	{
		auto texture_id = TextureManager::LoadTexture(texture.as<std::string>());
		SetTexture(GL_TEXTURE0 + texture_index, texture_id);
		++texture_index;
	}

	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	// Vertices
	std::vector<std::vector<float>> vertices;
	for (auto vert : node["vertices"])
	{
		vertices.push_back(vert.as<std::vector<float>>());
	}
	m_NumTriangles = vertices.size();

	float* vert_data = (float*) MemNewBytes(MemoryPool::Rendering, sizeof(float) * vertices.size() * vertices[0].size());
	float* v = vert_data;
	for (auto vert : vertices)
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

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	int vert_data_size = vertices.size() * vertices[0].size() * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, vert_data_size, vert_data, GL_STATIC_DRAW);

	MemDelete(vert_data);

	// Apply attribute bindings
	int offset = 0;
	for (auto ab : attribute_bindings)
	{
		GLint posAttrib = glGetAttribLocation(m_ShaderProgram, ab.name.c_str());
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, ab.num_floats, GL_FLOAT, GL_FALSE, total_floats * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
		offset += ab.num_floats;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::CreateEBO(unsigned int* elements, unsigned int num_elements)
{
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_elements, elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
		glActiveTexture(it.first);
		glBindTexture(GL_TEXTURE_2D, it.second);
	}

	GLint uniModel = glGetUniformLocation(m_ShaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(world_transform));

	GLint uniTint = glGetUniformLocation(m_ShaderProgram, "tint");
	glUniform4fv(uniTint, 1, glm::value_ptr(tint));

	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, m_NumTriangles);
	glBindVertexArray(0);
}