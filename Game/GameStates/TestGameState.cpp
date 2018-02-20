#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Cameras/DebugCamera.h"
#include "Engine/Entities/Entity.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/Viewport.h"
#include "Engine/Vehicles/VehicleFollowCamera.h"

#include "Game/GameStates/TestGameState.h"

TestGameState::TestGameState()
	: m_Time(0.0f)
{
}

TestGameState::~TestGameState()
{
}


void TestGameState::OnEnter()
{
	Entity::CreateEntity("floor", glm::mat4());

	//for (unsigned int x = 0; x < 7; x++)
	//{
	//	for (unsigned int z = 0; z < 7; z++)
	//	{
	//		glm::mat4 transform = glm::translate(glm::mat4(), glm::vec3(x * 4, 1.0f, z * 8));
	//		Entity::CreateEntity("Porsche_911_GT2", transform);
	//	}
	//}

	Entity* car = Entity::CreateEntity("player_0_car", glm::translate(glm::mat4(), glm::vec3(0.0f, 10.0f, 0.0f)));

	m_Camera = MemNew(MemoryPool::Vehicles, VehicleFollowCamera)(car, 0);

	//Entity::CreateEntity("Porsche_911_GT2", glm::mat4());
//	Entity::CreateEntity("Porsche_911_GT2", glm::translate(glm::mat4(), glm::vec3(5.0f, 0.0f, 0.0f)));

	unsigned int screen_width, screen_height;
	Renderer::GetWindowDimensions(screen_width, screen_height);
	unsigned int viewport_width = screen_width / 2;
	unsigned int viewport_height = screen_height / 2;

//	m_Camera = MemNew(MemoryPool::Rendering, DebugCamera);
	Renderer::SetActiveCamera(m_Camera);

	//Viewport& default_viewport = Renderer::GetViewport("default");
	//default_viewport.SetSize(0, 0);
	//default_viewport.SetSize(viewport_width, viewport_height);

	//Viewport v1(viewport_width, 0, viewport_width, viewport_height);
	//Renderer::RegisterViewport(v1, "v1");

	//Viewport v2(0, viewport_height, viewport_width, viewport_height);
	//Renderer::RegisterViewport(v2, "v2");

	//Viewport v3(viewport_width, viewport_height, viewport_width, viewport_height);
	//Renderer::RegisterViewport(v3, "v3");
}

bool TestGameState::OnUpdate(const Time& frame_time)
{
	m_Time += frame_time.toSeconds();

	GameState::OnUpdate(frame_time);
	m_Camera->Update(frame_time);

	//if (Input::JustPressed(GLFW_KEY_M))
	//{
	//	Audio::PlaySound("a2002011001-e02.wav");
	//}

	//unsigned int screen_width, screen_height;
	//Renderer::GetWindowDimensions(screen_width, screen_height);
	//Viewport& viewport = Renderer::GetViewport("default");
	//GLint x, y;
	//GLsizei width, height;
	////width = GLsizei(float(screen_width / 2) + float(screen_width / 2) * cos(m_Time));
	////height = GLsizei(float(screen_height / 2) + float(screen_height / 2) * sin(m_Time));
	////x = (screen_width - width) / 2;
	////y = (screen_height - height) / 2;
	//width = screen_width / 2 + GLsizei(float(screen_width / 2) * cos(m_Time));
	//height = screen_height / 2 + GLsizei(float(screen_height / 2) * sin(m_Time));
	//x = screen_width / 2 - width / 2;
	//y = screen_height / 2 - height / 2;
	//viewport.SetPosition(x, y);
	//viewport.SetSize(width, height);

	return true;
}

void TestGameState::OnExit()
{
	Renderer::UnregisterViewport("v1");
	Renderer::UnregisterViewport("v2");
	Renderer::UnregisterViewport("v3");

	MemDelete(m_Camera);
}
