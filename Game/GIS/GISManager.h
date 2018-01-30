#pragma once


namespace GIS
{
	void Initialize();

	void Terminate();

	void Render();

	void LoadCity(const std::string& city);

	void UnloadCity();
}