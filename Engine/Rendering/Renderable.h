#pragma once


class Renderable
{
public:
	Renderable();
	~Renderable();

	void CreateFromVertexArray(float* vertices, size_t num_verts, GLuint shader_program);
	void CreateEBO(unsigned int* elements, size_t num_elements);

	void Render(const glm::mat4& world_pos);

protected:
	GLuint m_Vao;
	GLuint m_ShaderProgram;
	unsigned int m_NumTriangles;
};