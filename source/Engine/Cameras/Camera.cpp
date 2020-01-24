#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/Viewport.h"


Camera::Camera()
	: m_Active(false)
{
	unsigned int width, height;
	Renderer::GetWindowDimensions(width, height);
	m_ProjMatrix = glm::perspective(glm::radians(45.0f), width / (float)height, 1.0f, 10000.0f);
}

Camera::~Camera()
{

}

glm::mat4 Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

glm::mat4 Camera::GetProjMatrix() const
{
	return m_ProjMatrix;
}

void Camera::SetActive(bool active)
{
	m_Active = active;
	OnSetActive(active);
}