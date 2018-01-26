// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/GameStateManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

#include "Game/Game.h"
#include "Game/GameStates/TestGameState.h"
#include "Game/GIS/GISManager.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Memory::RegisterAllocator(MemoryPool::System, Memory::CreateFreeStoreAllocator());
	Memory::RegisterAllocator(MemoryPool::Physics, Memory::CreateFreeStoreAllocator());
	Memory::RegisterAllocator(MemoryPool::Rendering, Memory::CreateFreeStoreAllocator());

	Physics::Initialize();
	Renderer::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	MeshManager::Initialize();
	Input::Initialize();
	GameStateManager::Initialize();
	EntityManager::Initialize();
	GIS::Initialize();

	GIS::LoadCity("vancouver");

	ComponentRegistry::RegisterComponentType("render", RenderComponent::CreateComponent);

	int x = 1024;
	int y = 768;
	bool full_screen = false;
	Renderer::CreateWindow(x, y, full_screen);

	TestGameState* test_game_state = MemNew(MemoryPool::System, TestGameState);
	GameStateManager::RequestGameState(test_game_state);

	Time last_frame_time = Time::fromSeconds(1.0f / 60.0f);

	while (1)
	{
		GLFWwindow* window = Renderer::GetWindow();
		
		Input::Update(window);

		if (!GameStateManager::Update(last_frame_time))
		{
			break;
		}

		EntityManager::Update(last_frame_time);
		EntityManager::DestroyPendingEntities();

		glfwSwapBuffers(window);
		Renderer::ClearWindow();
		Renderer::RenderScene();

		glfwPollEvents();

		if (Input::GetKeyEvent(GLFW_KEY_ESCAPE) == Input::PRESSED)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (glfwWindowShouldClose(window))
		{
			GameStateManager::RequestGameState(nullptr);
		}
	}

	GIS::Terminate();
	EntityManager::Terminate();
	GameStateManager::Terminate();
	Input::Terminate();
	MeshManager::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();

	return 0;
}
