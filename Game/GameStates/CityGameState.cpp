#include "Game/Pch.h"

#include "Engine/Cameras/DebugCamera.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

#include "Game/GameStates/CityGameState.h"
#include "Game/GIS/GISManager.h"

CityGameState::CityGameState()
{
}

CityGameState::~CityGameState()
{

}

void CityGameState::OnEnter()
{
	Mesh* mesh = MeshManager::LoadMeshFromFile("low-poly-car");

	Entity* m_GroundPlane = Entity::CreateEntity("sea", glm::mat4());

	GIS::LoadCity("Vancouver");

	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);
}

bool CityGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	GIS::Render();
	return true;
}

void CityGameState::OnExit()
{
	MemDelete(m_Camera);

	GIS::UnloadCity();
}
