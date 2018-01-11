#pragma once

class Entity;
class Message;

class Component
{
public:
	Component(Entity* owner);
	virtual ~Component();

	virtual bool OnMessage(Message*) { return false; }
	virtual void OnUpdate(float elapsed_time) {}

protected:
	const Entity * m_Entity;
};
