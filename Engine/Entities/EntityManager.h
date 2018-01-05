class Entity;

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	Entity* CreateEntity();
	void DestroyEntity(Entity* entity);
	
};
