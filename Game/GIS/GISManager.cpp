#include "Game/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/ShaderManager.h"

#include "Game/GIS/GISManager.h"
#include "Game/GIS/shapelib/shapefil.h"


namespace GIS
{
	GLuint s_ShaderProgram;

	void Initialize()
	{
	}

	void Terminate()
	{
	}

	struct Building
	{
		GLuint vao;
		unsigned int num_triangles;
	};

	double x_centre;
	double y_centre;
	std::vector<Building> s_Buildings;

	void GenerateMesh(std::vector<SHPObject*> objects)
	{
		Building building;

		// Convert building verts to worldspace floorverts/roofverts.
		std::vector<std::pair<glm::vec3, glm::vec3>> vertices;
		for (auto object : objects)
		{
			if (object->nVertices)
			{
				glm::vec3 tint = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
				std::vector<glm::vec3> m_FloorVerts;
				std::vector<glm::vec3> m_RoofVerts;
				assert(object->nParts == 1);
				float height = 5.0f + 10.f * rand() / (float)RAND_MAX;
				for (int i = 0; i < object->nVertices; ++i)
				{
					// This is the longitude and latitude based around the centre of the map.
					// x and y are in longitude and latitude, so we will use -y as our z
					// See https://en.wikipedia.org/wiki/Geographic_coordinate_system#Expressing_latitude_and_longitude_as_linear_units for conversion.
					double x = object->padfX[i] - x_centre;
					double y = object->padfY[i] - y_centre;
					x *= 111320;
					y *= 110574;
					float x_f = x / 10.0f;
					float z_f = -y / 10.0f;
					m_FloorVerts.push_back(glm::vec3(x_f, 0.0f, z_f));
					m_RoofVerts.push_back(glm::vec3(x_f, height, z_f));
				}
				m_FloorVerts.push_back(m_FloorVerts[0]);
				m_RoofVerts.push_back(m_RoofVerts[0]);

				// Make vert stream for all the faces in the object.
				for (int i = 0; i < m_FloorVerts.size() - 1; ++i)
				{
					vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_RoofVerts[i], tint));
					vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_FloorVerts[i], tint));
					vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_FloorVerts[i + 1], tint));
					vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_FloorVerts[i + 1], tint));
					vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_RoofVerts[i + 1], tint));
					vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_RoofVerts[i], tint));
				}
			}
		}

		glGenVertexArrays(1, &building.vao);
		glBindVertexArray(building.vao);

		// Vertices
		building.num_triangles = vertices.size();

		float* vert_data = (float*)MemNewBytes(MemoryPool::Rendering, sizeof(float) * vertices.size() * 6);
		float* v = vert_data;
		for (auto vert : vertices)
		{
			glm::vec3 pos = vert.first;
			glm::vec3 colour = vert.second;
			*v++ = pos.x;
			*v++ = pos.y;
			*v++ = pos.z;
			*v++ = colour.r;
			*v++ = colour.g;
			*v++ = colour.b;
		}

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		int vert_data_size = vertices.size() * 6 * sizeof(float);
		glBufferData(GL_ARRAY_BUFFER, vert_data_size, vert_data, GL_STATIC_DRAW);

		MemDelete(vert_data);

		// Apply attribute bindings
		GLint posAttrib = glGetAttribLocation(s_ShaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));

		GLint colourAttrib = glGetAttribLocation(s_ShaderProgram, "colour");
		glEnableVertexAttribArray(colourAttrib);
		glVertexAttribPointer(colourAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		s_Buildings.push_back(building);
	}

	void Render()
	{
		ShaderManager::SetActiveShader(s_ShaderProgram);
		for (auto building : s_Buildings)
		{
			glBindVertexArray(building.vao);
			glDrawArrays(GL_TRIANGLES, 0, building.num_triangles);
			glBindVertexArray(0);
		}
	}

	void LoadCity(const std::string& city)
	{
		s_ShaderProgram = ShaderManager::LoadProgram("SolidColourShader");

		std::string city_path = "Data/Shapefiles/" + city + "/buildings.shp";
		SHPHandle shape_file = SHPOpen(city_path.c_str(), "r");

		int num_entities;
		int shape_type;
		double min_bound[4];
		double max_bound[4];
		SHPGetInfo(shape_file, &num_entities, &shape_type, min_bound, max_bound);
		x_centre = (max_bound[0] + min_bound[0]) / 2.0;
		y_centre = (max_bound[1] + min_bound[1]) / 2.0;

		const int NUM_BUILDINGS_IN_BLOCK = 1000;
		for (int i = 0; i < shape_file->nRecords; i+= NUM_BUILDINGS_IN_BLOCK)
		{
			std::vector<SHPObject*> objects;
			for (int j = 0; j < NUM_BUILDINGS_IN_BLOCK; j++)
			{
				if (i + j == shape_file->nRecords)
				{
					break;
				}
				SHPObject* object = SHPReadObject(shape_file, i + j);
				objects.push_back(object);
			}

			GenerateMesh(objects);
			for (auto object : objects)
			{
				SHPDestroyObject(object);
			}

			std::stringstream str;
			str << "Creating buildings... " << i;
			Logging::Log("GIS", str.str());
		}

		SHPClose(shape_file);

		// Contours.
		std::string contours_path = "Data/Shapefiles/" + city + "/10-metre_contour_lines.shp";
		SHPHandle contours_file = SHPOpen(contours_path.c_str(), "r");
		SHPGetInfo(contours_file, &num_entities, &shape_type, min_bound, max_bound);
		x_centre = (max_bound[0] + min_bound[0]) / 2.0;
		y_centre = (max_bound[1] + min_bound[1]) / 2.0;
		for (int i = 0; i < contours_file->nRecords; ++i)
		{
			SHPObject* object = SHPReadObject(contours_file, i);
			// Looks like we just subtract the centre and we're done?
			double x = object->padfX[0] - x_centre;
			double y = object->padfY[0] - y_centre;
			std::stringstream str;
			str << "Contour " << i << " x " << x << " y " << y;
			Logging::Log("GIS", str.str());
		}
		SHPClose(contours_file);

		// Database - to get elevations.
		std::string contours_dbf = "Data/Shapefiles/" + city + "/10-metre_contour_lines.dbf";
		DBFHandle dbf_file = DBFOpen(contours_dbf.c_str(), "r");
		int num_fields = DBFGetFieldCount(dbf_file);
		for (int i = 0; i < num_fields; ++i)
		{
			char field_name[256];
			int width;
			int decimals;
			DBFGetFieldInfo(dbf_file, i, field_name, &width, &decimals);
			int a = 9;
		}
		DBFClose(dbf_file);
	}

	void UnloadCity()
	{
		for (auto building : s_Buildings)
		{
			glDeleteVertexArrays(1, &building.vao);
		}
		s_Buildings.clear();

		glDeleteShader(s_ShaderProgram);
	}
}
