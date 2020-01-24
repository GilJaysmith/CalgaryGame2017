#include "Game/Pch.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleFilterShader.h"
#include "Engine/Vehicles/Nvidia/SnippetVehicleSceneQuery.h"
#include "Game/GIS/City.h"

#include "sdks/PhysX/Include/PxPhysicsAPI.h"

#include <ios>

City::City()
	: m_Finalized(false)
{
}

City::~City()
{
	Unload();
}

void City::Initialize(CityCooker* cooker)
{
	m_Cooker = cooker;
	m_ShaderProgram = ShaderManager::LoadProgram("SolidColour");
}

void City::Unload()
{
	for (auto& object : m_Objects)
	{
		Renderer::UnregisterRenderable(&object);
		glDeleteVertexArrays(1, &object.vao);
	}
	m_Objects.clear();
	m_Finalized = false;
}

void City::CityObject::Render() const
{
	ShaderManager::SetActiveShader(shader_program);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, num_triangles);
}

bool City::CityObject::IsActive() const
{
	return true;
}

unsigned int City::CityObject::GetNumMeshes() const
{
	return 1;
}

unsigned int City::CityObject::GetNumVerts() const
{
	return num_triangles * 3;
}



physx::PxTriangleMesh* MakeTriangleMeshFromVerts(const std::vector<std::pair<glm::vec3, glm::vec3>>& vertices)
{
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
	return triangle_mesh;
}

void City::AddObject(const CityObjectData& object_data)
{
	if (m_Cooker)
	{
		m_Cooker->AddObject(object_data);
	}

	CityObject city_object;
	city_object.shader_program = m_ShaderProgram;

	glGenVertexArrays(1, &city_object.vao);
	glBindVertexArray(city_object.vao);

	// Vertices
	city_object.num_triangles = (int)(object_data.wall_vertices.size() + object_data.roof_vertices.size());

	float* vert_data = (float*)MemNewBytes(MemoryPool::Rendering, sizeof(float) * city_object.num_triangles * 6);
	float* v = vert_data;
	for (auto vert : object_data.wall_vertices)
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
	for (auto vert : object_data.roof_vertices)
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

	int vert_data_size = (int)(city_object.num_triangles * 6 * sizeof(float));
	glBufferData(GL_ARRAY_BUFFER, vert_data_size, vert_data, GL_STATIC_DRAW);

	MemDelete(vert_data);

	// Apply attribute bindings
	GLint posAttrib = glGetAttribLocation(m_ShaderProgram, "position");
	if (posAttrib > -1)
	{
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
		glEnableVertexAttribArray(posAttrib);
	}

	GLint colourAttrib = glGetAttribLocation(m_ShaderProgram, "colour");
	if (colourAttrib > -1)
	{
		glVertexAttribPointer(colourAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(colourAttrib);
	}

	m_Objects.push_back(city_object);

	// Physics for walls.
	{
		physx::PxTriangleMesh* triangle_mesh = MakeTriangleMeshFromVerts(object_data.wall_vertices);
		std::string material_name = "floor";
		physx::PxShape* shape = Physics::GetPhysics()->createShape(physx::PxTriangleMeshGeometry(triangle_mesh), *Physics::GetMaterial(material_name));
		physx::PxFilterData sim_filter_data(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
		shape->setSimulationFilterData(sim_filter_data);
		physx::PxFilterData query_filter_data(0, 0, 0, snippetvehicle::DRIVABLE_SURFACE);
		shape->setQueryFilterData(query_filter_data);
		physx::PxRigidStatic* static_actor = physx::PxCreateStatic(*Physics::GetPhysics(), physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f)), *shape);
		static_actor->userData = nullptr;
		Physics::GetScene()->addActor(*static_actor);
		m_PhysicsObjects.push_back(static_actor);
	}

	// Physics for roofs.
	{
		physx::PxTriangleMesh* triangle_mesh = MakeTriangleMeshFromVerts(object_data.roof_vertices);
		std::string material_name = "floor";
		physx::PxShape* shape = Physics::GetPhysics()->createShape(physx::PxTriangleMeshGeometry(triangle_mesh), *Physics::GetMaterial(material_name));
		physx::PxFilterData sim_filter_data(snippetvehicle::COLLISION_FLAG_GROUND, snippetvehicle::COLLISION_FLAG_GROUND_AGAINST, 0, 0);
		shape->setSimulationFilterData(sim_filter_data);
		physx::PxFilterData query_filter_data(0, 0, 0, snippetvehicle::DRIVABLE_SURFACE);
		shape->setQueryFilterData(query_filter_data);
		physx::PxRigidStatic* static_actor = physx::PxCreateStatic(*Physics::GetPhysics(), physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f)), *shape);
		static_actor->userData = nullptr;
		Physics::GetScene()->addActor(*static_actor);
		m_PhysicsObjects.push_back(static_actor);
	}
}

void City::Finalize()
{
	if (m_Cooker)
	{
		m_Cooker->Finalize();
		m_Cooker = nullptr;
	}
	m_Finalized = true;
	for (auto& city_object : m_Objects)
	{
		Renderer::RegisterRenderable(&city_object);
	}
}


CityCooker::CityCooker(const std::string& city_name)
{
	m_CookingStream.open("Data/Shapefiles/" + city_name + "/cooked.bin", std::ios::out | std::ios::binary);
}

void CityCooker::AddObject(const CityObjectData& object_data)
{
	m_Objects.push_back(object_data);
}

void CityCooker::Finalize()
{
	int city_version = City::CITY_COOKED_VERSION;
	m_CookingStream.write((char*)&city_version, sizeof(city_version));

	// Write out:
	// - number of objects (uint)
	// for each object:
	// - number of verts
	// for each vert:
	// - position (3 floats)
	// - tint (3 floats)
	size_t num_objects = m_Objects.size();
	m_CookingStream.write((char*)&num_objects, sizeof(num_objects));
	for (auto object : m_Objects)
	{
		// Walls
		size_t num_verts = object.wall_vertices.size();
		m_CookingStream.write((char*)&num_verts, sizeof(num_verts));
		for (auto vert : object.wall_vertices)
		{
			glm::vec3 pos = vert.first;
			glm::vec3 tint = vert.second;
			m_CookingStream.write((char*)&pos, sizeof(pos));
			m_CookingStream.write((char*)&tint, sizeof(tint));
		}
		// Roofs
		num_verts = object.roof_vertices.size();
		m_CookingStream.write((char*)&num_verts, sizeof(num_verts));
		for (auto vert : object.roof_vertices)
		{
			glm::vec3 pos = vert.first;
			glm::vec3 tint = vert.second;
			m_CookingStream.write((char*)&pos, sizeof(pos));
			m_CookingStream.write((char*)&tint, sizeof(tint));
		}
	}

	m_CookingStream.close();
}