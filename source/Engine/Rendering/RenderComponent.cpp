#include "Engine/Pch.h"

#include "Engine/Entities/Entity.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/RenderMessages.h"

#include "yaml-cpp/yaml.h"


Component* RenderComponent::CreateComponent(Entity* owner, const YAML::Node& properties)
{
	return MemNew(MemoryPool::Rendering, RenderComponent)(owner, properties);
}

RenderComponent::~RenderComponent()
{
	if (m_Renderable)
	{
		Renderer::UnregisterRenderable(m_Renderable);
		MemDelete(m_Renderable);
	}
}

bool RenderComponent::OnMessage(Message* message)
{
	if (message->GetMessageType() == MessageType::Render)
	{
		switch (message->GetMessageSubtype())
		{
			case RenderMessageSubtype::SetLocalPoses:
			{
				// Tell our renderable to update its local poses.
				Message_RenderSetLocalPoses* rslp = dynamic_cast<Message_RenderSetLocalPoses*>(message);
				m_Renderable->SetLocalPoses(rslp->m_LocalPoses);
				break;
			}

			case RenderMessageSubtype::GetLocalPoses:
			{
				Message_RenderGetLocalPoses* rglp = dynamic_cast<Message_RenderGetLocalPoses*>(message);
				rglp->m_LocalPoses = m_Renderable->GetLocalPoses(rglp->m_NodeNames);
				break;
			}

			case RenderMessageSubtype::GetLocalAABBs:
			{
				Message_RenderGetLocalAABBs* rgla = dynamic_cast<Message_RenderGetLocalAABBs*>(message);
				rgla->m_LocalAABBs = m_Renderable->GetLocalAABBs(rgla->m_NodeNames);
				break;
			}
		}
	}
	return false;
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
			m_Renderable = MemNew(MemoryPool::Rendering, RenderableMesh)(mesh);
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
