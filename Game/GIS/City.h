#pragma once

#include <fstream>
#include <vector>


namespace physx
{
	class PxRigidStatic;
}

struct CityObjectData
{
	std::vector<std::pair<glm::vec3, glm::vec3>> vertices;
};

struct CityCooker
{
	CityCooker(const std::string& city_name);
	void AddObject(const CityObjectData& object_data);
	void Finalize();

	std::vector<CityObjectData> m_Objects;
	std::ofstream m_CookingStream;
};

class City
{
public:

	City();
	~City();

	void Initialize(CityCooker* cooker = nullptr);
	void Unload();
	void Render() const;
	void AddObject(const CityObjectData& object_data);
	void Finalize();

protected:

	GLuint m_ShaderProgram;

	struct CityObject
	{
		GLuint vao;
		unsigned int num_triangles;
	};

	std::vector<CityObject> m_Objects;
	std::vector<physx::PxRigidStatic*> m_PhysicsObjects;

	CityCooker* m_Cooker;
	bool m_Finalized;
};