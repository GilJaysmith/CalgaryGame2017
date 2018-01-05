#include <vector>

#include "Math/Types.h"

class Component;
class Message;

class Entity
{
public:

	Entity();
	~Entity();

	void AddComponent(Component* component);
	void OnMessage(Message* message);

	Math::Vector3 GetTranslation() const;
	Math::Vector3 GetRotation() const;

protected:

	std::vector<Component*> m_Components;
	Math::Vector3 m_Translation;
	Math::Vector3 m_Rotation;
};
