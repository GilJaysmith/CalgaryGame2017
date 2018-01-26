#pragma once

class Mesh;

class Renderable
{
public:
	Renderable(Mesh* mesh);
	~Renderable();

	void SetTransform(const glm::mat4& world_transform);

	void Render();

	void SetActive(bool active) { m_Active = active;  }
	bool IsActive() const { return m_Active;  }

protected:
	Mesh* m_Mesh;
	glm::vec4 m_Tint;
	glm::mat4 m_WorldTransform;
	bool m_Active;
};