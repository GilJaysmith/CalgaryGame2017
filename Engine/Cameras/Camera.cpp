#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"


Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::SetViewMatrix(const glm::mat4& view_matrix)
{
	m_ViewMatrix = view_matrix;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}
