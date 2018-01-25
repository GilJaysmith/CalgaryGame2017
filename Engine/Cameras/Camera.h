#pragma once


class Camera
{
public:
	Camera();
	~Camera();

	void SetViewMatrix(const glm::mat4& view_matrix);
	glm::mat4 GetViewMatrix() const;

protected:
	glm::mat4 m_ViewMatrix;
};
