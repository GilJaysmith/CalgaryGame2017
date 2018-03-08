#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"

namespace RenderMessageSubtype
{
	enum TYPE
	{
		SetLocalPoses,
		GetLocalPoses,
		COUNT
	};
};

struct Message_RenderSetLocalPoses : Message
{
	Message_RenderSetLocalPoses(const std::map<std::string, glm::mat4>& local_poses)
		: Message(MessageType::Render, RenderMessageSubtype::SetLocalPoses)
		, m_LocalPoses(local_poses)
	{}
	std::map<std::string, glm::mat4> m_LocalPoses;
};

struct Message_RenderGetLocalPoses : Message
{
	Message_RenderGetLocalPoses(const std::vector<std::string>& node_names)
		: Message(MessageType::Render, RenderMessageSubtype::GetLocalPoses)
	{
		for (auto node_name : node_names)
		{
			m_LocalPoses[node_name] = glm::mat4();
		}
	}
	std::map<std::string, glm::mat4> m_LocalPoses;
};