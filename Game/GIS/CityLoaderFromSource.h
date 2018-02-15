#pragma once

#include "sdks/glm/glm.hpp"


class City;

class CityLoaderFromSource
{
public:
	CityLoaderFromSource(City& city, const std::string& city_name);

protected:

	struct VectorObject {
		std::vector<glm::dvec3> points;
		double ground_height;
		double roof_height;
		glm::vec3 tint;
	};

	void GenerateMesh(const std::vector<VectorObject>& objects, bool is_contour);
	bool NearEnough(const glm::dvec3& p1, const glm::dvec3& p2);

	City& m_City;
};
