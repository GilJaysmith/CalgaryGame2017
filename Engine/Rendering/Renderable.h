#pragma once


class Renderable
{
public:
	Renderable(Mesh* mesh);
	~Renderable();

	void Render(const glm::mat4& world_transform);

protected:
	Mesh * m_Mesh;
};