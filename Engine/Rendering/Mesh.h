#pragma once

#include <map>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadFromYaml(const std::string& filename);
	void Render(glm::mat4 world_transform);

	GLuint GetShaderProgram() const { return m_ShaderProgram;  }

protected:

	void CreateEBO(unsigned int* elements, unsigned int num_elements);
	void SetTexture(unsigned int texture_index, GLuint texture_id);

	GLuint m_Vao;
	GLuint m_ShaderProgram;
	unsigned int m_NumTriangles;
	std::map<unsigned int, GLuint> m_Textures;
};