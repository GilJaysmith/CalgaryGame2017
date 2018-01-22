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
	glm::mat4 m_WorldTransform;
};