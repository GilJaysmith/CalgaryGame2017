#pragma once

#include "Engine/Entities/Message.h"
#include "Engine/Entities/MessageTypes.h"

namespace RenderMessageSubtype
{
	enum TYPE
	{
		SetLocalPoses,
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