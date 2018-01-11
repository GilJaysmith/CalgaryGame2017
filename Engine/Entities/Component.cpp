#include "Pch.h"
#include "Entities/Component.h"

Component::Component(Entity * owner)
	: m_Entity(owner)
{
}

Component::~Component()
{
}
