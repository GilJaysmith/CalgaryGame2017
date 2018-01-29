#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"


Camera::Camera()
	: m_Active(false)
{

}

Camera::~Camera()
{

}

glm::mat4 Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

void Camera::SetActive(bool active)
{
	m_Active = active;
}