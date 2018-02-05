#pragma once

#include <map>

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadFromYaml(const std::string& filename);
	void Render(const glm::mat4& world_transform, const glm::vec4& tint);

protected:

	void SetTexture(unsigned int texture_index, GLuint texture_id);

	std::string m_MeshName;
	GLuint m_Vao;
	GLuint m_ShaderProgram;
	unsigned int m_NumVerts;
	std::map<unsigned int, GLuint> m_Textures;
};