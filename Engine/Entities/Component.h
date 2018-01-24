#pragma once

class Entity;
class Message;
class Time;

class Component
{
public:
	Component(Entity* owner);
	virtual ~Component();

	virtual bool OnMessage(Message*) { return false; }
	virtual void OnUpdate(const Time& elapsed_time) {}

protected:
	const Entity* m_Entity;
};
