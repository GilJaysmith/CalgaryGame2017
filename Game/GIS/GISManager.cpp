#include "Game/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/ShaderManager.h"

#include "Game/GIS/GISManager.h"
#include "Game/GIS/shapelib/shapefil.h"

#include "Game/GIS/polypartition/polypartition.h"


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

	void GenerateMesh(const std::vector<SHPObject*>& objects, const std::vector<float>& heights, bool is_buildings)
	{
		Building building;

		double OVERALL_SCALE = 10.0;

		// Convert building verts to worldspace floorverts/roofverts.
		std::vector<std::pair<glm::vec3, glm::vec3>> vertices;
		assert(objects.size() == heights.size());
		for (int o = 0; o < objects.size(); ++o)
		{
			SHPObject* object = objects[o];
			float height = heights[o] / OVERALL_SCALE;
			if (object && object->nParts >= 1)
			{
				std::vector<glm::vec3> m_FloorVerts;
				std::vector<glm::vec3> m_RoofVerts;

				assert(object->nParts == 1);

				std::vector<TPPLPoint> roof_points;

				for (int i = object->panPartStart[0]; i < object->nVertices; ++i)
				{
					// This is the longitude and latitude based around the centre of the map.
					// x and y are in longitude and latitude, so we will use -y as our z
					// See https://en.wikipedia.org/wiki/Geographic_coordinate_system#Expressing_latitude_and_longitude_as_linear_units for conversion.

					float x_f, z_f;
					if (is_buildings)
					{
						double x = object->padfX[i] - x_centre;
						double y = object->padfY[i] - y_centre;
						x *= 111320;
						y *= 110574;
						x_f = (float)(x / OVERALL_SCALE);
						z_f = (float)(-y / OVERALL_SCALE);
					}
					else
					{
						double x = object->padfX[i] - x_centre;
						double y = object->padfY[i] - y_centre;
						x_f = (float)(x / OVERALL_SCALE);
						z_f = (float)(-y / OVERALL_SCALE);
					}

					m_FloorVerts.push_back(glm::vec3(x_f, 0.0f, z_f));
					m_RoofVerts.push_back(glm::vec3(x_f, height, z_f));
					
					// Save out a point for the roof point.
					TPPLPoint roof_point;
					roof_point.x = x_f;
					roof_point.y = z_f;
					roof_points.push_back(roof_point);
				}

				//// Close the loop if necessary.
				//glm::vec3 first_vert = m_FloorVerts[0];
				//glm::vec3 last_vert = m_FloorVerts.back();
				//if (first_vert != last_vert)
				//{
				//	m_FloorVerts.push_back(m_FloorVerts[0]);
				//	m_RoofVerts.push_back(m_RoofVerts[0]);
				//}

				// Make poly describing roof.
				TPPLPoly roof_poly;
				roof_poly.Init(roof_points.size());
				for (int i = 0; i < roof_points.size(); ++i)
				{
					roof_poly[i] = roof_points[i];
				}
				roof_poly.SetOrientation(TPPL_CCW);

				// Triangulate roof poly.
				std::list<TPPLPoly> roof_triangles;
				TPPLPartition triangulator;
				//int triangulation_successful = triangulator.Triangulate_EC(&roof_poly, &roof_triangles);
				int triangulation_successful = 1;
				if (triangulation_successful == 1)
				{
					// Make vert stream for all the faces in the object.
					float c = (96 + 20 * height) / 256.0f;
					glm::vec3 tint = glm::vec3(c, c, c);
					for (int i = 0; i < m_FloorVerts.size() - 1; ++i)
					{
						vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_RoofVerts[i], tint));
						vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_FloorVerts[i], tint));
						vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_FloorVerts[i + 1], tint));
						vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_FloorVerts[i + 1], tint));
						vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_RoofVerts[i + 1], tint));
						vertices.push_back(std::pair<glm::vec3, glm::vec3>(m_RoofVerts[i], tint));
					}

					// Add verts for roof triangles, with new colour.
					if (false)
					{
						glm::vec3 roof_tint = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
						for (auto triangle : roof_triangles)
						{
							assert(triangle.GetNumPoints() == 3);
							vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[0].x, height, triangle[0].y), roof_tint));
							vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[1].x, height, triangle[1].y), roof_tint));
							vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[2].x, height, triangle[2].y), roof_tint));
						}
					}
				}
				else
				{
					std::stringstream str;
					str << "Couldn't triangulate roof - skipping building " << o;
					Logging::Log("GIS", str.str());
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

		if (false)
		{
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
			for (int i = 0; i < shape_file->nRecords; i += NUM_BUILDINGS_IN_BLOCK)
			{
				std::vector<SHPObject*> objects;
				std::vector<float> heights;
				for (int j = 0; j < NUM_BUILDINGS_IN_BLOCK; j++)
				{
					if (i + j == shape_file->nRecords)
					{
						break;
					}
					SHPObject* object = SHPReadObject(shape_file, i + j);
					objects.push_back(object);
					float height = 3.0f + 3.f * rand() / (float)RAND_MAX;
					heights.push_back(height);
				}
				GenerateMesh(objects, heights, true);
				for (auto object : objects)
				{
					SHPDestroyObject(object);
				}

				std::stringstream str;
				str << "Creating buildings... block " << i;
				Logging::Log("GIS", str.str());
			}

			SHPClose(shape_file);
		}

		if (true)
		{
			// Contours.
			std::string contours_path = "Data/Shapefiles/" + city + "/2-metre_contour_lines.shp";
			SHPHandle contours_file = SHPOpen(contours_path.c_str(), "r");
			std::string contours_dbf = "Data/Shapefiles/" + city + "/2-metre_contour_lines.dbf";
			DBFHandle dbf_file = DBFOpen(contours_dbf.c_str(), "r");

			int num_entities;
			int shape_type;
			double min_bound[4];
			double max_bound[4];
			SHPGetInfo(contours_file, &num_entities, &shape_type, min_bound, max_bound);
			x_centre = (max_bound[0] + min_bound[0]) / 2.0;
			y_centre = (max_bound[1] + min_bound[1]) / 2.0;

			const int NUM_BUILDINGS_IN_BLOCK = 1000;
			for (int i = 0; i < contours_file->nRecords; i += NUM_BUILDINGS_IN_BLOCK)
			{
				std::vector<SHPObject*> objects;
				std::vector<float> heights;
				for (int j = 0; j < NUM_BUILDINGS_IN_BLOCK; j++)
				{
					if (i + j == contours_file->nRecords)
					{
						break;
					}
					SHPObject* object = SHPReadObject(contours_file, i + j);
					objects.push_back(object);
					double elevation = DBFReadDoubleAttribute(dbf_file, i + j, 0);
					heights.push_back(elevation);
				}
				GenerateMesh(objects, heights, false);
				for (auto object : objects)
				{
					SHPDestroyObject(object);
				}

				std::stringstream str;
				str << "Creating contours... block " << i;
				Logging::Log("GIS", str.str());
			}

			SHPClose(contours_file);
			DBFClose(dbf_file);
		}
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
