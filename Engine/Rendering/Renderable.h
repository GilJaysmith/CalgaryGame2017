#pragma once

class Mesh;

class Renderable
{
public:
	Renderable(Mesh* mesh);
	~Renderable();

	void SetTransform(const glm::mat4& world_transform);

	void Render();

protected:
	Mesh* m_Mesh;
	glm::vec4 m_Tint;
	glm::mat4 m_WorldTransform;
};