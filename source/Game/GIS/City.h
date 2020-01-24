#pragma once

#include <fstream>
#include <vector>

#include "Engine/Rendering/Renderable.h"

namespace physx
{
	class PxRigidStatic;
}

struct CityObjectData
{
	std::vector<std::pair<glm::vec3, glm::vec3>> wall_vertices;
	std::vector<std::pair<glm::vec3, glm::vec3>> roof_vertices;
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
	void AddObject(const CityObjectData& object_data);
	void Finalize();
	bool IsFinalized() const { return m_Finalized; }

	static const int CITY_COOKED_VERSION = 2;

protected:

	GLuint m_ShaderProgram;

	class CityObject : public Renderable
	{
	public:
		GLuint vao;
		unsigned int num_triangles;
		GLuint shader_program;
	public:
		virtual void Render() const override;
		virtual bool IsActive() const override;
		virtual unsigned int GetNumMeshes() const override;
		virtual unsigned int GetNumVerts() const override;
	};

	std::vector<CityObject> m_Objects;
	std::vector<physx::PxRigidStatic*> m_PhysicsObjects;

	CityCooker* m_Cooker;
	bool m_Finalized;
};