#include "Engine/Pch.h"

#include "Engine/Cameras/Camera.h"
#include "Engine/Logging/Logging.h"
#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"
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

	void Initialize()
	{
		glfwSetErrorCallback(GLFWErrorFunction);
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	}

	void Terminate()
	{
		glfwTerminate();
	}

	GLFWwindow* s_Window = nullptr;

	void CreateWindow(unsigned int width, unsigned int height, bool full_screen)
	{
		DestroyWindow();
		if (full_screen)
		{
			s_Window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
		}
		else
		{
			s_Window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr); // Windowed
		}

		glfwMakeContextCurrent(s_Window);
		CheckGLError();
		glfwSwapInterval(1);
		CheckGLError();
		glEnable(GL_DEPTH_TEST);
		CheckGLError();

		glewExperimental = GL_TRUE;
		glewInit();
		CheckGLError();
	}

	void DestroyWindow()
	{
		if (s_Window)
		{
			glfwDestroyWindow(s_Window);
			s_Window = nullptr;
		}
	}

	GLFWwindow* GetWindow()
	{
		return s_Window;
	}

	void ClearWindow()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		CheckGLError();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		CheckGLError();
	}

	Camera* s_ActiveCamera = nullptr;

	void SetActiveCamera(Camera* camera)
	{
		s_ActiveCamera = camera;
	}

	std::set<Renderable*> m_RenderablesInScene;

	void RenderScene()
	{
		if (s_ActiveCamera)
		{
			ShaderManager::SetUniform4fv("view", glm::mat4(s_ActiveCamera->GetViewMatrix()));

			for (auto it : m_RenderablesInScene)
			{
				it->Render();
			}
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
