#include "Game/Pch.h"

#include "Game/GIS/City.h"
#include "Game/GIS/CityLoaderFromCooked.h"

#include <fstream>
#include <ios>

#include <time.h>

CityLoaderFromCooked::CityLoaderFromCooked(City& city, const std::string& city_name)
{
	time_t start_time;
	time(&start_time);

	std::ifstream cooked_stream;
	cooked_stream.open("Data/Shapefiles/" + city_name + "/cooked.bin", std::ios::in | std::ios::binary);

	char buffer[262144];
	cooked_stream.rdbuf()->pubsetbuf(buffer, sizeof(buffer));

	Logging::Log("CityLoaderFromCooked", "Creating objects...");
	size_t num_objects;
	cooked_stream.read((char*)&num_objects, sizeof(num_objects));
	CityObjectData object_data;
	for (int i = 0; i < num_objects; ++i)
	{
		size_t num_verts;
		// Walls.
		cooked_stream.read((char*)&num_verts, sizeof(num_objects));
		object_data.wall_vertices.clear();
		object_data.wall_vertices.reserve(num_verts);
		for (int v = 0; v < num_verts; ++v)
		{
			glm::vec3 pos, tint;
			cooked_stream.read((char*)&pos, sizeof(pos));
			cooked_stream.read((char*)&tint, sizeof(tint));
			object_data.wall_vertices.push_back(std::pair<glm::vec3, glm::vec3>(pos, tint));
		}
		// Roofs.
		cooked_stream.read((char*)&num_verts, sizeof(num_objects));
		object_data.roof_vertices.clear();
		object_data.roof_vertices.reserve(num_verts);
		for (int v = 0; v < num_verts; ++v)
		{
			glm::vec3 pos, tint;
			cooked_stream.read((char*)&pos, sizeof(pos));
			cooked_stream.read((char*)&tint, sizeof(tint));
			object_data.roof_vertices.push_back(std::pair<glm::vec3, glm::vec3>(pos, tint));
		}
		city.AddObject(object_data);
	}

	city.Finalize();

	time_t end_time;
	time(&end_time);
	double diff_time = difftime(end_time, start_time);
}