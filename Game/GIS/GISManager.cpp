#include "Game/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Game/GIS/GISManager.h"
#include "Game/GIS/shapelib/shapefil.h"

#include <iomanip>
#include <sstream>

namespace GIS
{
	void Initialize()
	{

	}

	void Terminate()
	{

	}

	void LoadCity(const std::string& city)
	{
		std::string city_path = "Data/Shapefiles/" + city + "/buildings.shp";
		SHPHandle shape_file = SHPOpen(city_path.c_str(), "r");

		int num_entities;
		int shape_type;
		double min_bound[4];
		double max_bound[4];
		SHPGetInfo(shape_file, &num_entities, &shape_type, min_bound, max_bound);

		// x and y are in longitude and latitude, so we will use -y as our z
		// See https://en.wikipedia.org/wiki/Geographic_coordinate_system#Expressing_latitude_and_longitude_as_linear_units for conversion.
		int i = 40657;
		{
			SHPObject* object = SHPReadObject(shape_file, i);
			std::stringstream s;
			s << "Object " << i << " num verts " << object->nVertices;
			Logging::Log("GIS", s.str());
			for (int v = 0; v < object->nVertices; ++v)
			{
				std::stringstream s;
				s  << "x " << std::setprecision(17) << object->padfX[v] << " y " << std::setprecision(17) << object->padfY[v] << " z " << std::setprecision(17) << object->padfZ[v];
				Logging::Log("GIS", s.str());
			}
			SHPDestroyObject(object);
		}

		SHPClose(shape_file);
	}
}
