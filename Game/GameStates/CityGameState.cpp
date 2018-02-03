#include "Game/Pch.h"

#include "Engine/Cameras/DebugCamera.h"
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
//	GIS::LoadCity("Vancouver");

	unsigned int width, height;
	Renderer::GetWindowDimensions(width, height);

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), width / (float)height, 1.0f, 10000.0f);
	ShaderManager::SetUniform4fv("proj", proj);

	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);

	GLuint texture = TextureManager::LoadTexture("intro1.png");
	ScreenSpaceRenderer::AddSprite(0, 0, texture);
}

void CityGameState::OnUpdate(const Time& frame_time)
{
	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	GIS::Render();
}

void CityGameState::OnExit()
{
	MemDelete(m_Camera);

	GIS::UnloadCity();
}
