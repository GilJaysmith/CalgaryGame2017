#include "Engine/Pch.h"

#include "Engine/Entities/Component.h"

Component::Component(Entity * owner)
	: m_Entity(owner)
	, m_Active(true)
{
}

Component::~Component()
{
}

void Component::SetActive(bool active)
{
	m_Active = active;
	OnSetActive(active);
}
