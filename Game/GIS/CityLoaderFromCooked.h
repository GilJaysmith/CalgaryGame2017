#pragma once

#include "sdks/glm/glm.hpp"

#include <string>

class City;

class CityLoaderFromCooked
{
public:
	CityLoaderFromCooked(City& city, const std::string& city_name);
	bool Load();

	City& m_City;
	std::string m_CityName;
};
