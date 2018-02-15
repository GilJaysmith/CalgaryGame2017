
#include "Game/Pch.h"

#include "Game/GIS/City.h"
#include "Game/GIS/CityLoaderFromCooked.h"
#include "Game/GIS/CityLoaderFromSource.h"
#include "Game/GIS/GISManager.h"


namespace GIS
{
	City s_City;

	void Initialize()
	{
	}

	void Terminate()
	{
		UnloadCity();
	}

	void Render()
	{
		s_City.Render();
	}

	void LoadCityFromSource(const std::string& city_name)
	{
		UnloadCity();
		CityCooker cooker(city_name);
		s_City.Initialize(&cooker);
		CityLoaderFromSource loader(s_City, city_name);
	}

	void LoadCityFromCooked(const std::string& city_name)
	{
		UnloadCity();
		s_City.Initialize();
		CityLoaderFromCooked loader(s_City, city_name);
	}

	void UnloadCity()
	{
		s_City.Unload();
	}
}
