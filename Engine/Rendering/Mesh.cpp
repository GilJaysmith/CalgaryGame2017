#include "Engine/Pch.h"

#include "Engine/Rendering/Mesh.h"


Mesh::Mesh()
{
}

Mesh::~Mesh()
{

}

void Mesh::CreateFromVertexArray(float* vertices, unsigned int num_vertices, GLuint shader_program)
{
	m_ShaderProgram = shader_program;
	m_NumTriangles = num_vertices / (8 * sizeof(float));

	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, num_vertices, vertices, GL_STATIC_DRAW);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	GLint colAttrib = glGetAttribLocation(shader_program, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLint texAttrib = glGetAttribLocation(shader_program, "texcoord");
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