#pragma once

#include "Engine/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/Renderable.h"


Renderable::Renderable(Mesh* mesh)
	: m_Mesh(mesh)
	, m_Active(true)
{
	m_Tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

Renderable::~Renderable()
{
}

void Renderable::SetTransform(const glm::mat4& world_transform)
{
	m_WorldTransform = world_transform;
}

void Renderable::Render()
{
	m_Mesh->Render(m_WorldTransform, m_Tint);
}
