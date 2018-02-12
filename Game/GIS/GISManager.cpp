#include "Game/Pch.h"

#include "Engine/Logging/Logging.h"
#include "Engine/Memory/Memory.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/ShaderManager.h"

#include "Game/GIS/GISManager.h"
#include "Game/GIS/shapelib/shapefil.h"

#include "Game/GIS/polypartition/polypartition.h"

#include "PxPhysicsAPI.h"
#include "foundation/PxFoundation.h"

namespace GIS
{
	GLuint s_ShaderProgram;

	void Initialize()
	{
		s_ShaderProgram = ShaderManager::LoadProgram("SolidColourShader");
	}

	void Terminate()
	{
	}

	struct Building
	{
		GLuint vao;
		unsigned int num_triangles;
	};

	std::vector<Building> s_Buildings;

	void GenerateMesh(const std::vector<glm::dvec3>& points, double height, const glm::vec3& tint)
	{
		Building building;

		double OVERALL_SCALE = 0.1;
		double HEIGHT_SCALE = 2.0;

		height *= OVERALL_SCALE;
		height *= HEIGHT_SCALE;

		// Convert building verts to worldspace floorverts/roofverts.
		std::vector<std::pair<glm::vec3, glm::vec3>> vertices;

		std::vector<glm::vec3> m_FloorVerts;
		std::vector<glm::vec3> m_RoofVerts;

		std::vector<TPPLPoint> roof_points;

		for (glm::dvec3 point : points)
		{
			m_FloorVerts.push_back(glm::vec3(float(point.x) * OVERALL_SCALE, 0.0f, float(point.z) * OVERALL_SCALE));
			m_RoofVerts.push_back(glm::vec3(float(point.x) * OVERALL_SCALE, height, float(point.z) * OVERALL_SCALE));
					
			// Save out a point for the roof point.
			TPPLPoint roof_point;
			roof_point.x = point.x * OVERALL_SCALE;
			roof_point.y = point.z * OVERALL_SCALE;
			roof_points.push_back(roof_point);
		}

		// Close the loop if necessary.
		glm::vec3 first_vert = m_FloorVerts[0];
		glm::vec3 last_vert = m_FloorVerts.back();
		if (first_vert == last_vert)
		{
			roof_points.erase(roof_points.begin());
		}

		// Make poly describing roof.
		TPPLPoly roof_poly;
		roof_poly.Init((long)roof_points.size());
		for (int i = 0; i < roof_points.size(); ++i)
		{
			roof_poly[i] = roof_points[i];
		}
		roof_poly.SetOrientation(TPPL_CCW);

		// Triangulate roof poly.
		std::list<TPPLPoly> roof_triangles;
		TPPLPartition triangulator;
		int triangulation_successful = triangulator.Triangulate_MONO(&roof_poly, &roof_triangles);
		if (triangulation_successful == 1)
		{
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

			// Add verts for roof triangles, with new colour.
//			glm::vec3 roof_tint = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
			glm::vec3 roof_tint = tint;
			for (auto triangle : roof_triangles)
			{
				assert(triangle.GetNumPoints() == 3);
				vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[0].x, height, triangle[0].y), roof_tint));
				vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[1].x, height, triangle[1].y), roof_tint));
				vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[2].x, height, triangle[2].y), roof_tint));
			}

			glGenVertexArrays(1, &building.vao);
			glBindVertexArray(building.vao);

			// Vertices
			building.num_triangles = (int)vertices.size();

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

			int vert_data_size = (int)(vertices.size() * 6 * sizeof(float));
			glBufferData(GL_ARRAY_BUFFER, vert_data_size, vert_data, GL_STATIC_DRAW);

			MemDelete(vert_data);

			// Apply attribute bindings
			GLint posAttrib = glGetAttribLocation(s_ShaderProgram, "position");
			if (posAttrib > -1)
			{
				glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
				glEnableVertexAttribArray(posAttrib);
			}

			GLint colourAttrib = glGetAttribLocation(s_ShaderProgram, "colour");
			if (colourAttrib > -1)
			{
				glVertexAttribPointer(colourAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(colourAttrib);
			}

			s_Buildings.push_back(building);

			// Physics.
			physx::PxVec3* physx_vert_data = (physx::PxVec3*)MemNewBytes(MemoryPool::Rendering, sizeof(physx::PxVec3) * vertices.size());
			physx::PxU32* physx_tri_data = (physx::PxU32*)MemNewBytes(MemoryPool::Rendering, sizeof(physx::PxU32) * vertices.size());

			physx::PxVec3* physx_vert_data_p = physx_vert_data;
			physx::PxU32* physx_tri_data_p = physx_tri_data;

			physx::PxU32 vert_idx = 0;
			for (auto vert : vertices)
			{
				glm::vec3 pos = vert.first;
				physx::PxVec3 vec(pos.x, pos.y, pos.z);
				*physx_vert_data_p++ = vec;
				*physx_tri_data_p++ = vert_idx++;
			}

			for (auto vert_idx = 0; vert_idx < vertices.size(); vert_idx += 3)
			{
				std::swap(physx_tri_data[vert_idx], physx_tri_data[vert_idx + 2]);
			}

			physx::PxTriangleMeshDesc meshDesc;
			meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
			meshDesc.points.stride = static_cast<physx::PxU32>(sizeof(physx::PxVec3));
			meshDesc.points.data = physx_vert_data;

			meshDesc.triangles.count = static_cast<physx::PxU32>(vertices.size() / 3);
			meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
			meshDesc.triangles.data = physx_tri_data;

			physx::PxDefaultMemoryOutputStream writeBuffer;
			physx::PxTriangleMeshCookingResult::Enum result;
			bool status = Physics::GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);

			MemDelete(physx_vert_data);
			MemDelete(physx_tri_data);

			physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

			physx::PxTriangleMesh* triangle_mesh = Physics::GetPhysics()->createTriangleMesh(readBuffer);
			std::string material_name = "floor";
			physx::PxShape* shape = Physics::GetPhysics()->createShape(physx::PxTriangleMeshGeometry(triangle_mesh), *Physics::GetMaterial(material_name));
			physx::PxRigidStatic* m_StaticActor = physx::PxCreateStatic(*Physics::GetPhysics(), physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f)), *shape);

			Physics::GetScene()->addActor(*m_StaticActor);
		}
		else
		{
			Logging::Log("GIS", "Couldn't triangulate roof.");
		}
	}

	void Render()
	{
		ShaderManager::SetActiveShader(s_ShaderProgram);
		for (auto building : s_Buildings)
		{
			glBindVertexArray(building.vao);
			glDrawArrays(GL_TRIANGLES, 0, building.num_triangles);
		}
	}

	bool NearEnough(const glm::dvec3& p1, const glm::dvec3& p2)
	{
		const double TOLERANCE = 2;
		return (abs(p1.x - p2.x) < TOLERANCE
			&& p1.y == p2.y
			&& abs(p1.z - p2.z) < TOLERANCE);
	}

	void LoadCity(const std::string& city)
	{
		//if (false)
		//{
		//	std::string city_path = "Data/Shapefiles/" + city + "/buildings.shp";
		//	SHPHandle shape_file = SHPOpen(city_path.c_str(), "r");
		//	int num_entities;
		//	int shape_type;
		//	double min_bound[4];
		//	double max_bound[4];
		//	SHPGetInfo(shape_file, &num_entities, &shape_type, min_bound, max_bound);
		//	double x_centre = (max_bound[0] + min_bound[0]) / 2.0;
		//	double y_centre = (max_bound[1] + min_bound[1]) / 2.0;

		//	const int NUM_BUILDINGS_IN_BLOCK = 1000;
		//	for (int i = 0; i < shape_file->nRecords; ++i)
		//	{
		//		std::vector<SHPObject*> objects;
		//		std::vector<float> heights;
		//		for (int j = 0; j < NUM_BUILDINGS_IN_BLOCK; j++)
		//		{
		//			if (i + j == shape_file->nRecords)
		//			{
		//				break;
		//			}
		//			SHPObject* object = SHPReadObject(shape_file, i + j);
		//			objects.push_back(object);
		//			float height = 3.0f + 3.f * rand() / (float)RAND_MAX;
		//			heights.push_back(height);
		//		}
		//		GenerateMesh(objects, heights, true);
		//		for (auto object : objects)
		//		{
		//			SHPDestroyObject(object);
		//		}

		//		std::stringstream str;
		//		str << "Creating buildings... block " << i;
		//		Logging::Log("GIS", str.str());
		//	}

		//	SHPClose(shape_file);
		//}

		if (true)
		{
			// Contours.
			std::string contours_path = "Data/Shapefiles/" + city + "/2-metre_contour_lines.shp";
			SHPHandle contours_file = SHPOpen(contours_path.c_str(), "r");
			std::string contours_dbf = "Data/Shapefiles/" + city + "/2-metre_contour_lines.dbf";
			DBFHandle dbf_file = DBFOpen(contours_dbf.c_str(), "r");

			// Read header, set bounds.
			int num_entities;
			int shape_type;
			double min_bound[4];
			double max_bound[4];
			SHPGetInfo(contours_file, &num_entities, &shape_type, min_bound, max_bound);
			double x_centre = (max_bound[0] + min_bound[0]) / 2.0;
			double y_centre = (max_bound[1] + min_bound[1]) / 2.0;

			struct Contour {
				std::vector<glm::dvec3> points;
				double height;
			};

			// Read contour data.
			std::vector<Contour> contours;
			for (int i = 0; i < contours_file->nRecords; ++i)
			{
				Contour contour;
				double elevation = DBFReadDoubleAttribute(dbf_file, i, 0);
				contour.height = elevation;
				SHPObject* object = SHPReadObject(contours_file, i);
				assert(object->nParts == 1);
				for (int i = object->panPartStart[0]; i < object->nVertices; ++i)
				{
					double x = object->padfX[i] - x_centre;
					double y = object->padfY[i] - y_centre;
					contour.points.push_back(glm::dvec3(x, elevation, -y));
				}
				if (contour.points.size() >= 2 && elevation >= 0.5f)
				{
					contours.push_back(contour);
				}
				else
				{
					Logging::Log("GIS", "Contour rejected, <2 points!");
				}
				SHPDestroyObject(object);
			}

			SHPClose(contours_file);
			DBFClose(dbf_file);

			// Loop through the contours looking for those where the first and last points are the same, and move them to a separate list.
			std::vector<Contour> final_contours;

			bool something_moved = true;
			while (something_moved)
			{
				something_moved = false;
				for (auto it = contours.begin(); it != contours.end(); )
				{
					Contour& contour = *it;
					if (NearEnough(contour.points[0], contour.points.back()))
					{
						final_contours.push_back(contour);
						it = contours.erase(it);
						Logging::Log("GIS", "Found closed contour!");
						something_moved = true;
					}
					else
					{
						++it;
					}
				}
				for (auto it = contours.begin(); it != contours.end(); )
				{
					Contour& contour = *it;
					bool contour_moved = false;
					for (auto candidate_it = contours.begin(); candidate_it != contours.end(); ++candidate_it)
					{
						if (it != candidate_it)
						{
							Contour& candidate = *candidate_it;
							// Contour could be appended to this contour.
							if (NearEnough(candidate.points.back(), contour.points[0]))
							{
								contour.points.erase(contour.points.begin());
								candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
								contour_moved = true;
								Logging::Log("GIS", "Contour joined!");
								break;
							}
							// Contour could be appended in reverse to this contour.
							else if (NearEnough(candidate.points.back(), contour.points.back()))
							{
								std::reverse(contour.points.begin(), contour.points.end());
								contour.points.erase(contour.points.begin());
								candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
								contour_moved = true;
								Logging::Log("GIS", "Contour joined in reverse!");
								break;
							}
							// Contour could be prepared to this contour.
							else if (NearEnough(candidate.points.front(), contour.points[0]))
							{
								std::reverse(candidate.points.begin(), candidate.points.end());
								contour.points.erase(contour.points.begin());
								candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
								contour_moved = true;
								Logging::Log("GIS", "Contour joined in reverse!");
								break;
							}
							// Contour could be prepared to this contour.
							else if (NearEnough(candidate.points.front(), contour.points.back()))
							{
								std::reverse(candidate.points.begin(), candidate.points.end());
								std::reverse(contour.points.begin(), contour.points.end());
								contour.points.erase(contour.points.begin());
								candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
								contour_moved = true;
								Logging::Log("GIS", "Contour joined in reverse!");
								break;
							}
						}
					}
					if (contour_moved)
					{
						it = contours.erase(it);
						something_moved = true;
					}
					else
					{
						++it;
					}
				}
			}

			// Final pass: we have a lot of data that's cut off at the right, so let's try to join that up.
			for (auto& contour : contours)
			{
				if (contour.points[0].x == contour.points.back().x)
				{
					contour.points.push_back(contour.points[0]);
				}
				final_contours.push_back(contour);
			}

			// Go for it.
			int n = 0;
			for (auto& contour : final_contours)
			{
				glm::vec3 tint(rand() / (float) RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
				GenerateMesh(contour.points, contour.height, tint);
				//++n;
				//std::stringstream str;
				//str << "Generated contour " << n << " of " << final_contours.size();
				//Logging::Log("GIS", str.str());
			}

			Logging::Log("GIS", "Combining finished!");
		}
	}

	void UnloadCity()
	{
		for (auto building : s_Buildings)
		{
			glDeleteVertexArrays(1, &building.vao);
		}
		s_Buildings.clear();
	}
}
