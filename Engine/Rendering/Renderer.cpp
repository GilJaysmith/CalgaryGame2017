#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/DebugPanels/DebugPanels.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Rendering/ScreenSpaceRenderer.h"
#include "Engine/Rendering/ShaderManager.h"

#include <set>

namespace Renderer
{
	void GLFWErrorFunction(int error, const char* message)
	{
		std::stringstream s;
		s << "Error " << error << " (" << message << ")";
		Logging::Log("GLFW", s.str());
	}

	GLFWwindow* s_Window = nullptr;

	unsigned int s_Width;
	unsigned int s_Height;
	bool s_FullScreen;

	void DestroyWindow()
	{
		if (s_Window)
		{
			glfwDestroyWindow(s_Window);
			s_Window = nullptr;
		}
	}

	void CreateWindow(unsigned int width, unsigned int height, bool full_screen)
	{
		DestroyWindow();

		s_Width = width;
		s_Height = height;
		s_FullScreen = full_screen;

		if (full_screen)
		{
			s_Window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
		}
		else
		{
			s_Window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); // Windowed
		}

		glfwMakeContextCurrent(s_Window);
		glfwSwapInterval(1);
		glEnable(GL_DEPTH_TEST);

		glewExperimental = GL_TRUE;
		glewInit();
	}

	void Initialize()
	{
		glfwSetErrorCallback(GLFWErrorFunction);
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		int x = 800;
		int y = 600;
		bool full_screen = false;
		CreateWindow(x, y, full_screen);
	}

	void Terminate()
	{
		DestroyWindow();
		glfwTerminate();
	}

	void GetWindowDimensions(unsigned int& width, unsigned int& height)
	{
		width = s_Width;
		height = s_Height;
	}

	GLFWwindow* GetWindow()
	{
		return s_Window;
	}

	void ClearWindow()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	Camera* s_ActiveCamera = nullptr;

	void SetActiveCamera(Camera* camera)
	{
		if (s_ActiveCamera)
		{
			s_ActiveCamera->SetActive(false);
		}
		s_ActiveCamera = camera;
		if (s_ActiveCamera)
		{
			s_ActiveCamera->SetActive(true);
		}
	}

	Camera* GetActiveCamera()
	{
		return s_ActiveCamera;
	}

	std::set<Renderable*> m_RenderablesInScene;

	unsigned int s_PeakVertsInScene = 0;

	void RenderScene()
	{
		unsigned int num_meshes = 0;
		unsigned int num_verts = 0;
		if (s_ActiveCamera)
		{
			ShaderManager::SetUniformMatrix4fv("camera_projection", s_ActiveCamera->GetProjMatrix());
			ShaderManager::SetUniformMatrix4fv("camera_view", s_ActiveCamera->GetViewMatrix());

			glm::vec3 ambient_colour(1.0f, 0.0f, 1.0f);
			ShaderManager::SetUniform3fv("lighting_ambient_colour", ambient_colour);

			glm::vec3 diffuse_position(-10.0f, 0.0f, 0.0f);
			ShaderManager::SetUniform3fv("lighting_directional_vector", diffuse_position);

			glm::vec3 diffuse_colour(0.5f, 0.5f, 0.5f);
			ShaderManager::SetUniform3fv("lighting_directional_colour", diffuse_colour);

			glm::vec3 camera_pos = s_ActiveCamera->GetPosition();
			ShaderManager::SetUniform3fv("camera_position", diffuse_colour);

			for (auto it : m_RenderablesInScene)
			{
				if (it->IsActive())
				{
					it->Render();
					num_meshes += it->GetNumMeshes();
					num_verts += it->GetNumVerts();
				}
			}
		}

		if (num_verts > s_PeakVertsInScene)
		{
			s_PeakVertsInScene = num_verts;
			std::stringstream str;
			str << "New peak verts: " << s_PeakVertsInScene << " (" << num_meshes << " meshes in scene)";
			Logging::Log("Renderer", str.str());
		}
	}

	void RegisterRenderable(Renderable* renderable)
	{
		m_RenderablesInScene.insert(renderable);
	}

	void UnregisterRenderable(Renderable* renderable)
	{
		m_RenderablesInScene.erase(renderable);
	}

}
