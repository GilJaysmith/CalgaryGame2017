#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"

#include "sdks/libyaml/include/yaml-cpp/yaml.h"


Component* RenderComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Rendering, RenderComponent)(owner, properties);
}

RenderComponent::~RenderComponent()
{
	if (m_Renderable)
	{
		MemDelete(m_Renderable);
	}
}

void RenderComponent::OnUpdate(const Time& elapsed_time, UpdatePass::TYPE update_pass)
{
	switch (update_pass)
	{
		case UpdatePass::AfterPhysics:
		{
			const glm::mat4& world_transform = m_Entity->GetTransform();
			m_Renderable->SetTransform(world_transform);
		}
	}
}

RenderComponent::RenderComponent(Entity* owner, const YAML::Node& properties)
	: Component(owner)
{
	for (auto property : properties)
	{
		std::string property_name = property.first.as<std::string>();
		if (property_name == "mesh")
		{
			std::string mesh_name = property.second.as<std::string>();
			Mesh* mesh = MeshManager::LoadMeshFromFile(mesh_name);
			m_Renderable = MemNew(MemoryPool::Rendering, Renderable)(mesh);
			Renderer::RegisterRenderable(m_Renderable);
		}
	}
}

void RenderComponent::OnSetActive(bool active)
{
	if (m_Renderable)
	{
		m_Renderable->SetActive(active);
	}
}
