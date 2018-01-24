#pragma once

#include <string>

#include "sdks/libyaml/include/yaml-cpp/yaml.h"

class Component;
class Entity;

namespace ComponentRegistry
{
	typedef Component*(*ComponentCreateFunction)(Entity*, const YAML::Node&);

	Component* CreateComponent(Entity* owner, const std::string& component_type, const YAML::Node& definition);
	void RegisterComponentType(const std::string& component_type, ComponentCreateFunction create_function);
}
