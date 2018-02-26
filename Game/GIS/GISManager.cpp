
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
		if (s_City.IsFinalized())
		{
			s_City.Render();
		}
	}

	void LoadCityFromSource(const std::string& city_name)
	{
		UnloadCity();
		CityCooker cooker(city_name);
		s_City.Initialize(&cooker);
		CityLoaderFromSource loader(s_City, city_name);
	}

	bool LoadCityFromCooked(const std::string& city_name)
	{
		UnloadCity();
		s_City.Initialize();
		CityLoaderFromCooked loader(s_City, city_name);
		return loader.Load();
	}

	void UnloadCity()
	{
		s_City.Unload();
	}
}
