#pragma once

class Entity;
class Message;

class Component
{
public:
	Component(Entity*);
	virtual ~Component();

	virtual bool OnMessage(Message*) { return false; }
};
