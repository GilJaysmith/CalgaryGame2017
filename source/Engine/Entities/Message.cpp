#include "Engine/Pch.h"

#include "Engine/Entities/Message.h"

Message::Message(int message_type, int message_subtype)
	: m_MessageType(message_type)
	, m_MessageSubtype(message_subtype)
{
}

Message::~Message()
{
}
