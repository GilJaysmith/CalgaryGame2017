#include "Game/Pch.h"

#include "Engine/Physics/Physics.h"
#include "Game/GIS/City.h"
#include "Game/GIS/CityLoaderFromSource.h"
#include "Game/GIS/shapelib/shapefil.h"
#include "Game/GIS/polypartition/polypartition.h"

#include "PxPhysicsAPI.h"
#include "foundation/PxFoundation.h"

#include <time.h>

void CityLoaderFromSource::GenerateMesh(const std::vector<VectorObject>& objects, bool is_contour)
{
	double HEIGHT_SCALE = 1.0;

	std::vector<std::pair<glm::vec3, glm::vec3>> vertices;

	for (auto object : objects)
	{
		double ground_height = object.ground_height;
		ground_height *= HEIGHT_SCALE;

		double roof_height = object.roof_height;
		roof_height *= HEIGHT_SCALE;

		glm::vec3 tint = object.tint;

		// Convert building verts to worldspace floorverts/roofverts.

		std::vector<glm::vec3> m_FloorVerts;
		std::vector<glm::vec3> m_RoofVerts;

		std::vector<TPPLPoint> roof_points;

		for (glm::dvec3 point : object.points)
		{
			m_FloorVerts.push_back(glm::vec3(float(point.x), ground_height, float(point.z)));
			m_RoofVerts.push_back(glm::vec3(float(point.x), roof_height, float(point.z)));

			// Save out a point for the roof point.
			TPPLPoint roof_point;
			roof_point.x = point.x;
			roof_point.y = point.z;
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
		int triangulation_successful;
		if (!is_contour)
		{
			triangulation_successful = triangulator.Triangulate_EC(&roof_poly, &roof_triangles);
		}
		else
		{
			triangulation_successful = triangulator.Triangulate_MONO(&roof_poly, &roof_triangles);
		}
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
				vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[0].x, roof_height, triangle[0].y), roof_tint));
				vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[1].x, roof_height, triangle[1].y), roof_tint));
				vertices.push_back(std::pair<glm::vec3, glm::vec3>(glm::vec3(triangle[2].x, roof_height, triangle[2].y), roof_tint));
			}
		}
		else
		{
			Logging::Log("CityLoaderFromSource", "Couldn't triangulate roof.");
		}
	}

	if (vertices.size() > 0)
	{
		CityObjectData object_data;
		object_data.vertices = vertices;
		m_City.AddObject(object_data);
	}
}


bool CityLoaderFromSource::NearEnough(const glm::dvec3& p1, const glm::dvec3& p2)
{
	const double TOLERANCE = 2;
	return (abs(p1.x - p2.x) < TOLERANCE
		&& p1.y == p2.y
		&& abs(p1.z - p2.z) < TOLERANCE);
}

CityLoaderFromSource::CityLoaderFromSource(City& city, const std::string& city_name)
	: m_City(city)
{
	time_t start_time;
	time(&start_time);

	double OVERALL_SCALE = 0.1;
	float max_contour_height = 0.0f;

	if (true)
	{
		// Contours.
		std::string contours_path = "Data/Shapefiles/" + city_name + "/2-metre_contour_lines-gcs2.shp";
		SHPHandle contours_file = SHPOpen(contours_path.c_str(), "r");
		std::string contours_dbf = "Data/Shapefiles/" + city_name + "/2-metre_contour_lines-gcs2.dbf";
		DBFHandle dbf_file = DBFOpen(contours_dbf.c_str(), "r");

		// Read header, set bounds.
		int num_entities;
		int shape_type;
		double min_bound[4];
		double max_bound[4];
		SHPGetInfo(contours_file, &num_entities, &shape_type, min_bound, max_bound);
		double x_centre = (max_bound[0] + min_bound[0]) / 2.0;
		double y_centre = (max_bound[1] + min_bound[1]) / 2.0;

		// Read contour data.
		std::vector<VectorObject> contours;
		for (int i = 0; i < contours_file->nRecords; ++i)
		{
			VectorObject contour;
			double elevation = DBFReadDoubleAttribute(dbf_file, i, 0);
			contour.ground_height = 0.0f;
			contour.roof_height = elevation * OVERALL_SCALE;

			max_contour_height = glm::max(max_contour_height, static_cast<float>(elevation));

			SHPObject* object = SHPReadObject(contours_file, i);
			if (object && object->nParts == 1)
			{
				assert(object->nParts == 1);
				for (int i = object->panPartStart[0]; i < object->nVertices; ++i)
				{
					double x = object->padfX[i] - x_centre;
					double y = object->padfY[i] - y_centre;

					x *= 111320 * OVERALL_SCALE;
					y *= 110574 * OVERALL_SCALE;

					contour.points.push_back(glm::dvec3(x, elevation, -y));
				}
				if (contour.points.size() >= 2 && elevation >= 0.5f)
				{
					contours.push_back(contour);
				}
				else
				{
					Logging::Log("CityLoaderFromSource", "Contour rejected, <2 points!");
				}
				SHPDestroyObject(object);
			}
		}

		SHPClose(contours_file);
		DBFClose(dbf_file);

		// Loop through the contours looking for those where the first and last points are the same, and move them to a separate list.
		std::vector<VectorObject> final_contours;

		bool something_moved = true;
		while (something_moved)
		{
			something_moved = false;
			for (auto it = contours.begin(); it != contours.end(); )
			{
				VectorObject& contour = *it;
				if (NearEnough(contour.points[0], contour.points.back()))
				{
					final_contours.push_back(contour);
					it = contours.erase(it);
					Logging::Log("CityLoaderFromSource", "Found closed contour!");
					something_moved = true;
				}
				else
				{
					++it;
				}
			}
			for (auto it = contours.begin(); it != contours.end(); )
			{
				VectorObject& contour = *it;
				bool contour_moved = false;
				for (auto candidate_it = contours.begin(); candidate_it != contours.end(); ++candidate_it)
				{
					if (it != candidate_it)
					{
						VectorObject& candidate = *candidate_it;
						// Contour could be appended to this contour.
						if (NearEnough(candidate.points.back(), contour.points[0]))
						{
							contour.points.erase(contour.points.begin());
							candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
							contour_moved = true;
							Logging::Log("CityLoaderFromSource", "Contour joined!");
							break;
						}
						// Contour could be appended in reverse to this contour.
						else if (NearEnough(candidate.points.back(), contour.points.back()))
						{
							std::reverse(contour.points.begin(), contour.points.end());
							contour.points.erase(contour.points.begin());
							candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
							contour_moved = true;
							Logging::Log("CityLoaderFromSource", "Contour joined in reverse!");
							break;
						}
						// Contour could be prepared to this contour.
						else if (NearEnough(candidate.points.front(), contour.points[0]))
						{
							std::reverse(candidate.points.begin(), candidate.points.end());
							contour.points.erase(contour.points.begin());
							candidate.points.insert(candidate.points.end(), contour.points.begin(), contour.points.end());
							contour_moved = true;
							Logging::Log("CityLoaderFromSource", "Contour joined in reverse!");
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
							Logging::Log("CityLoaderFromSource", "Contour joined in reverse!");
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
			glm::vec3 tint(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
//			float c = float((50 + (contour.roof_height / OVERALL_SCALE)) / 256);
//			glm::vec3 tint(c, c, c);
			contour.tint = tint;
			std::vector<VectorObject> objects;
			objects.push_back(contour);
			GenerateMesh(objects, true);

			++n;
			std::stringstream str;
			str << "Generated contour " << n << " of " << final_contours.size();
			Logging::Log("CityLoaderFromSource", str.str());
		}

		Logging::Log("CityLoaderFromSource", "Combining finished!");
	}

	if (true)
	{
		std::string city_path = "Data/Shapefiles/" + city_name + "/buildings-gcs2.shp";
		SHPHandle shape_file = SHPOpen(city_path.c_str(), "r");
		int num_entities;
		int shape_type;
		double min_bound[4];
		double max_bound[4];
		SHPGetInfo(shape_file, &num_entities, &shape_type, min_bound, max_bound);
		double x_centre = (max_bound[0] + min_bound[0]) / 2.0;
		double y_centre = (max_bound[1] + min_bound[1]) / 2.0;

		physx::PxScene* scene = Physics::GetScene();

		const int NUM_BUILDINGS_IN_BLOCK = 1000;
		for (int i = 0; i < shape_file->nRecords; i += NUM_BUILDINGS_IN_BLOCK)
		{
			std::vector<VectorObject> vector_objects;

			for (int j = 0; j < NUM_BUILDINGS_IN_BLOCK; j++)
			{
				if (i + j == shape_file->nRecords)
				{
					break;
				}

				SHPObject* object = SHPReadObject(shape_file, i + j);

				VectorObject vector_object;
				assert(object->nParts == 1);
				for (int v = object->panPartStart[0]; v < object->nVertices; ++v)
				{
					double x = object->padfX[v] - x_centre;
					double y = object->padfY[v] - y_centre;
					x *= 111320 * OVERALL_SCALE;
					y *= 110574 * OVERALL_SCALE;

					vector_object.points.push_back(glm::dvec3(x, 0.0f, -y));
				}

				float min_ground_height = FLT_MAX;

				for (auto v : vector_object.points)
				{
					// Do a raycast down from max_contour_height + 1.0 from these coordinates.
					// Take that as the ground height and pass in the actual height.
					physx::PxVec3 origin((float)v.x, max_contour_height + 1.0f, (float)v.z);
					physx::PxVec3 unitDir(0.0f, -1.0f, 0.0f);
					physx::PxReal maxDistance = max_contour_height + 10.0f;
					physx::PxRaycastBuffer hit;
					bool status = scene->raycast(origin, unitDir, maxDistance, hit);
					float ground_height = -100.0f;
					if (status)
					{
						ground_height = hit.block.position.y;
					}
					min_ground_height = glm::min(min_ground_height, ground_height);
				}

//				std::stringstream str;
//				str << "Building: min height " << min_ground_height << " from " << vector_object.points.size() << " points";
//				Logging::Log("CityLoaderFromSource", str.str());

				float height = 8.0f + (10.0f * rand() / (float)RAND_MAX);
				height *= (float)OVERALL_SCALE;
				vector_object.ground_height = min_ground_height;
				vector_object.roof_height = min_ground_height + height;
				vector_object.tint = glm::vec3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
				//float c = rand() / (float)RAND_MAX;
				//vector_object.tint = glm::vec3(c, c, c);

				vector_objects.push_back(vector_object);
				SHPDestroyObject(object);
			}

			GenerateMesh(vector_objects, false);

			std::stringstream str;
			str << "Creating buildings... block " << i;
			Logging::Log("CityLoaderFromSource", str.str());
		}

		SHPClose(shape_file);
	}

	m_City.Finalize();

	time_t end_time;
	time(&end_time);
	double diff_time = difftime(start_time, end_time);
}
