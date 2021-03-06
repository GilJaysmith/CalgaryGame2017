// Game.cpp : Defines the entry point for the application.

#include "Game/Pch.h"

#include <chrono>

#include "Engine/Audio/Audio.h"
#include "Engine/Audio/Music.h"
#include "Engine/Cameras/Camera.h"
#include "Engine/Cameras/CameraManager.h"
#include "Engine/DebugDraw/DebugDraw.h"
#include "Engine/DebugPanels/DebugPanels.h"
#include "Engine/Entities/ComponentRegistry.h"
#include "Engine/Entities/EntityManager.h"
#include "Engine/GameStates/GameStateManager.h"
#include "Engine/GameStates/SplashScreenGameState.h"
#include "Engine/Health/HealthComponent.h"
#include "Engine/Input/Input.h"
#include "Engine/Physics/CollisionResponseComponent.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Physics/PhysicsComponent.h"
#include "Engine/Physics/TriggerComponent.h"
#include "Engine/Rendering/MeshManager.h"
#include "Engine/Rendering/Postprocessing/PostProcessEffect.h"
#include "Engine/Rendering/Postprocessing/PostProcessFade.h"
#include "Engine/Rendering/RenderComponent.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"
#include "Engine/Rendering/TextureManager.h"
#include "Engine/Vehicles/VehicleAIComponent.h"
#include "Engine/Vehicles/VehicleAudioComponent.h"
#include "Engine/Vehicles/VehicleComponent.h"
#include "Engine/Vehicles/VehicleInputComponent.h"

#include "Game/Game.h"
#include "Game/GameStates/CityGameState.h"
#include "Game/GameStates/RaceGameState.h"
#include "Game/GameStates/TestGameState.h"
#include "Game/GIS/GISManager.h"
#include "Game/Race/CheckpointComponent.h"


int main(int argc, char** argv)
{
	Memory::Initialize();

	Memory::RegisterAllocator(MemoryPool::System, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Physics, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::Multithreaded);
	Memory::RegisterAllocator(MemoryPool::Rendering, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Entities, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Audio, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);
	Memory::RegisterAllocator(MemoryPool::Vehicles, Memory::CreateFreeStoreAllocator(), MemoryAllocatorThreadSafeness::SingleThreaded);

	Audio::Initialize();
	Music::Initialize();
	Physics::Initialize();
	Renderer::Initialize();
	ShaderManager::Initialize();
	TextureManager::Initialize();
	DebugDraw::Initialize();
	ScreenSpaceRenderer::Initialize();
	DebugPanels::Initialize();
	MeshManager::Initialize();
	Input::Initialize();
	GameStateManager::Initialize();
	EntityManager::Initialize();
	GIS::Initialize();
	CameraManager::Initialize();

	ComponentRegistry::RegisterComponentType("checkpoint", CheckpointComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("collisionresponse", CollisionResponseComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("health", HealthComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("physics", PhysicsComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("render", RenderComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("trigger", TriggerComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("vehicle", VehicleComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("vehicleai", VehicleAIComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("vehicleaudio", VehicleAudioComponent::CreateComponent);
	ComponentRegistry::RegisterComponentType("vehicleinput", VehicleInputComponent::CreateComponent);

	SplashScreenGameState* splash_screen_game_state = MemNew(MemoryPool::System, SplashScreenGameState)("Intro1.png", Time::fromSeconds(5.0f));
	//GameStateManager::QueueGameState(splash_screen_game_state);
	RaceGameState* game_state = MemNew(MemoryPool::System, RaceGameState);
//	TestGameState* game_state = MemNew(MemoryPool::System, TestGameState);
	GameStateManager::QueueGameState(game_state);

	Time last_frame_time = Time::fromSeconds(1.0f / 60.0f);
	GameStateManager::Update(last_frame_time);

	const int DELAY_FRAMES = 1;
	int delay_frames = DELAY_FRAMES;

	glm::vec3 last_camera_position(0.0f, 0.0f, 0.0f);
	Camera* active_camera = Renderer::GetActiveCamera();
	if (active_camera)
	{
		last_camera_position = active_camera->GetPosition();
	}

	//PostProcessEffect* ppe = MemNew(MemoryPool::Rendering, PostProcessEffect)("PostprocessKernel", 10.0f);
	//Renderer::InsertPostProcessEffect(ppe);
	//PostProcessEffect* ppe2 = MemNew(MemoryPool::Rendering, PostProcessEffect)("PostprocessKernel", 20.0f);
	//Renderer::InsertPostProcessEffect(ppe2);
	PostProcessEffect* ppe = MemNew(MemoryPool::Rendering, PostProcessFadeIn)(glm::vec3(1.0f, 1.0f, 1.0f), 5.0f);
	Renderer::InsertPostProcessEffect(ppe);


	while (1)
	{
		--delay_frames;

		auto frame_start_time = std::chrono::system_clock::now();

		GLFWwindow* window = Renderer::GetWindow();

		DebugPanels::Update(last_frame_time);

		CameraManager::Update(last_frame_time);
		Camera* active_camera = Renderer::GetActiveCamera();
		if (active_camera)
		{
			glm::vec3 current_camera_position = active_camera->GetPosition();
			glm::vec3 current_camera_velocity = (current_camera_position - last_camera_position) * last_frame_time.toMilliseconds();
			glm::vec3 current_camera_forward = active_camera->GetForwardVector();
			current_camera_forward.y = 0.0f;
			current_camera_forward = glm::normalize(current_camera_forward);
			Audio::SetListenerPosition(current_camera_position, current_camera_velocity, current_camera_forward);
			last_camera_position = current_camera_position;
		}

		Music::Update(last_frame_time);
		Audio::Update(last_frame_time);

		Input::Update(window);

		{
			Time logical_frame_time = delay_frames ? Time::fromMilliseconds(0.0f) : last_frame_time;
			if (!GameStateManager::Update(logical_frame_time))
			{
				break;
			}
			EntityManager::Update(logical_frame_time, UpdatePass::BeforePhysics);
			Physics::Simulate(logical_frame_time);
			EntityManager::Update(logical_frame_time, UpdatePass::AfterPhysics);
			EntityManager::DestroyPendingEntities();
		}

		glfwSwapBuffers(window);

		Renderer::RenderScene(last_frame_time);

		glfwPollEvents();

		if (Input::JustPressed(GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (glfwWindowShouldClose(window))
		{
			GameStateManager::ForceQuit();
		}

		auto frame_end_time = std::chrono::system_clock::now();
		auto frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end_time - frame_start_time).count() / 1000.0f;
		last_frame_time = Time::fromSeconds(glm::clamp(frame_time, 1.0f / 60.0f, 1.0f / 20.0f));

		if (delay_frames == 0)
		{
			delay_frames = DELAY_FRAMES;
		}
	}

	MemDelete(game_state);
	MemDelete(splash_screen_game_state);

	CameraManager::Terminate();
	GIS::Terminate();
	EntityManager::Terminate();
	GameStateManager::Terminate();
	Input::Terminate();
	MeshManager::Terminate();
	DebugPanels::Terminate();
	DebugDraw::Terminate();
	ScreenSpaceRenderer::Terminate();
	TextureManager::Terminate();
	ShaderManager::Terminate();
	Renderer::Terminate();
	Physics::Terminate();
	Music::Terminate();
	Audio::Terminate();
	Memory::Terminate();

	return 0;
}
