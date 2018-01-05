class Entity;
class Message;

class Component
{
public:
	Component(Entity*);
	virtual ~Component();

	void OnMessage(Message*);
};
