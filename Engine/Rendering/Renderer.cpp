#include "Engine/Pch.h"

#include "Engine/Rendering/Renderable.h"
#include "Engine/Rendering/Renderer.h"

#include <set>

namespace Renderer
{
	void Initialize()
	{
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	}

	void Terminate()
	{
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
		glfwSwapInterval(1);
		glEnable(GL_DEPTH_TEST);

		glewExperimental = GL_TRUE;
		glewInit();
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	std::set<Renderable*> m_RenderablesInScene;

	void RenderScene()
	{
		for (auto it : m_RenderablesInScene)
		{
			it->Render();
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
