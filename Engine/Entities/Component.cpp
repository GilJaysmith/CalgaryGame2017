#include "Engine/Pch.h"

#include "Engine/Entities/Component.h"

Component::Component(Entity * owner)
	: m_Entity(owner)
{
}

Component::~Component()
{
}
