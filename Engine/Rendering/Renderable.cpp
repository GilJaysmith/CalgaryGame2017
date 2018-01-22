#pragma once

#include "Engine/Pch.h"

#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/Renderable.h"


Renderable::Renderable(Mesh* mesh)
	: m_Mesh(mesh)
{
}

Renderable::~Renderable()
{

}

void Renderable::Render(const glm::mat4& world_transform)
{
	m_Mesh->Render(world_transform);
}
