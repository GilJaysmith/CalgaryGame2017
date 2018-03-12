#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"
#include "Engine/Math/AABB.h"

namespace RenderMessageSubtype
{
	enum TYPE
	{
		SetLocalPoses,
		GetLocalPoses,
		GetLocalAABBs,
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
		, m_NodeNames(node_names)
	{}
	std::vector<std::string> m_NodeNames;
	std::map<std::string, glm::mat4> m_LocalPoses;
};

struct Message_RenderGetLocalAABBs : Message
{
	Message_RenderGetLocalAABBs(const std::vector<std::string>& node_names)
		: Message(MessageType::Render, RenderMessageSubtype::GetLocalAABBs)
		, m_NodeNames(node_names)
	{}
	std::vector<std::string> m_NodeNames;
	std::map<std::string, math::AABB> m_LocalAABBs;
};
