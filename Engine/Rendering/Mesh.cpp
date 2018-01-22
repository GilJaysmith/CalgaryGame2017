#include "Engine/Pch.h"

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
	YAML::Node node = YAML::LoadFile(mesh_filename)["mesh"];

	// Shaders
	std::vector<unsigned int> shaders;
	for (auto shader : node["shaders"])
	{
		auto shader_type = shader["type"].as<std::string>() == "vertex" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
		auto shader_name = shader["shader"].as<std::string>();
		shaders.push_back(ShaderManager::LoadShader(shader_name, shader_type));
	}
	m_ShaderProgram = ShaderManager::MakeProgram(shaders, { { 0, "outColor" } });

	// Textures
	unsigned int texture_index = 0;
	for (auto texture : node["textures"])
	{
		auto texture_id = TextureManager::LoadTexture(texture.as<std::string>());
		SetTexture(GL_TEXTURE0 + texture_index, texture_id);
		++texture_index;
	}

	// Verts
	std::vector<std::vector<float>> verts;
	for (auto vert : node["vertices"])
	{
		verts.push_back(vert.as<std::vector<float>>());
	}
	CreateFromVertexArray(verts);
}

void Mesh::CreateFromVertexArray(std::vector<std::vector<float>> vertices)
{
	m_NumTriangles = vertices.size();

	float* vert_data = new float[vertices.size() * vertices[0].size()];
	float* v = vert_data;
	for (auto i : vertices)
	{
		for (auto j : i)
		{
			*v++ = j;
		}
	}

	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	int fc = vertices.size() * vertices[0].size();
	glBufferData(GL_ARRAY_BUFFER, fc * sizeof(float), vert_data, GL_STATIC_DRAW);

	delete[] vert_data;

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(m_ShaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(m_ShaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLint texAttrib = glGetAttribLocation(m_ShaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

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

void Mesh::Render(glm::mat4 world_transform)
{
	glUseProgram(m_ShaderProgram);

	for (auto it : m_Textures)
	{
		glActiveTexture(it.first);
		glBindTexture(it.first, it.second);
	}

	GLint uniModel = glGetUniformLocation(m_ShaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(world_transform));

	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, m_NumTriangles);
	glBindVertexArray(0);
}