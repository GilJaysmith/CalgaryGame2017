#pragma once


namespace GIS
{
	void Initialize();

	void Terminate();

	void Render();

	void LoadCityFromCooked(const std::string& city);
	void LoadCityFromSource(const std::string& city);

	void UnloadCity();
}