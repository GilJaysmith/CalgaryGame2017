#pragma once

#include <map>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void CreateFromVertexArray(float* vertices, unsigned int num_vertices, GLuint shader_program);
	void CreateEBO(unsigned int* elements, unsigned int num_elements);
	void SetTexture(unsigned int texture_index, GLuint texture_id);
	void Render(glm::mat4 world_transform);

protected:
	GLuint m_Vao;
	GLuint m_ShaderProgram;
	unsigned int m_NumTriangles;
	std::map<unsigned int, GLuint> m_Textures;
};