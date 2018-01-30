// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/GameStateManager.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"

#include "Game/Game.h"
#include "Game/GameStates/CityGameState.h"
#include "Game/GameStates/TestGameState.h"
#include "Game/GIS/GISManager.h"

#include "sdks/assimp/include/Importer.hpp"
#include "sdks/assimp/include/scene.h"
#include "sdks/assimp/include/postprocess.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Memory::Initialize();

	Memory::RegisterAllocator(MemoryPool::System, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Physics, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::Multithreaded);
	Memory::RegisterAllocator(MemoryPool::Rendering, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Entities, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Audio, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);

	Audio::Initialize();
	Physics::Initialize();
	Renderer::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	MeshManager::Initialize();
	Input::Initialize();
	GameStateManager::Initialize();
	EntityManager::Initialize();
	GIS::Initialize();

	Assimp::Importer importer;

	ComponentRegistry::RegisterComponentType("render", RenderComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("physics", PhysicsComponent::CreateComponent);

	int x = 800;
	int y = 600;
	bool full_screen = false;
	Renderer::CreateWindow(x, y, full_screen);

	CityGameState* game_state = MemNew(MemoryPool::System, CityGameState);
	GameStateManager::RequestGameState(game_state);

	Time last_frame_time = Time::fromSeconds(1.0f / 60.0f);

	bool update_physics = false;

	while (1)
	{
		Audio::Update(last_frame_time);

		GLFWwindow* window = Renderer::GetWindow();

		Input::Update(window);

		if (Input::GetKeyEvent(GLFW_KEY_P) == Input::PRESSED)
		{
			update_physics = !update_physics;
		}

		if (!GameStateManager::Update(last_frame_time))
		{
			break;
		}

		if (update_physics)
		{
			Physics::Simulate(last_frame_time);
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

	MemDelete(game_state);

	GIS::Terminate();
	EntityManager::Terminate();
	GameStateManager::Terminate();
	Input::Terminate();
	MeshManager::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();
	Audio::Terminate();
	Memory::Terminate();

	return 0;
}
