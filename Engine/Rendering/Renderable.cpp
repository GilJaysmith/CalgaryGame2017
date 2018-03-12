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

void RenderableMesh::SetLocalPoses(const std::map<std::string, glm::mat4>& local_poses)
{
	m_Mesh->SetLocalPoses(local_poses);
}

std::map<std::string, glm::mat4> RenderableMesh::GetLocalPoses(const std::vector<std::string>& node_names)
{
	return m_Mesh->GetLocalPoses(node_names);
}

std::map<std::string, math::AABB> RenderableMesh::GetLocalAABBs(const std::vector<std::string>& node_names)
{
	return m_Mesh->GetLocalAABBs(node_names);
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
