#include "Engine/Pch.h"

#include "Engine/Entities/ComponentRegistry.h"

namespace ComponentRegistry
{
	std::map<std::string, ComponentCreateFunction> s_Registry;

	Component* CreateComponent(Entity* owner, const std::string& component_type, const YAML::Node& yaml)
	{
		auto it = s_Registry.find(component_type);
		if (it == s_Registry.end())
		{
			Logging::Log("ComponentRegistry", "Can't find component type " + component_type);
			exit(1);
		}
		return it->second(owner, yaml);
	}

	void RegisterComponentType(const std::string& component_type, ComponentCreateFunction create_function)
	{
		s_Registry[component_type] = create_function;
	}
}