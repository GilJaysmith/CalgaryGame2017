#pragma once

#include "Engine/Pch.h"

#include "Engine/Rendering/Mesh.h"
#include "Engine/Rendering/Renderable.h"


RenderableMesh::RenderableMesh(Mesh* mesh)
	: m_Mesh(mesh)
	, m_Active(true)
{
	m_Tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

RenderableMesh::~RenderableMesh()
{
}

void RenderableMesh::SetTransform(const glm::mat4& world_transform)
{
	m_WorldTransform = world_transform;
}

void RenderableMesh::Render() const
{
	m_Mesh->Render(m_WorldTransform, m_Tint);
}

unsigned int RenderableMesh::GetNumMeshes() const
{
	return 1;
}

unsigned int RenderableMesh::GetNumVerts() const
{
	return m_Mesh->GetNumVerts();
}
