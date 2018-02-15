#include "Game/Pch.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Game/GIS/City.h"

#include "PxPhysicsAPI.h"
#include "foundation/PxFoundation.h"

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
	m_ShaderProgram = ShaderManager::LoadProgram("SolidColourShader");
}

void City::Unload()
{
	for (auto object : m_Objects)
	{
		glDeleteVertexArrays(1, &object.vao);
	}
	m_Objects.clear();
	m_Finalized = false;
}

void City::Render() const
{
	assert(m_Finalized);

	ShaderManager::SetActiveShader(m_ShaderProgram);
	for (auto object : m_Objects)
	{
		glBindVertexArray(object.vao);
		glDrawArrays(GL_TRIANGLES, 0, object.num_triangles);
	}
}


void City::AddObject(const CityObjectData& object_data)
{
	if (m_Cooker)
	{
		m_Cooker->AddObject(object_data);
	}

	CityObject city_object;

	glGenVertexArrays(1, &city_object.vao);
	glBindVertexArray(city_object.vao);

	// Vertices
	city_object.num_triangles = (int)object_data.vertices.size();

	float* vert_data = (float*)MemNewBytes(MemoryPool::Rendering, sizeof(float) * object_data.vertices.size() * 6);
	float* v = vert_data;
	for (auto vert : object_data.vertices)
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

	int vert_data_size = (int)(object_data.vertices.size() * 6 * sizeof(float));
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

	// Physics.
	physx::PxVec3* physx_vert_data = (physx::PxVec3*)MemNewBytes(MemoryPool::Rendering, sizeof(physx::PxVec3) * object_data.vertices.size());
	physx::PxU32* physx_tri_data = (physx::PxU32*)MemNewBytes(MemoryPool::Rendering, sizeof(physx::PxU32) * object_data.vertices.size());

	physx::PxVec3* physx_vert_data_p = physx_vert_data;
	physx::PxU32* physx_tri_data_p = physx_tri_data;

	physx::PxU32 vert_idx = 0;
	for (auto vert : object_data.vertices)
	{
		glm::vec3 pos = vert.first;
		physx::PxVec3 vec(pos.x, pos.y, pos.z);
		*physx_vert_data_p++ = vec;
		*physx_tri_data_p++ = vert_idx++;
	}

	for (auto vert_idx = 0; vert_idx < object_data.vertices.size(); vert_idx += 3)
	{
		std::swap(physx_tri_data[vert_idx], physx_tri_data[vert_idx + 2]);
	}

	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<physx::PxU32>(object_data.vertices.size());
	meshDesc.points.stride = static_cast<physx::PxU32>(sizeof(physx::PxVec3));
	meshDesc.points.data = physx_vert_data;

	meshDesc.triangles.count = static_cast<physx::PxU32>(object_data.vertices.size() / 3);
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
	physx::PxRigidStatic* static_actor = physx::PxCreateStatic(*Physics::GetPhysics(), physx::PxTransform(physx::PxVec3(0.0f, 0.0f, 0.0f)), *shape);

	Physics::GetScene()->addActor(*static_actor);
	m_PhysicsObjects.push_back(static_actor);
}

void City::Finalize()
{
	if (m_Cooker)
	{
		m_Cooker->Finalize();
		m_Cooker = nullptr;
	}
	m_Finalized = true;
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
		size_t num_verts = object.vertices.size();
		m_CookingStream.write((char*)&num_verts, sizeof(num_verts));
		for (auto vert : object.vertices)
		{
			glm::vec3 pos = vert.first;
			glm::vec3 tint = vert.second;
			m_CookingStream.write((char*)&pos, sizeof(pos));
			m_CookingStream.write((char*)&tint, sizeof(tint));
		}
	}

	m_CookingStream.close();
}